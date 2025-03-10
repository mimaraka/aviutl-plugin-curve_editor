import React from 'react';
import * as d3 from 'd3';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faCode } from '@fortawesome/free-solid-svg-icons';
import { editor } from './interface';
import './style/curve_thumbnail.scss';


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


interface CurveThumbnailProps {
    curveId: number;
    width: number;
}

const CurveThumbnail: React.FC<CurveThumbnailProps> = React.memo((props: CurveThumbnailProps) => {
    const curveType = editor.getCurveName(props.curveId);
    if (curveType === 'script') {
        return <ScriptThumbnail width={props.width}/>
    } else {
        return <GraphThumbnail curveId={props.curveId} width={props.width}/>
    }
});

export default CurveThumbnail;