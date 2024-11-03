import * as d3 from 'd3';
import Curve from './curve_base';


interface PrevHandleFunc {
    onAnchorEndDragStart: (event: MouseEvent, bound?: boolean) => void;
    onAnchorEndDrag: (event: MouseEvent, bound?: boolean) => void;
    onAnchorEndDragEnd: (event: MouseEvent, bound?: boolean) => void;
    update: () => void;
};

interface NextHandleFunc {
    onAnchorStartDragStart: (event: MouseEvent, bound?: boolean) => void;
    onAnchorStartDrag: (event: MouseEvent, bound?: boolean) => void;
    onAnchorStartDragEnd: (event: MouseEvent, bound?: boolean) => void;
    update: () => void;
};

// 制御点
class Control {
    handleRadius: number;
    anchorRadius: number;
    prevHandleFunc: PrevHandleFunc | null;
    nextHandleFunc: NextHandleFunc | null;
    scaleX: d3.ScaleLinear<number, number>;
    scaleY: d3.ScaleLinear<number, number>;
    curve: Curve;
    _bufferAnchorStartX;
    _bufferAnchorStartY;
    _bufferAnchorEndX;
    _bufferAnchorEndY;
    anchorStart;
    anchorEnd;

    // コンストラクタ
    constructor(
        curve: Curve,
        g: d3.Selection<SVGGElement, unknown, HTMLElement, unknown>,
        scaleX: d3.ScaleLinear<number, number>,
        scaleY: d3.ScaleLinear<number, number>
    ) {
        this.handleRadius = 4.8;
        this.anchorRadius = 3.6;
        this.prevHandleFunc = null;
        this.nextHandleFunc = null;
        this.scaleX = scaleX;
        this.scaleY = scaleY;
        this.curve = curve;
        this._bufferAnchorStartX = curve.getAnchorStartX();
        this._bufferAnchorStartY = curve.getAnchorStartY();
        this._bufferAnchorEndX = curve.getAnchorEndX();
        this._bufferAnchorEndY = curve.getAnchorEndY();
        this.anchorStart = g.append('rect')
            .attr('x', scaleX(this._bufferAnchorStartX) - this.anchorRadius)
            .attr('y', scaleY(this._bufferAnchorStartY) - this.anchorRadius)
            .attr('width', this.anchorRadius * 2)
            .attr('height', this.anchorRadius * 2)
            .attr('class', 'anchor')
            .on('mousedown', event => {
                // TODO: 値指定モードに対応
                if (event.button === 2) {
                    event.stopPropagation();
                    window.chrome.webview.postMessage({
                        command: 'ContextMenuSegment',
                        curveId: curve.parentId,
                        segmentId: curve.id,
                    });
                }
            });
        this.anchorEnd = g.append('rect')
            .attr('x', scaleX(this._bufferAnchorEndX) - this.anchorRadius)
            .attr('y', scaleY(this._bufferAnchorEndY) - this.anchorRadius)
            .attr('width', this.anchorRadius * 2)
            .attr('height', this.anchorRadius * 2)
            .attr('class', 'anchor');

        this.anchorStart.call(
            d3.drag<SVGRectElement, unknown>()
                .on('start', (event) => this.onAnchorStartDragStart(event))
                .on('drag', (event) => this.onAnchorStartDrag(event))
                .on('end', (event) => this.onAnchorStartDragEnd(event))
        );
        this.anchorEnd.call(
            d3.drag<SVGRectElement, unknown>()
                .on('start', (event) => this.onAnchorEndDragStart(event))
                .on('drag', (event) => this.onAnchorEndDrag(event))
                .on('end', (event) => this.onAnchorEndDragEnd(event))
        );
    }

    setPrevHandleFunc(func: PrevHandleFunc | null) {
        this.prevHandleFunc = func;
    }

    setNextHandleFunc(func: NextHandleFunc | null) {
        this.nextHandleFunc = func;
    }

    rescaleX(scaleX: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.scaleX = scaleX;
        const startX = this.scaleX(this._bufferAnchorStartX);
        const endX = this.scaleX(this._bufferAnchorEndX);
        (transition ? this.anchorStart.transition(transition) : this.anchorStart)
            .attr('x', startX - this.anchorRadius);
        (transition ? this.anchorEnd.transition(transition) : this.anchorEnd)
            .attr('x', endX - this.anchorRadius);
    }

    rescaleY(scaleY: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.scaleY = scaleY;
        const startY = this.scaleY(this._bufferAnchorStartY);
        const endY = this.scaleY(this._bufferAnchorEndY);
        (transition ? this.anchorStart.transition(transition) : this.anchorStart)
            .attr('y', startY - this.anchorRadius);
        (transition ? this.anchorEnd.transition(transition) : this.anchorEnd)
            .attr('y', endY - this.anchorRadius);
    }

    update() {
        this.updateControl();
        this.updateCurvePath();
    }

    updateControl(
        transition: d3.Transition<any, unknown, any, unknown> | null = null,
        anchorStartX: number| null = null,
        anchorStartY: number| null = null,
        anchorEndX: number| null = null,
        anchorEndY: number| null = null
    ) {
        this._bufferAnchorStartX = anchorStartX || this.curve.getAnchorStartX();
        this._bufferAnchorStartY = anchorStartY || this.curve.getAnchorStartY();
        this._bufferAnchorEndX = anchorEndX || this.curve.getAnchorEndX();
        this._bufferAnchorEndY = anchorEndY || this.curve.getAnchorEndY();
        if (transition) {
            this.anchorStart.transition(transition)
                .attr('x', this.scaleX(this._bufferAnchorStartX) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorStartY) - this.anchorRadius);
            this.anchorEnd.transition(transition)
                .attr('x', this.scaleX(this._bufferAnchorEndX) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorEndY) - this.anchorRadius);
        } else {
            this.anchorStart
                .attr('x', this.scaleX(this._bufferAnchorStartX) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorStartY) - this.anchorRadius);
            this.anchorEnd
                .attr('x', this.scaleX(this._bufferAnchorEndX) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorEndY) - this.anchorRadius);
        }
    }

    updateCurvePath() {
        window.postMessage({command: 'UpdateCurvePath'});
    }

    // 開始アンカーのドラッグ開始
    onAnchorStartDragStart(event: DragEvent, bound = false) {
        if (!bound) {
            this.prevHandleFunc?.onAnchorEndDragStart(event, true);
        }
    }

    // 開始アンカーのドラッグ
    onAnchorStartDrag(event: DragEvent, bound = false) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        this.curve.setAnchorStart(x, y);
        const newX = this.curve.getAnchorStartX();
        const newY = this.curve.getAnchorStartY();
        this.anchorStart
            .attr('x', this.scaleX(newX) - this.anchorRadius)
            .attr('y', this.scaleY(newY) - this.anchorRadius);
        
        if (!bound) {
            this.prevHandleFunc?.onAnchorEndDrag(event, true);
        }
    }

    // 開始アンカーのドラッグ終了
    onAnchorStartDragEnd(event: DragEvent, bound = false) {
        this._bufferAnchorStartX = this.curve.getAnchorStartX();
        this._bufferAnchorStartY = this.curve.getAnchorStartY();
        if (!bound) {
            this.prevHandleFunc?.onAnchorEndDragEnd(event, true);
        } else {
            this.onDragEnd();
        }
    }

    // 終了アンカーのドラッグ開始
    onAnchorEndDragStart(event: DragEvent, bound = false) {
        if (!bound) {
            this.nextHandleFunc?.onAnchorStartDragStart(event, true);
        }
    }

    // 終了アンカーのドラッグ
    onAnchorEndDrag(event: DragEvent, bound = false) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        this.curve.setAnchorEnd(x, y);
        const newX = this.curve.getAnchorEndX();
        const newY = this.curve.getAnchorEndY();
        this.anchorEnd
            .attr('x', this.scaleX(newX) - this.anchorRadius)
            .attr('y', this.scaleY(newY) - this.anchorRadius);
        if (!bound) {
            this.nextHandleFunc?.onAnchorStartDrag(event, true);
        }
    }

    // 終了アンカーのドラッグ終了
    onAnchorEndDragEnd(event: DragEvent, bound = false) {
        this._bufferAnchorEndX = this.curve.getAnchorEndX();
        this._bufferAnchorEndY = this.curve.getAnchorEndY();
        if (!bound) {
            this.nextHandleFunc?.onAnchorStartDragEnd(event, true);
        } else {
            this.onDragEnd();
        }
    }

    onDragEnd() {
        window.chrome.webview.postMessage({
            command: 'OnHandleDragEnd',
            curveId: (this.curve.parentId != 0) ? this.curve.parentId : this.curve.id,
        });
    }

    // ハンドル・アンカーを削除
    remove() {
        this.anchorStart.remove();
        this.anchorEnd.remove();
    }
}

export default Control;