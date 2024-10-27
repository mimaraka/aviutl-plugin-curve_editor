import * as d3 from 'd3';
import React, { useEffect, useRef } from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faUpRightAndDownLeftFromCenter } from '@fortawesome/free-solid-svg-icons';
import { config, graphEditor } from './host_object';
import { NormalCurve, NumericCurve, BezierCurve, ElasticCurve, BounceCurve } from './curve';
import { Handles, BezierHandles, ElasticHandles, BounceHandles, NormalHandles, createHandles } from './handles';
import './scss/editor_graph.scss';


class ImageObject {
    #image;
    #d3Image;
    #loaded = false;
    constructor(group: d3.Selection<SVGGElement, unknown, HTMLElement, any>) {
        this.#image = new Image();
        this.#d3Image = group.append('svg:image');
    }

    load(src: string, width: number, height: number) {
        this.#loaded = false;
        this.#d3Image
            .attr('xlink:href', src)
            .attr('opacity', config.backgroundImageOpacity);
        this.#image.onload = () => {
            this.#loaded = true;
            this.resize(width, height);
        };
        this.#image.src = src;
    }

    loaded() { return this.#loaded; }

    resize(width: number, height: number) {
        if (this.loaded()) {
            const imageWidth = this.#image.width;
            const imageHeight = this.#image.height;
            const imageAspectRatio = imageWidth / imageHeight;
            const aspectRatio = width / height;
            if (aspectRatio < imageAspectRatio) {
                const scale = height / imageHeight;
                this.#d3Image
                    .attr('width', null)
                    .attr('height', height)
                    .attr('x', (width - imageWidth * scale) / 2)
                    .attr('y', 0);
            } else {
                const scale = width / imageWidth;
                this.#d3Image
                    .attr('width', width)
                    .attr('height', null)
                    .attr('x', 0)
                    .attr('y', (height - imageHeight * scale) / 2);
            }
        }
    }
}

class GraphEditor {
    #width: number;
    #height: number;

    #editMode: number;
    #idx: number;

    #normalCurve: NormalCurve;
    #valueCurve: any;
    #bezierCurve: BezierCurve;
    #elasticCurve: ElasticCurve;
    #bounceCurve: BounceCurve;

    #originalScaleX;
    #originalScaleY;
    #currentScaleX;
    #currentScaleY;

    // D3.jsの要素
    #svg: d3.Selection<SVGSVGElement, unknown, HTMLElement, unknown>;
    #group;
    #backgroundImage;
    #axisX;
    #axisY;
    #gridX;
    #gridY;
    #subAxisX;
    #subAxisY;
    #subGridX;
    #subGridY;
    #zoomGroup;
    #velocityPath;
    #curvePath;
    #leftRect;
    #rightRect;
    #handles: Handles | BezierHandles | ElasticHandles| BounceHandles | NormalHandles | null;
    #labelAxisX;
    #labelAxisY;
    #labelGridX;
    #labelGridY;
    #d3Curve;
    #zoomBehavior;

    constructor(editMode: number, idx: number, normalCurveId: number, valueCurveId: number, bezierCurveId: number, elasticCurveId: number, bounceCurveId: number) {
        const container = document.getElementById('container');
        this.#width = container!.clientWidth;
        this.#height = container!.clientHeight;

        this.#editMode = editMode;
        this.#idx = idx;

        this.#normalCurve = new NormalCurve(normalCurveId);
        //this.#valueCurve = new ValueCurve(valueCurveId);
        this.#bezierCurve = new BezierCurve(bezierCurveId);
        this.#elasticCurve = new ElasticCurve(elasticCurveId);
        this.#bounceCurve = new BounceCurve(bounceCurveId);

        this.#svg = d3.select<SVGSVGElement, unknown>('#canvas')
            .attr('width', this.#width)
            .attr('height', this.#height)
            .on('mousedown', event => {
                if (event.button === 1) {
                    event.preventDefault();
                } else if (event.button === 2) {
                    event.preventDefault();
                    window.chrome.webview.postMessage({
                        command: 'contextmenu-graph',
                        mode: editMode,
                        curveId: this.getCurrentCurve().id
                    });
                }
            })
            .on('dblclick', event => {
                const x = this.#currentScaleX.invert(event.clientX - container!.offsetLeft);
                const y = this.#currentScaleY.invert(event.clientY - container!.offsetTop);
                if (this.#handles instanceof NormalHandles && x >= 0 && x <= 1) {
                    (this.#handles.curve as NormalCurve).addCurve(x, y, this.#currentScaleX(1) - this.#currentScaleX(0));
                    this.updateCurvePath();
                    this.updateVelocityPath();
                    this.updateHandles();
                }
            })
            .on('contextmenu', event => {
                event.preventDefault();
            })
            .on('wheel', event => {
                event.preventDefault();
            });

        this.#group = this.#svg.append('g');
        // 背景画像を追加
        this.#backgroundImage = new ImageObject(this.#group);
        if (config.setBackgroundImage) {
            this.#backgroundImage.load(config.backgroundImagePath, this.#width, this.#height);
        }

        const graphMarginX = Math.min(50, this.#width * 0.1, this.#height * 0.1);
        const innerGraphWidth = this.#width - graphMarginX * 2;
        const innerGraphHeight = Math.min(this.#height - 2 * graphMarginX, innerGraphWidth);
        const graphMarginY = (this.#height - innerGraphHeight) / 2;
        const nTick = 5;

        // スケール・軸・グリッド(X)を追加
        this.#currentScaleX = this.#originalScaleX = d3.scaleLinear()
            .domain([-graphMarginX / innerGraphWidth, 1 + graphMarginX / innerGraphWidth])
            .range([-2, this.#width + 2]);
        this.#axisX = d3.axisBottom(this.#originalScaleX)
            .ticks(this.#width / this.#height * nTick)
            .tickSize(-this.#height)
            .tickFormat(() => '');
        this.#gridX = this.#group.append('g')
            .attr('class', 'grid')
            .attr('transform', `translate(0,${this.#height})`)
            .call(this.#axisX);

        // スケール・軸・グリッド(Y)を追加
        this.#currentScaleY = this.#originalScaleY = d3.scaleLinear()
            .domain([-graphMarginY / innerGraphHeight, 1 + graphMarginY / innerGraphHeight])
            .range([this.#height + 2, -2]);
        this.#axisY = d3.axisLeft(this.#originalScaleY)
            .ticks(nTick)
            .tickSize(-this.#width)
            .tickFormat(() => '');
        this.#gridY = this.#group.append('g')
            .attr('class', 'grid')
            .call(this.#axisY);
        
        // サブグリッド(X)を追加
        this.#subAxisX = d3.axisBottom(this.#originalScaleX)
            .ticks(this.#width / this.#height * nTick * 2)
            .tickSize(-this.#height)
            .tickFormat(() => '');
        this.#subGridX = this.#group.append('g')
            .attr('class', 'grid-sub')
            .attr('transform', `translate(0,${this.#height})`)
            .call(this.#subAxisX);

        // サブグリッド(Y)を追加
        this.#subAxisY = d3.axisLeft(this.#originalScaleY)
            .ticks(nTick * 2)
            .tickSize(-this.#width)
            .tickFormat(() => '');
        this.#subGridY = this.#group.append('g')
            .attr('class', 'grid-sub')
            .call(this.#subAxisY);

        this.#zoomGroup = this.#group.append('g');

        this.#d3Curve = d3.curveCatmullRom.alpha(0.5);
        const line = d3.line<{ x: number; y: number }>()
            .x(d => this.#originalScaleX(d.x))
            .y(d => this.#originalScaleY(d.y))
            .curve(this.#d3Curve);

        this.#velocityPath = this.#zoomGroup.append('path')
            .datum(this.createPathData(0, 1, true))
            .attr('fill', 'none')
            .attr('class', 'velocity-path')
            .attr('stroke-width', config.curveThickness)
            .attr('d', line);
        this.updateVelocityGraphVisibility();

        this.#curvePath = this.#zoomGroup.append('path')
            .datum(this.createPathData(0, 1))
            .attr('fill', 'none')
            .attr('stroke', config.curveColor)
            .attr('stroke-width', config.curveThickness)
            .attr('d', line);

        // 範囲外領域描画用の矩形(左)
        this.#leftRect = this.#group.append('rect')
            .attr('x', 0)
            .attr('y', 0)
            .attr('width', this.#originalScaleX(0))
            .attr('height', this.#height)
            .attr('class', 'rect');

        // 範囲外領域描画用の矩形(右)
        this.#rightRect = this.#group.append('rect')
            .attr('x', this.#originalScaleX(1))
            .attr('y', 0)
            .attr('width', this.#width - this.#originalScaleX(1))
            .attr('height', this.#height)
            .attr('class', 'rect');

        // ハンドル
        this.#handles = createHandles(this.getCurrentCurve(), this.#group, this.#currentScaleX, this.#currentScaleY);
        this.updateHandleVisibility();

        this.#labelAxisX = (scale: d3.AxisScale<number>) => d3.axisBottom(scale).tickPadding(-15).tickFormat(d3.format(''));
        this.#labelAxisY = (scale: d3.AxisScale<number>) => d3.axisLeft(scale).tickPadding(-15).tickFormat(d3.format(''));

        // 軸ラベル(X軸)
        this.#labelGridX = this.#group.append('g')
            .attr('class', 'axis-label')
            .attr('transform', `translate(0,${this.#height - 5})`)
            .style('visibility', config.showXLabel ? 'visible' : 'hidden')
            .call(this.#labelAxisX(this.#originalScaleX).tickSize(0));

        // 軸ラベル(Y軸)
        this.#labelGridY = this.#group.append('g')
            .attr('class', 'axis-label axis-label-y')
            .style('visibility', config.showYLabel ? 'visible' : 'hidden')
            .call(this.#labelAxisY(this.#originalScaleY).tickSize(0));

        this.#zoomBehavior = d3.zoom<SVGSVGElement, unknown>()
            .scaleExtent([0.0001, 10000])
            .translateExtent([[-this.#width, -Infinity], [2 * this.#width, Infinity]])
            .wheelDelta(event => {
                let ret = -event.deltaY;
                if (config.invertWheel) {
                    ret *= -1;
                }
                if (event.ctrlKey) {
                    ret *= 5;
                }
                if (event.shiftKey) {
                    ret *= 0.2;
                }
                if (event.deltaMode === 1) {
                    ret *= 0.05;
                }
                else if (!event.deltaMode) {
                    ret *= 0.002;
                }
                return ret;
            })
            .on('start', this.onZoomStart.bind(this))
            .on('zoom', this.onZoom.bind(this))
            .on('end', this.onZoomEnd.bind(this))
            .filter(event => {
                // ホイールクリックまたはAlt+左クリックの場合のみパンを有効にする
                return event.type == 'wheel'
                || event.button === 1
                || (event.button === 0 && event.altKey);
            });

        this.#svg.call(this.#zoomBehavior);
    }

    get backgroundImage() { return this.#backgroundImage; }
    get handles() { return this.#handles; }

    setEditMode(editMode: number) {
        if (editMode != this.#editMode) {
            this.#editMode = editMode;
            this.updateHandles();
            this.updateCurvePath();
            this.updateVelocityPath();
            this.fit(0);
        }
    }

    setIdx(idx: number) {
        if (idx != this.#idx) {
            this.#idx = idx;
             switch (this.#editMode) {
                case 0:
                    this.#normalCurve = new NormalCurve(graphEditor.normal.getId(idx));
                    break;
                case 1:
                    //this.#valueCurve = new ValueCurve(graphEditor.value.getId(getIdx()));
                    break;
            }
            this.updateHandles();
            this.updateCurvePath();
            this.updateVelocityPath();
            this.fit(0);
        }
    }

    getCurrentCurve() {
        switch (this.#editMode) {
            case 0:
                return this.#normalCurve;
            case 1:
                return this.#valueCurve;
            case 2:
                return this.#bezierCurve;
            case 3:
                return this.#elasticCurve;
            case 4:
                return this.#bounceCurve;
            default:
                return null;
        }
    }

    encode() {
        const curve = this.getCurrentCurve();
        if (curve instanceof NumericCurve) {
            return curve.encode();
        }
    }

    decode(code: number) {
        const curve = this.getCurrentCurve();
        if (curve instanceof NumericCurve) {
            curve.decode(code);
            this.updateCurvePath();
            this.updateHandlePos();
        }
    }

    // D3.js用のデータを生成
    createPathData(startGraphX: number, endGraphX: number, velocity = false) {
        const n = config.curveResolution;
        let data: {
            x: number;
            y: number;
        }[] = [];
        let yArray;
        const currentCurve = this.getCurrentCurve();
        if (!currentCurve) {
            return data;
        }
        if (velocity) {
            yArray = graphEditor.getCurveVelocityArray(currentCurve.id, startGraphX, 0, endGraphX, 1, n);
        } else {
            yArray = graphEditor.getCurveValueArray(currentCurve.id, startGraphX, 0, endGraphX, 1, n);
        } 
        for (let i = 0; i < n; i++) {
            const x = startGraphX + (endGraphX - startGraphX) / (n - 1) * i;
            data.push({
                x: x,
                y: yArray[i]
            });
        }
        return data;
    }

    // 描画するカーブの内容を更新
    updateCurvePath(t: d3.Transition<any, unknown, any, unknown> | null = null, zoom: boolean = false) {
        const startGraphX = Math.max(0, this.#currentScaleX.invert(zoom ? -200 : 0));
        const endGraphX = Math.min(1, this.#currentScaleX.invert(this.#width + (zoom ? 200 : 0)));

        this.#curvePath.datum(this.createPathData(startGraphX, endGraphX));
        (t ? this.#curvePath.transition(t) : this.#curvePath)
            .attr('d', d3.line<{ x: number; y: number }>()
                .x(d => this.#originalScaleX(d.x))
                .y(d => this.#originalScaleY(d.y))
                .curve(this.#d3Curve)
            );

        if (!zoom) {
            if (this.#editMode == 2 || this.#editMode == 3 || this.#editMode == 4) {
                window.postMessage({
                command: 'updateParam'
                }, '*');
            }
        }
    }

    updateCurvePathStyle() {
        this.#curvePath.attr('stroke', config.curveColor)
            .attr('stroke-width', config.curveThickness);
    }

    // 描画するカーブの内容を更新
    updateVelocityPath(t: d3.Transition<any, unknown, any, unknown> | null = null) {
        if (!config.showVelocityGraph) {
            return;
        }
        const startGraphX = Math.max(0, this.#currentScaleX.invert(0));
        const endGraphX = Math.min(1, this.#currentScaleX.invert(this.#width));

        this.#velocityPath.datum(this.createPathData(startGraphX, endGraphX, true));
        (t ? this.#velocityPath.transition(t) : this.#velocityPath).attr('d',
            d3.line<{ x: number; y: number }>()
                .x(d => this.#originalScaleX(d.x))
                .y(d => this.#originalScaleY(d.y))
                .curve(this.#d3Curve)
        );
    }

    updateVelocityGraphVisibility() {
        this.#velocityPath.style('visibility', config.showVelocityGraph ? 'visible' : 'hidden');
        this.updateVelocityPath();
    }

    updateHandles() {
        this.#handles?.remove();
        this.#handles = createHandles(this.getCurrentCurve(), this.#group, this.#currentScaleX, this.#currentScaleY);
    }

    updateHandlePos(t: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.#handles?.updateHandles(t);
    }

    updateHandleVisibility() {
        document.querySelectorAll<HTMLElement>('.anchor, .handle, .handle-line').forEach((element) => {
            element.style.visibility = config.showHandle ? 'visible' : 'hidden';
        });
    }

    updateAxisLabelVisibility() {
        this.#labelGridX.style('visibility', config.showXLabel ? 'visible' : 'hidden');
        this.#labelGridY.style('visibility', config.showYLabel ? 'visible' : 'hidden');
    }

    loadBackgroundImage(src: string) {
        this.#backgroundImage.load(src, this.#width, this.#height);
    }

    onZoomStart(event: any) {
        if (event.sourceEvent && event.sourceEvent.type !== 'wheel') {
            this.#svg.attr('cursor', 'move');
        }
    }

    onZoom(event: any) {
        const fit = document.getElementById('fit');
        if (event.transform.k == 1 && event.transform.x == 0 && event.transform.y == 0) {
            if (fit?.classList.contains('visible')) {
                fit?.classList.remove('visible');
                fit?.classList.add('hidden');
            }
        }
        else {
            if (fit?.classList.contains('hidden')) {
                fit?.classList.remove('hidden');
                fit?.classList.add('visible');
            }
        }

        this.#currentScaleX = event.transform.rescaleX(this.#originalScaleX);
        this.#currentScaleY = event.transform.rescaleY(this.#originalScaleY);

        const duration = config.enableAnimation ? 180 : 0;
        const t = d3.transition().duration(duration).ease(d3.easeCubicOut);

        this.updateCurvePath(null, true);
        this.updateVelocityPath();

        const isZoom = event.sourceEvent && event.sourceEvent.type === 'wheel';

        if (isZoom) {
            this.#gridX.transition(t).call(this.#axisX.scale(this.#currentScaleX));
            this.#gridY.transition(t).call(this.#axisY.scale(this.#currentScaleY));
            this.#subGridX.transition(t).call(this.#subAxisX.scale(this.#currentScaleX));
            this.#subGridY.transition(t).call(this.#subAxisY.scale(this.#currentScaleY));
            this.#labelGridX.transition(t).call(this.#labelAxisX(this.#currentScaleX));
            this.#labelGridY.transition(t).call(this.#labelAxisY(this.#currentScaleY));
            this.#zoomGroup.transition(t).attr('transform', event.transform);
            this.#leftRect.transition(t).attr('width', Math.max(0, this.#currentScaleX(0)));
            this.#rightRect.transition(t)
                .attr('x', this.#currentScaleX(1))
                .attr('width', Math.max(0, this.#width - this.#currentScaleX(1)));
            this.#curvePath.transition(t)
                .attr('stroke-width', config.curveThickness / event.transform.k);
            this.#velocityPath.transition(t)
                .attr('stroke-width', config.curveThickness / event.transform.k);
        } else {
            this.#gridX.call(this.#axisX.scale(this.#currentScaleX));
            this.#gridY.call(this.#axisY.scale(this.#currentScaleY));
            this.#subGridX.call(this.#subAxisX.scale(this.#currentScaleX));
            this.#subGridY.call(this.#subAxisY.scale(this.#currentScaleY));
            this.#labelGridX.call(this.#labelAxisX(this.#currentScaleX));
            this.#labelGridY.call(this.#labelAxisY(this.#currentScaleY));
            this.#zoomGroup.attr('transform', event.transform);
            this.#leftRect.attr('width', Math.max(0, this.#currentScaleX(0)));
            this.#rightRect.attr('x', this.#currentScaleX(1))
                .attr('width', Math.max(0, this.#width - this.#currentScaleX(1)));
        }
        this.#handles?.rescaleX(this.#currentScaleX, isZoom? t : null);
        this.#handles?.rescaleY(this.#currentScaleY, isZoom? t : null);
    }

    onZoomEnd(event: any) {
        this.#svg.attr('cursor', 'default');
    }

    fit(customDuration = 700) {
        const duration = config.enableAnimation ? customDuration : 0;
        const t = d3.transition().duration(duration);
        this.#svg.transition(t)
            .call(this.#zoomBehavior.transform, d3.zoomIdentity);
        this.#curvePath.transition(t)
            .attr('stroke-width', config.curveThickness);
        this.#velocityPath.transition(t)
            .attr('stroke-width', config.curveThickness);
    }

    resize(width: number, height: number) {
        this.#width = width;
        this.#height = height;

        const graphMarginX = Math.min(50, width * 0.1, height * 0.1);
        const innerGraphWidth = width - graphMarginX * 2;
        const innerGraphHeight = Math.min(height - 2 * graphMarginX, innerGraphWidth);
        const graphMarginY = (height - innerGraphHeight) / 2;

        this.#svg
            .attr('width', width)
            .attr('height', height);

        this.#originalScaleX
            .domain([-graphMarginX / innerGraphWidth, 1 + graphMarginX / innerGraphWidth])
            .range([-2, width + 2]);
        this.#originalScaleY
            .domain([-graphMarginY / innerGraphHeight, 1 + graphMarginY / innerGraphHeight])
            .range([height + 2, -2]);
        
        let transform: d3.ZoomTransform | null = null;
        const svgNode = this.#svg.node();
        if (svgNode) {
            transform = d3.zoomTransform(svgNode);
        }

        this.#currentScaleX = transform?.rescaleX(this.#originalScaleX) ?? this.#originalScaleX;
        this.#currentScaleY = transform?.rescaleY(this.#originalScaleY) ?? this.#originalScaleY;

        this.#backgroundImage.resize(width, height);
        this.#gridX.call(this.#axisX.scale(this.#currentScaleX).tickSize(-height)).attr('transform', `translate(0,${height})`);
        this.#gridY.call(this.#axisY.scale(this.#currentScaleY).tickSize(-width));
        this.#subGridX.call(this.#subAxisX.scale(this.#currentScaleX).tickSize(-height)).attr('transform', `translate(0,${height})`);
        this.#subGridY.call(this.#subAxisY.scale(this.#currentScaleY).tickSize(-width));
        this.#labelGridX.call(this.#labelAxisX(this.#currentScaleX));
        this.#labelGridY.call(this.#labelAxisY(this.#currentScaleY));
        this.#curvePath.attr('d', d3.line<{ x: number; y: number }>()
            .x(d => this.#originalScaleX(d.x))
            .y(d => this.#originalScaleY(d.y))
            .curve(this.#d3Curve));
        this.#velocityPath.attr('d', d3.line<{ x: number; y: number }>()
            .x(d => this.#originalScaleX(d.x))
            .y(d => this.#originalScaleY(d.y))
            .curve(this.#d3Curve));
        this.#leftRect.attr('width', this.#currentScaleX(0))
            .attr('height', height);
        this.#rightRect.attr('x', this.#currentScaleX(1))
            .attr('width', width - this.#currentScaleX(1))
            .attr('height', height);
        this.#handles?.rescaleX(this.#currentScaleX);
        this.#handles?.rescaleY(this.#currentScaleY);
    }
}


interface GraphEditorPanelProps {
    isSelectDialog: boolean;
    editMode: number;
    idx: number;
    size: number;
    setIdx: (idx: number) => void;
}

const GraphEditorPanel: React.FC<GraphEditorPanelProps> = (props: GraphEditorPanelProps) => {
    const ref = useRef<HTMLDivElement | null>(null);
    const editorRef = useRef<GraphEditor | null>(null);

    editorRef.current?.setEditMode(props.editMode);
    editorRef.current?.setIdx(props.idx);

    const updateHandlePos = () => {
        if (editorRef.current?.handles instanceof NormalHandles && editorRef.current?.handles.segmentHandlesArray.length > 1) {
            editorRef.current?.updateHandles();
            editorRef.current?.updateCurvePath();
            editorRef.current?.updateVelocityPath();
        }
        else {
            const duration = config.enableAnimation ? 180 : 0;
            const t = d3.transition().duration(duration).ease(d3.easeCubicOut);
            editorRef.current?.updateHandlePos(t);
            editorRef.current?.updateCurvePath(t);
            editorRef.current?.updateVelocityPath(t);
        }
    }

    const onMessage = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'setCurve':
                if (event.data.mode != undefined && event.data.param != undefined) {
                    editorRef.current?.setEditMode(event.data.mode);
                    if (event.data.mode == 2 || event.data.mode == 3 || event.data.mode == 4) {
                        editorRef.current?.decode(event.data.param);
                    } else {
                        editorRef.current?.setIdx(event.data.param);
                    }
                }
                break;

            case 'updateCurvePath':
                editorRef.current?.updateCurvePath();
                editorRef.current?.updateVelocityPath();
                break;
        }
    }

    const onMessageFromNative = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'updateCurvePath':
                editorRef.current?.updateCurvePath();
                editorRef.current?.updateVelocityPath();
                break;

            case 'updateEditor':
            case 'updateHandles':
                editorRef.current?.updateHandles();
                editorRef.current?.updateCurvePath();
                editorRef.current?.updateVelocityPath();
                break;

            case 'updateHandlePos':
                updateHandlePos();
                break;

            case 'updateAxisLabelVisibility':
                editorRef.current?.updateAxisLabelVisibility();
                break;

            case 'updateHandleVisibility':
                editorRef.current?.updateHandleVisibility();
                break;

            case 'updateVelocityGraphVisibility':
                editorRef.current?.updateVelocityGraphVisibility();
                break;

            case 'applyPreferences':
                editorRef.current?.loadBackgroundImage(config.setBackgroundImage ? config.backgroundImagePath : '');
                editorRef.current?.updateCurvePathStyle();
                editorRef.current?.updateCurvePath();
                editorRef.current?.updateVelocityPath();
                break;
        }
    };

    const onKeyDown = (event: KeyboardEvent) => {
        if (!config.enableHotkeys) {
            return;
        }
        switch (event.key) {
        case 'Home':
            editorRef.current?.fit();
            break;

        case 'Delete':
            window.chrome.webview.postMessage({
                command: 'clear'
            });
            break;

        case 'r':
            editorRef.current?.getCurrentCurve().reverse();
            updateHandlePos();
            break;

        case 'a':
            config.alignHandle = !config.alignHandle;
            break;

        case 'ArrowLeft':
            if (0 < props.idx) {
                props.setIdx(props.idx - 1);
            }
            break

        case 'ArrowRight':
            if (props.idx < props.size - 1) {
                props.setIdx(props.idx + 1);
            }
            break;
        }
    }

    useEffect(() => {
        editorRef.current = new GraphEditor(
            props.editMode,
            props.idx,
            graphEditor.normal.getId(props.idx),
            0,
            graphEditor.bezier.getId(props.isSelectDialog),
            graphEditor.elastic.getId(props.isSelectDialog),
            graphEditor.bounce.getId(props.isSelectDialog)
        );

        window.addEventListener('message', onMessage);
        window.chrome.webview.addEventListener('message', onMessageFromNative);
        window.addEventListener('keydown', onKeyDown);

        const observer = new ResizeObserver((entries) => {
            entries.forEach((entry) => {
                editorRef.current?.resize(entry.contentRect.width, entry.contentRect.height);
            });
        });
        observer.observe(ref.current!);

        return () => {
            window.removeEventListener('message', onMessage);
            window.chrome.webview.removeEventListener('message', onMessageFromNative);
            window.removeEventListener('keydown', onKeyDown);
            observer.disconnect();
        };
    }, []);

    return (
        <div id='container' ref={ref}>
            <svg id='canvas'></svg>
            <button id='fit' className='hidden' title='ビューをフィット' onClick={() => { editorRef.current?.fit(); }}>
                <FontAwesomeIcon icon={faUpRightAndDownLeftFromCenter} size='sm' />
            </button>
        </div>
    );
}

export default GraphEditorPanel;