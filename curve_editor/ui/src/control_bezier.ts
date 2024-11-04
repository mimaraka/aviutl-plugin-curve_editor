import * as d3 from 'd3';
import Control from './control_base';
import BezierCurve from './curve_bezier';


// 制御点 (ベジェ)
class BezierControl extends Control {
    // バッファ
    #bufferHandleLeftX: number;
    #bufferHandleLeftY: number;
    #bufferHandleRightX: number;
    #bufferHandleRightY: number;
    // D3オブジェクト
    handleLeft;
    handleRight;
    handleLineLeft;
    handleLineRight;

    // コンストラクタ
    constructor(
        curve: BezierCurve,
        g: d3.Selection<SVGGElement, unknown, HTMLElement, unknown>,
        scaleX: d3.ScaleLinear<number, number>,
        scaleY: d3.ScaleLinear<number, number>
    ) {
        super(curve, g, scaleX, scaleY);
        this.#bufferHandleLeftX = curve.getHandleLeftX();
        this.#bufferHandleLeftY = curve.getHandleLeftY();
        this.#bufferHandleRightX = curve.getHandleRightX();
        this.#bufferHandleRightY = curve.getHandleRightY();

        // ハンドルの制御点(左)の作成
        this.handleLeft = g.append('circle')
            .attr('cx', scaleX(this.#bufferHandleLeftX))
            .attr('cy', scaleY(this.#bufferHandleLeftY))
            .attr('r', this.handleRadius)
            .attr('class', 'handle')
            .on('mousedown', event => {
                if (event.button === 2) {
                    event.stopPropagation();
                    window.chrome.webview.postMessage({
                        command: 'ContextMenuBezierHandle',
                        curveId: curve.id,
                        parentCurveId: curve.parentId,
                        scaleX: this.scaleX(1) - this.scaleX(0),
                        scaleY: this.scaleY(1) - this.scaleY(0),
                        handleType: 'left',
                    });
                }
            });
        // ハンドルの制御点(右)の作成
        this.handleRight = g.append('circle')
            .attr('cx', scaleX(this.#bufferHandleRightX))
            .attr('cy', scaleY(this.#bufferHandleRightY))
            .attr('r', this.handleRadius)
            .attr('class', 'handle')
            .on('mousedown', event => {
                if (event.button === 2) {
                    event.stopPropagation();
                    window.chrome.webview.postMessage({
                        command: 'ContextMenuBezierHandle',
                        curveId: curve.id,
                        parentCurveId: curve.parentId,
                        scaleX: this.scaleX(1) - this.scaleX(0),
                        scaleY: this.scaleY(1) - this.scaleY(0),
                        handleType: 'right',
                    });
                }
            });

        // ハンドルの棒(左)の作成
        this.handleLineLeft = g.append('line')
            .attr('x1', scaleX(this._bufferAnchorStartX))
            .attr('y1', scaleY(this._bufferAnchorStartY))
            .attr('x2', scaleX(this.#bufferHandleLeftX))
            .attr('y2', scaleY(this.#bufferHandleLeftY))
            .attr('class', 'handle-line');
        // ハンドルの棒(右)の作成
        this.handleLineRight = g.append('line')
            .attr('x1', scaleX(this._bufferAnchorEndX))
            .attr('y1', scaleY(this._bufferAnchorEndY))
            .attr('x2', scaleX(this.#bufferHandleRightX))
            .attr('y2', scaleY(this.#bufferHandleRightY))
            .attr('class', 'handle-line');

        // ドラッグイベントの設定
        this.handleLeft.call(
            d3.drag<SVGCircleElement, unknown>()
                .on('start', this.onHandleLeftDragStart.bind(this))
                .on('drag', this.onHandleLeftDrag.bind(this))
                .on('end', this.onDragEnd.bind(this))
        );
        this.handleRight.call(
            d3.drag<SVGCircleElement, unknown>()
                .on('start', this.onHandleRightDragStart.bind(this))
                .on('drag', this.onHandleRightDrag.bind(this))
                .on('end', this.onDragEnd.bind(this))
        );
    }

    rescaleX(scaleX: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleX(scaleX, transition);
        const leftX = this.scaleX(this.#bufferHandleLeftX);
        const rightX = this.scaleX(this.#bufferHandleRightX);
        const startX = this.scaleX(this._bufferAnchorStartX);
        const endX = this.scaleX(this._bufferAnchorEndX);
        if (transition) {
            this.handleLeft.transition(transition)
                .attr('cx', leftX);
            this.handleRight.transition(transition)
                .attr('cx', rightX);
            this.handleLineLeft.transition(transition)
                .attr('x1', startX)
                .attr('x2', leftX);
            this.handleLineRight.transition(transition)
                .attr('x1', endX)
                .attr('x2', rightX);
        } else {
            this.handleLeft
                .attr('cx', leftX);
            this.handleRight
                .attr('cx', rightX);
            this.handleLineLeft
                .attr('x1', startX)
                .attr('x2', leftX);
            this.handleLineRight
                .attr('x1', endX)
                .attr('x2', rightX);
        }
    }

    rescaleY(scaleY: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleY(scaleY, transition);
        const leftY = this.scaleY(this.#bufferHandleLeftY);
        const rightY = this.scaleY(this.#bufferHandleRightY);
        const startY = this.scaleY(this._bufferAnchorStartY);
        const endY = this.scaleY(this._bufferAnchorEndY);
        if (transition) {
            this.handleLeft.transition(transition)
                .attr('cy', leftY);
            this.handleRight.transition(transition)
                .attr('cy', rightY);
            this.handleLineLeft.transition(transition)
                .attr('y1', startY)
                .attr('y2', leftY);
            this.handleLineRight.transition(transition)
                .attr('y1', endY)
                .attr('y2', rightY);
        } else {
            this.handleLeft
                .attr('cy', leftY);
            this.handleRight
                .attr('cy', rightY);
            this.handleLineLeft
                .attr('y1', startY)
                .attr('y2', leftY);
            this.handleLineRight
                .attr('y1', endY)
                .attr('y2', rightY);
        }
    }

    updateAnchorStart(
        transition: d3.Transition<any, unknown, any, unknown> | null = null
    ) {
        super.updateAnchorStart(transition);
        if (transition) {
            this.handleLineLeft.transition(transition)
                .attr('x1', this.scaleX(this._bufferAnchorStartX))
                .attr('y1', this.scaleY(this._bufferAnchorStartY));
        } else {
            this.handleLineLeft
                .attr('x1', this.scaleX(this._bufferAnchorStartX))
                .attr('y1', this.scaleY(this._bufferAnchorStartY));
        }
    }

    updateAnchorEnd(
        transition: d3.Transition<any, unknown, any, unknown> | null = null
    ) {
        super.updateAnchorEnd(transition);
        if (transition) {
            this.handleLineRight.transition(transition)
                .attr('x1', this.scaleX(this._bufferAnchorEndX))
                .attr('y1', this.scaleY(this._bufferAnchorEndY));
        } else {
            this.handleLineRight
                .attr('x1', this.scaleX(this._bufferAnchorEndX))
                .attr('y1', this.scaleY(this._bufferAnchorEndY));
        }
    }

    updateControl(transition?: d3.Transition<any, unknown, any, unknown> | null): void {
        super.updateControl(transition);
        this.updateHandleLeft(transition);
        this.updateHandleRight(transition);
    }

    updateHandleLeft(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.#bufferHandleLeftX = (this.curve as BezierCurve).getHandleLeftX();
        this.#bufferHandleLeftY = (this.curve as BezierCurve).getHandleLeftY();
        const scaledX = this.scaleX(this.#bufferHandleLeftX);
        const scaledY = this.scaleY(this.#bufferHandleLeftY);
        if (transition) {
            this.handleLeft.transition(transition)
                .attr('cx', scaledX)
                .attr('cy', scaledY);
            this.handleLineLeft.transition(transition)
                .attr('x2', scaledX)
                .attr('y2', scaledY);
        } else {
            this.handleLeft
                .attr('cx', scaledX)
                .attr('cy', scaledY);
            this.handleLineLeft
                .attr('x2', scaledX)
                .attr('y2', scaledY);
        }
    }

    updateHandleRight(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.#bufferHandleRightX = (this.curve as BezierCurve).getHandleRightX();
        this.#bufferHandleRightY = (this.curve as BezierCurve).getHandleRightY();
        const scaledX = this.scaleX(this.#bufferHandleRightX);
        const scaledY = this.scaleY(this.#bufferHandleRightY);
        if (transition) {
            this.handleRight.transition(transition)
                .attr('cx', scaledX)
                .attr('cy', scaledY);
            this.handleLineRight.transition(transition)
                .attr('x2', scaledX)
                .attr('y2', scaledY);
        } else {
            this.handleRight
                .attr('cx', scaledX)
                .attr('cy', scaledY);
            this.handleLineRight
                .attr('x2', scaledX)
                .attr('y2', scaledY);
        }
    }

    updateHandle(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.updateHandleLeft(transition);
        this.updateHandleRight(transition);
    }

    // 左ハンドルのドラッグ開始
    onHandleLeftDragStart(event: DragEvent) {
        (this.curve as BezierCurve).beginMoveHandleLeft(
            this.scaleX(1) - this.scaleX(0),
            this.scaleY(1) - this.scaleY(0)
        );
    }

    // 左ハンドルのドラッグ
    onHandleLeftDrag(event: DragEvent) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        (this.curve as BezierCurve).moveHandleLeft(x, y);
        this.updateHandleLeft();
        this.prevHandleFunc?.updateHandleRight?.(null);
        this.updateCurvePath();
    }

    // 右ハンドルのドラッグ開始
    onHandleRightDragStart(event: DragEvent) {
        (this.curve as BezierCurve).beginMoveHandleRight(
            this.scaleX(1) - this.scaleX(0),
            this.scaleY(1) - this.scaleY(0)
        );
    }

    // 右ハンドルのドラッグ
    onHandleRightDrag(event: DragEvent) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        (this.curve as BezierCurve).moveHandleRight(x, y);
        this.updateHandleRight();
        this.nextHandleFunc?.updateHandleLeft?.(null);
        this.updateCurvePath();
    }

    // ハンドル・アンカーを削除
    remove() {
        super.remove();
        this.handleLeft.remove();
        this.handleRight.remove();
        this.handleLineLeft.remove();
        this.handleLineRight.remove();
    }
}

export default BezierControl;