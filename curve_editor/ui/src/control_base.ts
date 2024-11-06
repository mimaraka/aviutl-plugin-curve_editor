import * as d3 from 'd3';
import Curve from './curve_base';


export interface PrevHandleFunc {
    updateAnchorEnd: ((transition: d3.Transition<any, unknown, any, unknown> | null) => void) | null;
    updateHandle: ((transition: d3.Transition<any, unknown, any, unknown> | null) => void) | null;
    updateHandleRight: ((transition: d3.Transition<any, unknown, any, unknown> | null, bound: boolean) => void) | null;
};

export interface NextHandleFunc {
    updateAnchorStart: ((transition: d3.Transition<any, unknown, any, unknown> | null) => void) | null;
    updateHandle: ((transition: d3.Transition<any, unknown, any, unknown> | null) => void) | null;
    updateHandleLeft: ((transition: d3.Transition<any, unknown, any, unknown> | null, bound: boolean) => void) | null;
};

// 制御点
class Control {
    handleRadius: number;
    anchorRadius: number;
    prevHandleFunc: PrevHandleFunc | null;
    nextHandleFunc: NextHandleFunc | null;
    // D3スケール
    scaleX: d3.ScaleLinear<number, number>;
    scaleY: d3.ScaleLinear<number, number>;
    // カーブ
    curve: Curve;
    // D3オブジェクト
    anchorStart;
    anchorEnd;
    // バッファ
    _bufferAnchorStart;
    _bufferAnchorEnd;

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
        this._bufferAnchorStart = this.curve.getAnchorStart();
        this._bufferAnchorEnd = this.curve.getAnchorEnd();

        // D3オブジェクトの作成
        this.anchorStart = g.append('rect')
            .attr('x', scaleX(this._bufferAnchorStart.x) - this.anchorRadius)
            .attr('y', scaleY(this._bufferAnchorStart.y) - this.anchorRadius)
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
            .attr('x', scaleX(this._bufferAnchorEnd.x) - this.anchorRadius)
            .attr('y', scaleY(this._bufferAnchorEnd.y) - this.anchorRadius)
            .attr('width', this.anchorRadius * 2)
            .attr('height', this.anchorRadius * 2)
            .attr('class', 'anchor');
        
        // ドラッグイベントの設定
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
        const scaledStartX = this.scaleX(this._bufferAnchorStart.x);
        const scaledEndX = this.scaleX(this._bufferAnchorEnd.x);
        if (transition) {
            this.anchorStart.transition(transition)
                .attr('x', scaledStartX - this.anchorRadius);
            this.anchorEnd.transition(transition)
                .attr('x', scaledEndX - this.anchorRadius);
        } else {
            this.anchorStart
                .attr('x', scaledStartX - this.anchorRadius);
            this.anchorEnd
                .attr('x', scaledEndX - this.anchorRadius);
        }
    }

    rescaleY(scaleY: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.scaleY = scaleY;
        const scaledStartY = this.scaleY(this._bufferAnchorStart.y);
        const scaledEndY = this.scaleY(this._bufferAnchorEnd.y);
        if (transition) {
            this.anchorStart.transition(transition)
                .attr('y', scaledStartY - this.anchorRadius);
            this.anchorEnd.transition(transition)
                .attr('y', scaledEndY - this.anchorRadius);
        } else {
            this.anchorStart
                .attr('y', scaledStartY - this.anchorRadius);
            this.anchorEnd
                .attr('y', scaledEndY - this.anchorRadius);
        }
    }

    updateAnchorStart(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this._bufferAnchorStart = this.curve.getAnchorStart();
        if (transition) {
            this.anchorStart.transition(transition)
                .attr('x', this.scaleX(this._bufferAnchorStart.x) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorStart.y) - this.anchorRadius);
        } else {
            this.anchorStart
                .attr('x', this.scaleX(this._bufferAnchorStart.x) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorStart.y) - this.anchorRadius);
        }
    }

    updateAnchorEnd(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this._bufferAnchorEnd = this.curve.getAnchorEnd();
        if (transition) {
            this.anchorEnd.transition(transition)
                .attr('x', this.scaleX(this._bufferAnchorEnd.x) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorEnd.y) - this.anchorRadius);
        } else {
            this.anchorEnd
                .attr('x', this.scaleX(this._bufferAnchorEnd.x) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorEnd.y) - this.anchorRadius);
        }
    }

    updateControl(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.updateAnchorStart(transition);
        this.updateAnchorEnd(transition);
    }

    updateHandle(transition: d3.Transition<any, unknown, any, unknown> | null = null) {}

    updateCurvePath() {
        window.postMessage({command: 'UpdateCurvePath'});
    }

    // 開始アンカーのドラッグ開始
    onAnchorStartDragStart(event: DragEvent) {
        this.curve.beginMoveAnchorStart();
    }

    // 開始アンカーのドラッグ
    onAnchorStartDrag(event: DragEvent) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        // 新しい座標をホストに送信
        this.curve.moveAnchorStart(x, y);
        this.updateAnchorStart();
        this.updateHandle();
        this.prevHandleFunc?.updateAnchorEnd?.(null);
        this.prevHandleFunc?.updateHandle?.(null);
        this.updateCurvePath();
    }

    onAnchorStartDragEnd(event: DragEvent) {
        this.curve.endMoveAnchorStart();
        this.onDragEnd(event);
    }

    // 終了アンカーのドラッグ開始
    onAnchorEndDragStart(event: DragEvent) {
        this.curve.beginMoveAnchorEnd();
    }

    // 終了アンカーのドラッグ
    onAnchorEndDrag(event: DragEvent) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        // 新しい座標をホストに送信
        this.curve.moveAnchorEnd(x, y);
        this.updateAnchorEnd();
        this.updateHandle();
        this.nextHandleFunc?.updateAnchorStart?.(null);
        this.nextHandleFunc?.updateHandle?.(null);
        this.updateCurvePath();
    }

    onAnchorEndDragEnd(event: DragEvent) {
        this.curve.endMoveAnchorEnd();
        this.onDragEnd(event);
    }

    onDragEnd(event: DragEvent) {
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