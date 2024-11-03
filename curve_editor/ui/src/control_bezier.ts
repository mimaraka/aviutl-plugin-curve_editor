import * as d3 from 'd3';
import Control from './control_base';
import BezierCurve from './curve_bezier';
import { config } from './host_object';


// 制御点 (ベジェ)
class BezierControl extends Control {
    #bufferHandleLeftOffsetX = 0;
    #bufferHandleLeftOffsetY = 0;
    #bufferHandleRightOffsetX = 0;
    #bufferHandleRightOffsetY = 0;
    #lengthBuffer = 0;
    #alignHandleFlag = false;
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
        this.handleLeft = g.append('circle')
            .attr('cx', scaleX(curve.getHandleLeftX()))
            .attr('cy', scaleY(curve.getHandleLeftY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle')
            .on('mousedown', event => {
                if (event.button === 2) {
                    event.stopPropagation();
                    window.chrome.webview.postMessage({
                        command: 'ContextMenuBezierHandle',
                        curveId: curve.id,
                        handleType: 'left',
                    });
                }
            });
        this.handleRight = g.append('circle')
            .attr('cx', scaleX(curve.getHandleRightX()))
            .attr('cy', scaleY(curve.getHandleRightY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle')
            .on('mousedown', event => {
                if (event.button === 2) {
                    event.stopPropagation();
                    window.chrome.webview.postMessage({
                        command: 'ContextMenuBezierHandle',
                        curveId: curve.id,
                        handleType: 'right',
                    });
                }
            });
        this.handleLineLeft = g.append('line')
            .attr('x1', scaleX(this._bufferAnchorStartX))
            .attr('y1', scaleY(this._bufferAnchorStartY))
            .attr('x2', scaleX(curve.getHandleLeftX()))
            .attr('y2', scaleY(curve.getHandleLeftY()))
            .attr('class', 'handle-line');
        this.handleLineRight = g.append('line')
            .attr('x1', scaleX(this._bufferAnchorEndX))
            .attr('y1', scaleY(this._bufferAnchorEndY))
            .attr('x2', scaleX(curve.getHandleRightX()))
            .attr('y2', scaleY(curve.getHandleRightY()))
            .attr('class', 'handle-line');
        this.handleLeft.call(
            d3.drag<SVGCircleElement, unknown>()
                .on('start', this.onHandleLeftDragStart.bind(this))
                .on('drag', this.onHandleLeftDrag.bind(this))
                .on('end', this.onHandleLeftDragEnd.bind(this))
        );
        this.handleRight.call(
            d3.drag<SVGCircleElement, unknown>()
                .on('start', this.onHandleRightDragStart.bind(this))
                .on('drag', this.onHandleRightDrag.bind(this))
                .on('end', this.onHandleRightDragEnd.bind(this))
        );
    }

    rescaleX(scaleX: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleX(scaleX, transition);
        const leftX = this.scaleX((this.curve as BezierCurve).getHandleLeftX());
        const rightX = this.scaleX((this.curve as BezierCurve).getHandleRightX());
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
        const leftY = this.scaleY((this.curve as BezierCurve).getHandleLeftY());
        const rightY = this.scaleY((this.curve as BezierCurve).getHandleRightY());
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

    updateControl(
        transition: d3.Transition<any, unknown, any, unknown> | null = null,
        anchorStartX: number| null = null,
        anchorStartY: number| null = null,
        anchorEndX: number| null = null,
        anchorEndY: number| null = null
    ) {
        super.updateControl(transition, anchorStartX, anchorStartY, anchorEndX, anchorEndY);
        const scaledAnchorStartX = this.scaleX(anchorStartX || this.curve.getAnchorStartX());
        const scaledAnchorStartY = this.scaleY(anchorStartY || this.curve.getAnchorStartY());
        const scaledAnchorEndX = this.scaleX(anchorEndX || this.curve.getAnchorEndX());
        const scaledAnchorEndY = this.scaleY(anchorEndY || this.curve.getAnchorEndY());
        const scaledHandleLeftX = this.scaleX((this.curve as BezierCurve).getHandleLeftX());
        const scaledHandleLeftY = this.scaleY((this.curve as BezierCurve).getHandleLeftY());
        const scaledHandleRightX = this.scaleX((this.curve as BezierCurve).getHandleRightX());
        const scaledHandleRightY = this.scaleY((this.curve as BezierCurve).getHandleRightY());
        if (transition) {
            this.handleLeft.transition(transition)
                .attr('cx', scaledHandleLeftX)
                .attr('cy', scaledHandleLeftY);
            this.handleRight.transition(transition)
                .attr('cx', scaledHandleRightX)
                .attr('cy', scaledHandleRightY);
            this.handleLineLeft.transition(transition)
                .attr('x1', scaledAnchorStartX)
                .attr('y1', scaledAnchorStartY)
                .attr('x2', scaledHandleLeftX)
                .attr('y2', scaledHandleLeftY);
            this.handleLineRight.transition(transition)
                .attr('x1', scaledAnchorEndX)
                .attr('y1', scaledAnchorEndY)
                .attr('x2', scaledHandleRightX)
                .attr('y2', scaledHandleRightY);
        } else {
            this.handleLeft
                .attr('cx', scaledHandleLeftX)
                .attr('cy', scaledHandleLeftY);
            this.handleRight
                .attr('cx', scaledHandleRightX)
                .attr('cy', scaledHandleRightY);
            this.handleLineLeft
                .attr('x1', scaledAnchorStartX)
                .attr('y1', scaledAnchorStartY)
                .attr('x2', scaledHandleLeftX)
                .attr('y2', scaledHandleLeftY);
            this.handleLineRight
                .attr('x1', scaledAnchorEndX)
                .attr('y1', scaledAnchorEndY)
                .attr('x2', scaledHandleRightX)
                .attr('y2', scaledHandleRightY);
        }
    }

    onAnchorDragStart() {
        this.#bufferHandleLeftOffsetX = (this.curve as BezierCurve).getHandleLeftX() - this.curve.getAnchorStartX();
        this.#bufferHandleLeftOffsetY = (this.curve as BezierCurve).getHandleLeftY() - this.curve.getAnchorStartY();
        this.#bufferHandleRightOffsetX = (this.curve as BezierCurve).getHandleRightX() - this.curve.getAnchorEndX();
        this.#bufferHandleRightOffsetY = (this.curve as BezierCurve).getHandleRightY() - this.curve.getAnchorEndY();
    }

    onAnchorDrag() {
        const anchorStartX = this.curve.getAnchorStartX();
        const anchorStartY = this.curve.getAnchorStartY();
        const anchorEndX = this.curve.getAnchorEndX();
        const anchorEndY = this.curve.getAnchorEndY();
        (this.curve as BezierCurve).setHandleLeft(anchorStartX + this.#bufferHandleLeftOffsetX, anchorStartY + this.#bufferHandleLeftOffsetY, true);
        (this.curve as BezierCurve).setHandleRight(anchorEndX + this.#bufferHandleRightOffsetX, anchorEndY + this.#bufferHandleRightOffsetY, true);
        this.updateControl(null, anchorStartX, anchorStartY, anchorEndX, anchorEndY);
    }

    // 開始アンカーのドラッグ開始
    onAnchorStartDragStart(event: DragEvent, bound = false) {
        super.onAnchorStartDragStart(event, bound);
        this.onAnchorDragStart();
    }

    // 開始アンカーのドラッグ
    onAnchorStartDrag(event: DragEvent, bound = false) {
        super.onAnchorStartDrag(event, bound);
        this.onAnchorDrag();
        if (!bound) {
            this.updateCurvePath();
        }
    }

    // 終了アンカーのドラッグ開始
    onAnchorEndDragStart(event: DragEvent, bound = false) {
        super.onAnchorEndDragStart(event, bound);
        this.onAnchorDragStart();
    }

    // 終了アンカーのドラッグ
    onAnchorEndDrag(event: DragEvent, bound = false) {
        super.onAnchorEndDrag(event, bound);
        this.onAnchorDrag();
        if (!bound) {
            this.updateCurvePath();
        }
    }

    // 左ハンドルのドラッグ開始
    onHandleLeftDragStart(event: DragEvent) {
        const prevBezier = (this.curve as BezierCurve).getPrevBezier();
        if (prevBezier && config.alignHandle) {
            this.#alignHandleFlag = true;
            const prevOffsetX = prevBezier.getHandleRightX() - prevBezier.getAnchorEndX();
            const prevOffsetY = prevBezier.getHandleRightY() - prevBezier.getAnchorEndY();
            const scaledPrevOffsetX = this.scaleX(prevOffsetX) - this.scaleX(0);
            const scaledPrevOffsetY = this.scaleY(prevOffsetY) - this.scaleY(0);
            this.#lengthBuffer = Math.sqrt(scaledPrevOffsetX ** 2 + scaledPrevOffsetY ** 2);
        }
    }

    // 左ハンドルのドラッグ
    onHandleLeftDrag(event: DragEvent) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        (this.curve as BezierCurve).setHandleLeft(x, y);
        const newX = (this.curve as BezierCurve).getHandleLeftX();
        const newY = (this.curve as BezierCurve).getHandleLeftY();
        this.handleLeft
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.handleLineLeft
            .attr('x2', this.scaleX(newX))
            .attr('y2', this.scaleY(newY));
        // 左ハンドルをドラッグしたときに、前のカーブの右ハンドルも移動する
        if (this.#alignHandleFlag) {
            const prevBezier = (this.curve as BezierCurve).getPrevBezier();
            const scaledAngle = Math.atan2(
                -this.scaleY(newY) + this.scaleY(this._bufferAnchorStartY),
                this.scaleX(newX) - this.scaleX(this._bufferAnchorStartX)
            );
            const scaledX = this.scaleX(this._bufferAnchorStartX) + this.#lengthBuffer * Math.cos(scaledAngle + Math.PI);
            const scaledY = this.scaleY(this._bufferAnchorStartY) - this.#lengthBuffer * Math.sin(scaledAngle + Math.PI);
            prevBezier?.setHandleRight(this.scaleX.invert(scaledX), this.scaleY.invert(scaledY), true);
            this.prevHandleFunc?.update();
        } else {
            this.updateCurvePath();
        }
    }

    // 左ハンドルのドラッグ終了
    onHandleLeftDragEnd(event: DragEvent) {
        this.#alignHandleFlag = false;
        this.onDragEnd();
    }

    // 右ハンドルのドラッグ開始
    onHandleRightDragStart(event: DragEvent) {
        const nextBezier = (this.curve as BezierCurve).getNextBezier();
        if (nextBezier && config.alignHandle) {
            this.#alignHandleFlag = true;
            const nextOffsetX = nextBezier.getHandleLeftX() - nextBezier.getAnchorStartX();
            const nextOffsetY = nextBezier.getHandleLeftY() - nextBezier.getAnchorStartY();
            const scaledNextOffsetX = this.scaleX(nextOffsetX) - this.scaleX(0);
            const scaledNextOffsetY = this.scaleY(nextOffsetY) - this.scaleY(0);
            this.#lengthBuffer = Math.sqrt(scaledNextOffsetX ** 2 + scaledNextOffsetY ** 2);
        }
    }

    // 右ハンドルのドラッグ
    onHandleRightDrag(event: DragEvent) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        (this.curve as BezierCurve).setHandleRight(x, y);
        const newX = (this.curve as BezierCurve).getHandleRightX();
        const newY = (this.curve as BezierCurve).getHandleRightY();
        this.handleRight
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.handleLineRight
            .attr('x2', this.scaleX(newX))
            .attr('y2', this.scaleY(newY));
        // 右ハンドルをドラッグしたときに、次のカーブの左ハンドルも移動する
        if (this.#alignHandleFlag) {
            const nextBezier = (this.curve as BezierCurve).getNextBezier();
            const scaledAngle = Math.atan2(
                -this.scaleY(newY) + this.scaleY(this._bufferAnchorEndY),
                this.scaleX(newX) - this.scaleX(this._bufferAnchorEndX)
            );
            const scaledX = this.scaleX(this._bufferAnchorEndX) + this.#lengthBuffer * Math.cos(scaledAngle + Math.PI);
            const scaledY = this.scaleY(this._bufferAnchorEndY) - this.#lengthBuffer * Math.sin(scaledAngle + Math.PI);
            nextBezier?.setHandleLeft(this.scaleX.invert(scaledX), this.scaleY.invert(scaledY), true);
            this.nextHandleFunc?.update();
        } else {
            this.updateCurvePath();
        }
    }

    // 右ハンドルのドラッグ終了
    onHandleRightDragEnd(event: DragEvent) {
        this.#alignHandleFlag = false;
        this.onDragEnd();
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