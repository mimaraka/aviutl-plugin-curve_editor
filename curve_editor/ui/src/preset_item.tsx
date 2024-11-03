import React from 'react';
import * as d3 from 'd3';
import { editor, preset } from './host_object';
import './scss/preset_item.scss';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faCode } from '@fortawesome/free-solid-svg-icons';


interface GraphThumbnailProps {
    curveId: number;
    width: number;
}

const GraphThumbnail: React.FC<GraphThumbnailProps> = (props: GraphThumbnailProps) => {
    interface Point {
        x: number;
        y: number;
    }

    const canvasId = `preset-canvas-${props.curveId}`;
    const padding = 8;
    const svg = React.useRef<d3.Selection<SVGSVGElement, unknown, HTMLElement, any> | null>(null);
    const scaleX = React.useRef<d3.ScaleLinear<number, number> | null>(null);
    const scaleY = React.useRef<d3.ScaleLinear<number, number> | null>(null);
    const line = React.useRef<d3.Line<Point> | null>(null);
    const curvePath = React.useRef<d3.Selection<SVGPathElement, Point[], HTMLElement, any> | null>(null);

    React.useEffect(() => {
        const resolution = 80;
        let curveData: Point[] = [];

        const yArray = editor.graph.getCurveValueArray(props.curveId, 0, 0, 1, 1, resolution);
        for (let i = 0; i < resolution; i++) {
            const x = i / (resolution - 1);
            curveData.push({
                x: x,
                y: yArray[i]
            });
        }

        scaleX.current = d3.scaleLinear()
            .domain([0, 1])
            .range([padding, props.width - padding]);

        scaleY.current = d3.scaleLinear()
            .domain([0, 1])
            .range([props.width - padding, padding]);

        svg.current = d3.select<SVGSVGElement, unknown>(`#${canvasId}`)
            .attr('width', props.width)
            .attr('height', props.width);

        line.current = d3.line<Point>()
            .x(d => scaleX.current?.(d.x) ?? 0)
            .y(d => scaleY.current?.(d.y) ?? 0)
            .curve(d3.curveCatmullRom.alpha(0.5));

        curvePath.current = svg.current.append('path')
            .datum(curveData)
            .attr('fill', 'none')
            .attr('class', 'curve-preset')
            .attr('d', line.current);
    }, []);

    React.useEffect(() => {
        svg.current?.attr('width', props.width)
            .attr('height', props.width);

        scaleX.current?.range([padding, props.width - padding]);
        scaleY.current?.range([props.width - padding, padding]);

        line.current?.x(d => scaleX.current?.(d.x) ?? 0)
            .y(d => scaleY.current?.(d.y) ?? 0);
        curvePath.current?.attr('d', line.current);
    }, [props.width]);

    return (
        <svg className='thumbnail' id={`preset-canvas-${props.curveId}`}/>
    );
}

interface ScriptThumbnailProps {
    width: number;
}

const ScriptThumbnail: React.FC<ScriptThumbnailProps> = (props: ScriptThumbnailProps) => {
    return (
        <div className='thumbnail'>
            <FontAwesomeIcon icon={faCode} style={{ fontSize: props.width * 0.35 }}/>
        </div>
    )
}


interface PresetItemProps {
    width: number;
    curveId: number;
    collectionId: number;
    name: string;
}

const PresetItem: React.FC<PresetItemProps> = (props: PresetItemProps) => {
    const [isDragging, setIsDragging] = React.useState(false);

    const onMouseDown = (event: React.MouseEvent) => {
        if (event.button === 0) {
            setIsDragging(true);
        }
        else if (event.button === 2) {
            window.chrome.webview.postMessage({
                command: 'ContextMenuPresetItem',
                curveId: props.curveId
            });
        }
    }

    const onMouseLeave = (event: React.MouseEvent) => {
        if (isDragging) {
            const type = editor.getCurveName(props.curveId);
            if (type === 'bezier' || type === 'elastic' || type === 'bounce') {
                window.chrome.webview.postMessage({
                command: 'OnDndStart',
                curveId: props.curveId
            });
            }
        }
    }

    const onMouseUp = (event: React.MouseEvent) => {
        setIsDragging(false);
    }

    const onDoubleClick = (event: React.MouseEvent) => {
        window.chrome.webview.postMessage({
            command: 'ApplyPreset',
            curveId: props.curveId
        });
    }

    const onMessageFromHost = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'OnDndEnd':
                setIsDragging(false);
                break;
        }
    }

    React.useEffect(() => {
        window.chrome.webview.addEventListener('message', onMessageFromHost);

        return () => {
            window.chrome.webview.removeEventListener('message', onMessageFromHost);
        }
    }, []);

    return (
        <div className='preset-item' style={{ width: props.width, height: props.width + 16 /*<- TODO: 直す*/ }}>
            <div
                className='preset-item-thumbnail'
                title={`${props.name}\nタイプ: ${editor.getCurveDispName(props.curveId)}\nコレクション: ${preset.getCollectionName(props.collectionId)}`}
                onMouseDown={onMouseDown}
                onMouseLeave={onMouseLeave}
                onMouseUp={onMouseUp}
                onDoubleClick={onDoubleClick}
            >
                {editor.getCurveName(props.curveId) === 'script' ?
                    <ScriptThumbnail width={props.width - 4}/> :
                    <GraphThumbnail width={props.width - 4} curveId={props.curveId}/>
                }
            </div>
            <div className='preset-item-text'>
                {props.name}
            </div>
        </div>
    );
}

export default PresetItem;