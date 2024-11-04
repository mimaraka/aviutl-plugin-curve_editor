import * as d3 from 'd3';
import Curve from './curve_base';


export interface PrevHandleFunc {
    updateAnchorEnd: ((transition: d3.Transition<any, unknown, any, unknown> | null) => void) | null;
    updateHandle: ((transition: d3.Transition<any, unknown, any, unknown> | null) => void) | null;
    updateHandleRight: ((transition: d3.Transition<any, unknown, any, unknown> | null) => void) | null;
};

export interface NextHandleFunc {
    updateAnchorStart: ((transition: d3.Transition<any, unknown, any, unknown> | null) => void) | null;
    updateHandle: ((transition: d3.Transition<any, unknown, any, unknown> | null) => void) | null;
    updateHandleLeft: ((transition: d3.Transition<any, unknown, any, unknown> | null) => void) | null;
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
    _bufferAnchorStartX: number;
    _bufferAnchorStartY: number;
    _bufferAnchorEndX: number;
    _bufferAnchorEndY: number;

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
        this._bufferAnchorStartX = this.curve.getAnchorStartX();
        this._bufferAnchorStartY = this.curve.getAnchorStartY();
        this._bufferAnchorEndX = this.curve.getAnchorEndX();
        this._bufferAnchorEndY = this.curve.getAnchorEndY();

        // D3オブジェクトの作成
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
        
        // ドラッグイベントの設定
        this.anchorStart.call(
            d3.drag<SVGRectElement, unknown>()
                .on('start', (event) => this.onAnchorStartDragStart(event))
                .on('drag', (event) => this.onAnchorStartDrag(event))
                .on('end', (event) => this.onDragEnd(event))
        );
        this.anchorEnd.call(
            d3.drag<SVGRectElement, unknown>()
                .on('start', (event) => this.onAnchorEndDragStart(event))
                .on('drag', (event) => this.onAnchorEndDrag(event))
                .on('end', (event) => this.onDragEnd(event))
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
        const scaledStartX = this.scaleX(this._bufferAnchorStartX);
        const scaledEndX = this.scaleX(this._bufferAnchorEndX);
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
        const scaledStartY = this.scaleY(this._bufferAnchorStartY);
        const scaledEndY = this.scaleY(this._bufferAnchorEndY);
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
        this._bufferAnchorStartX = this.curve.getAnchorStartX();
        this._bufferAnchorStartY = this.curve.getAnchorStartY();
        if (transition) {
            this.anchorStart.transition(transition)
                .attr('x', this.scaleX(this._bufferAnchorStartX) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorStartY) - this.anchorRadius);
        } else {
            this.anchorStart
                .attr('x', this.scaleX(this._bufferAnchorStartX) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorStartY) - this.anchorRadius);
        }
    }

    updateAnchorEnd(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this._bufferAnchorEndX = this.curve.getAnchorEndX();
        this._bufferAnchorEndY = this.curve.getAnchorEndY();
        if (transition) {
            this.anchorEnd.transition(transition)
                .attr('x', this.scaleX(this._bufferAnchorEndX) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorEndY) - this.anchorRadius);
        } else {
            this.anchorEnd
                .attr('x', this.scaleX(this._bufferAnchorEndX) - this.anchorRadius)
                .attr('y', this.scaleY(this._bufferAnchorEndY) - this.anchorRadius);
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