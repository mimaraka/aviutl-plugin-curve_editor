'use strict';

class ImageObject {
    #image;
    #d3Image;
    #loaded = false;
    constructor(group) {
        this.#image = new Image();
        this.#d3Image = group.append('svg:image');
    }

    load(src, width, height) {
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

    resize(width, height) {
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
    #width;
    #height;

    #normalCurve;
    #valueCurve;
    #bezierCurve;
    #elasticCurve;
    #bounceCurve;

    #originalScaleX;
    #originalScaleY;
    #currentScaleX;
    #currentScaleY;

    // D3.jsの要素
    #svg;
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
    #handles;
    #labelAxisX;
    #labelAxisY;
    #labelGridX;
    #labelGridY;
    #d3Curve;
    #zoomBehavior;

    constructor(normalCurveId, valueCurveId, bezierCurveId, elasticCurveId, bounceCurveId) {
        this.#width = document.documentElement.clientWidth;
        this.#height = document.documentElement.clientHeight;

        this.#normalCurve = new NormalCurve(normalCurveId);
        //this.#valueCurve = new ValueCurve(valueCurveId);
        this.#bezierCurve = new BezierCurve(bezierCurveId);
        this.#elasticCurve = new ElasticCurve(elasticCurveId);
        this.#bounceCurve = new BounceCurve(bounceCurveId);

        this.#svg = d3.select('#canvas')
            .attr('width', this.#width)
            .attr('height', this.#height)
            .on('mousedown', event => {
                if (event.button === 1) {
                    event.preventDefault();
                } else if (event.button === 2) {
                    event.preventDefault();
                    window.top.postMessage({
                        to: 'native',
                        command: 'contextmenu-graph',
                        mode: getEditMode(),
                        curveId: this.getCurrentCurve().id
                    }, '*');
                }
            })
            .on('dblclick', event => {
                const x = this.#currentScaleX.invert(event.clientX);
                const y = this.#currentScaleY.invert(event.clientY);
                if (this.#handles instanceof NormalHandles && x >= 0 && x <= 1) {
                    this.#handles.curve.addCurve(x, y, this.#currentScaleX(1) - this.#currentScaleX(0));
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
            .tickFormat('');
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
            .tickFormat('');
        this.#gridY = this.#group.append('g')
            .attr('class', 'grid')
            .call(this.#axisY);
        
        // サブグリッド(X)を追加
        this.#subAxisX = d3.axisBottom(this.#originalScaleX)
            .ticks(this.#width / this.#height * nTick * 2)
            .tickSize(-this.#height)
            .tickFormat('');
        this.#subGridX = this.#group.append('g')
            .attr('class', 'grid-sub')
            .attr('transform', `translate(0,${this.#height})`)
            .call(this.#subAxisX);

        // サブグリッド(Y)を追加
        this.#subAxisY = d3.axisLeft(this.#originalScaleY)
            .ticks(nTick * 2)
            .tickSize(-this.#width)
            .tickFormat('');
        this.#subGridY = this.#group.append('g')
            .attr('class', 'grid-sub')
            .call(this.#subAxisY);

        this.#zoomGroup = this.#group.append('g');

        this.#d3Curve = d3.curveCatmullRom.alpha(0.5);
        const line = d3.line()
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

        this.#labelAxisX = scale => d3.axisBottom(scale).tickPadding(-15).tickFormat(d3.format('.6'));
        this.#labelAxisY = scale => d3.axisLeft(scale).tickPadding(-15).tickFormat(d3.format('.6'));

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

        this.#zoomBehavior = d3.zoom()
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

    getCurrentCurve() {
        switch (getEditMode()) {
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

    decode(code) {
        const curve = this.getCurrentCurve();
        if (curve instanceof NumericCurve) {
            curve.decode(code);
            this.updateCurvePath();
            this.updateHandlePos();
        }
    }

    updateIdCurve() {
        switch (getEditMode()) {
            case 0:
                this.#normalCurve = new NormalCurve(graphEditor.normal.getId(getIdx()));
                break;
            case 1:
                //this.#valueCurve = new ValueCurve(graphEditor.value.getId(getIdx()));
                break;
        }
    }

    // D3.js用のデータを生成
    createPathData(startGraphX, endGraphX, velocity = false) {
        const n = config.curveResolution;
        let data = [];
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
    updateCurvePath(t = null, zoom = false) {
        const startGraphX = Math.max(0, this.#currentScaleX.invert(zoom ? -200 : 0));
        const endGraphX = Math.min(1, this.#currentScaleX.invert(this.#width + (zoom ? 200 : 0)));

        this.#curvePath.datum(this.createPathData(startGraphX, endGraphX));
        (t ? this.#curvePath.transition(t) : this.#curvePath).attr('d', d3.line()
            .x(d => this.#originalScaleX(d.x))
            .y(d => this.#originalScaleY(d.y))
            .curve(this.#d3Curve));

        if (!zoom) {
            const editMode = getEditMode();
            if (editMode == 2 || editMode == 3 || editMode == 4) {
                window.top.postMessage({
                to: 'panel-editor',
                command: 'updateParam'
                });
            }
        }
    }

    updateCurvePathStyle() {
        this.#curvePath.attr('stroke', config.curveColor)
            .attr('stroke-width', config.curveThickness);
    }

    // 描画するカーブの内容を更新
    updateVelocityPath(t = null) {
        if (!config.showVelocityGraph) {
            return;
        }
        const startGraphX = Math.max(0, this.#currentScaleX.invert(0));
        const endGraphX = Math.min(1, this.#currentScaleX.invert(this.#width));

        this.#velocityPath.datum(this.createPathData(startGraphX, endGraphX, true));
        (t ? this.#velocityPath.transition(t) : this.#velocityPath).attr('d', d3.line()
            .x(d => this.#originalScaleX(d.x))
            .y(d => this.#originalScaleY(d.y))
            .curve(this.#d3Curve));
    }

    updateVelocityGraphVisibility() {
        this.#velocityPath.style('visibility', config.showVelocityGraph ? 'visible' : 'hidden');
        this.updateVelocityPath();
    }

    updateHandles() {
        this.#handles.remove();
        this.#handles = createHandles(this.getCurrentCurve(), this.#group, this.#currentScaleX, this.#currentScaleY);
    }

    updateHandlePos(t = null) {
        this.#handles.updateHandles(t);
    }

    updateHandleVisibility() {
        $('.anchor, .handle, .handle-line').css('visibility', config.showHandle ? 'visible' : 'hidden');
    }

    updateAxisLabelVisibility() {
        this.#labelGridX.style('visibility', config.showXLabel ? 'visible' : 'hidden');
        this.#labelGridY.style('visibility', config.showYLabel ? 'visible' : 'hidden');
    }

    loadBackgroundImage(src) {
        this.#backgroundImage.load(src, this.#width, this.#height);
    }

    onZoomStart(event) {
        if (event.sourceEvent && event.sourceEvent.type !== 'wheel') {
            this.#svg.attr('cursor', 'move');
        }
    }

    onZoom(event) {
        if (event.transform.k == 1 && event.transform.x == 0 && event.transform.y == 0) {
            if ($('#fit').hasClass('visible')) {
                $('#fit').removeClass('visible');
                $('#fit').addClass('hidden');
            }
        }
        else {
            if ($('#fit').hasClass('hidden')) {
                $('#fit').addClass('visible');
                $('#fit').removeClass('hidden');
            }
        }

        this.#currentScaleX = event.transform.rescaleX(this.#originalScaleX);
        this.#currentScaleY = event.transform.rescaleY(this.#originalScaleY);

        const duration = config.enableAnimation ? 180 : 0;
        const t = d3.transition().duration(duration).ease(d3.easeCubicOut);

        this.updateCurvePath(null, true);
        this.updateVelocityPath();

        const isZoom = event.sourceEvent && event.sourceEvent.type === 'wheel';

        (isZoom? this.#gridX.transition(t) : this.#gridX).call(this.#axisX.scale(this.#currentScaleX));
        (isZoom? this.#gridY.transition(t) : this.#gridY).call(this.#axisY.scale(this.#currentScaleY));
        (isZoom? this.#subGridX.transition(t) : this.#subGridX).call(this.#subAxisX.scale(this.#currentScaleX));
        (isZoom? this.#subGridY.transition(t) : this.#subGridY).call(this.#subAxisY.scale(this.#currentScaleY));
        (isZoom? this.#labelGridX.transition(t) : this.#labelGridX).call(this.#labelAxisX(this.#currentScaleX));
        (isZoom? this.#labelGridY.transition(t) : this.#labelGridY).call(this.#labelAxisY(this.#currentScaleY));
        (isZoom? this.#zoomGroup.transition(t) : this.#zoomGroup).attr('transform', event.transform);
        (isZoom? this.#leftRect.transition(t) : this.#leftRect).attr('width', Math.max(0, this.#currentScaleX(0)));
        (isZoom? this.#rightRect.transition(t) : this.#rightRect).attr('x', this.#currentScaleX(1))
            .attr('width', Math.max(0, this.#width - this.#currentScaleX(1)));
        this.#handles.rescaleX(this.#currentScaleX, isZoom? t : null);
        this.#handles.rescaleY(this.#currentScaleY, isZoom? t : null);

        if (isZoom) {
            this.#curvePath.transition(t)
                .attr('stroke-width', config.curveThickness / event.transform.k);
            this.#velocityPath.transition(t)
                .attr('stroke-width', config.curveThickness / event.transform.k);
        }
    }

    onZoomEnd(event) {
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

    resize(width, height) {
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
        
        const transform = d3.zoomTransform(this.#svg.node());

        this.#currentScaleX = transform.rescaleX(this.#originalScaleX);
        this.#currentScaleY = transform.rescaleY(this.#originalScaleY);

        this.#backgroundImage.resize(width, height);
        this.#gridX.call(this.#axisX.scale(this.#currentScaleX).tickSize(-height)).attr('transform', `translate(0,${height})`);
        this.#gridY.call(this.#axisY.scale(this.#currentScaleY).tickSize(-width));
        this.#subGridX.call(this.#subAxisX.scale(this.#currentScaleX).tickSize(-height)).attr('transform', `translate(0,${height})`);
        this.#subGridY.call(this.#subAxisY.scale(this.#currentScaleY).tickSize(-width));
        this.#labelGridX.call(this.#labelAxisX(this.#currentScaleX));
        this.#labelGridY.call(this.#labelAxisY(this.#currentScaleY));
        this.#curvePath.attr('d', d3.line()
            .x(d => this.#originalScaleX(d.x))
            .y(d => this.#originalScaleY(d.y))
            .curve(this.#d3Curve));
        this.#velocityPath.attr('d', d3.line()
            .x(d => this.#originalScaleX(d.x))
            .y(d => this.#originalScaleY(d.y))
            .curve(this.#d3Curve));
        this.#leftRect.attr('width', this.#currentScaleX(0))
            .attr('height', height);
        this.#rightRect.attr('x', this.#currentScaleX(1))
            .attr('width', width - this.#currentScaleX(1))
            .attr('height', height);
        this.#handles.rescaleX(this.#currentScaleX);
        this.#handles.rescaleY(this.#currentScaleY);
    }
}


$(document).ready(() => {
    window.graphEditor = new GraphEditor(
        graphEditor.normal.getId(getIdx()),
        null,
        graphEditor.bezier.getId(window.top.isSelectDialog),
        graphEditor.elastic.getId(window.top.isSelectDialog),
        graphEditor.bounce.getId(window.top.isSelectDialog)
    );
});


$(window).on('message', event => {
    const data = event.originalEvent.data;
    switch (data.command) {
        case 'changeCurve':
            window.graphEditor?.updateIdCurve();
            window.graphEditor?.updateCurvePath();
            window.graphEditor?.updateVelocityPath();
            window.graphEditor?.fit(0);
            window.graphEditor?.updateHandles();
            break;

        case 'updateCurvePath':
            window.graphEditor.updateCurvePath();
            window.graphEditor.updateVelocityPath();
            break;

        case 'updateEditor':
        case 'updateHandles':
            window.graphEditor.updateHandles();
            window.graphEditor.updateCurvePath();
            window.graphEditor.updateVelocityPath();
            break;

        case 'updateHandlePos':
            if (window.graphEditor.handles instanceof NormalHandles && window.graphEditor.handles.segmentHandlesArray.length > 1) {
                window.graphEditor.updateHandles();
                window.graphEditor.updateCurvePath();
                window.graphEditor.updateVelocityPath();
            }
            else {
                const duration = config.enableAnimation ? 180 : 0;
                const t = d3.transition().duration(duration).ease(d3.easeCubicOut);
                window.graphEditor.updateHandlePos(t);
                window.graphEditor.updateCurvePath(t);
                window.graphEditor.updateVelocityPath(t);
            }
            break;

        case 'updateAxisLabelVisibility':
            window.graphEditor.updateAxisLabelVisibility();
            break;

        case 'updateHandleVisibility':
            window.graphEditor.updateHandleVisibility();
            break;

        case 'updateVelocityGraphVisibility':
            window.graphEditor.updateVelocityGraphVisibility();
            break;

        case 'applyPreferences':
            window.graphEditor.loadBackgroundImage(config.setBackgroundImage ? config.backgroundImagePath : '');
            window.graphEditor.updateCurvePathStyle();
            window.graphEditor.updateCurvePath();
            window.graphEditor.updateVelocityPath();
            break;

        case 'decode':
            window.graphEditor.decode(data.code);
            break;
    }
});

$(window).on('keydown', event => {
    if (!config.enableHotkeys) {
        return;
    }
    switch (event.key) {
    case 'Home':
        window.graphEditor.fit();
        break;

    case 'Delete':
        window.top.postMessage({
            to: 'native',
            command: 'clear'
        }, '*');
        break;

    case 'r':
        window.graphEditor.getCurrentCurve().reverse();
        window.postMessage({command: 'updateHandlePos'}, '*');
        break;

    case 'a':
        config.alignHandle = !config.alignHandle;
        break;

    case 'ArrowLeft':
        window.top.postMessage({
            to: 'panel-editor',
            command: 'goBackIdx'
        }, '*');
        break

    case 'ArrowRight':
        if (!isIdxLast()) {
            window.top.postMessage({
                to: 'panel-editor',
                command: 'goForwardIdx'
            }, '*');
        }
        break;
    }
});

$(window).on('resize', () => {
    window.graphEditor.resize(document.documentElement.clientWidth, document.documentElement.clientHeight);
});

// フィットボタン
$('#fit').on('click', () => window.graphEditor.fit());