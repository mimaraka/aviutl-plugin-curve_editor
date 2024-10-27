import * as d3 from 'd3';
import { config, graphEditor } from './host_object';
import { Curve, BezierCurve, ElasticCurve, BounceCurve, LinearCurve, NormalCurve } from './curve';


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

// ハンドル (アンカー)
export class Handles {
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
                        command: 'contextmenu-curve-segment',
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
        this.updateHandles();
        this.updateCurvePath();
    }

    updateHandles(
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
        window.postMessage({command: 'updateCurvePath'});
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
            command: 'drag-end',
            curveId: (this.curve.parentId != 0) ? this.curve.parentId : this.curve.id,
        });
    }

    // ハンドル・アンカーを削除
    remove() {
        this.anchorStart.remove();
        this.anchorEnd.remove();
    }
}

// ハンドル (ベジェ)
export class BezierHandles extends Handles {
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
            .attr('class', 'handle');
        this.handleRight = g.append('circle')
            .attr('cx', scaleX(curve.getHandleRightX()))
            .attr('cy', scaleY(curve.getHandleRightY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
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

    updateHandles(
        transition: d3.Transition<any, unknown, any, unknown> | null = null,
        anchorStartX: number| null = null,
        anchorStartY: number| null = null,
        anchorEndX: number| null = null,
        anchorEndY: number| null = null
    ) {
        super.updateHandles(transition, anchorStartX, anchorStartY, anchorEndX, anchorEndY);
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
        this.updateHandles(null, anchorStartX, anchorStartY, anchorEndX, anchorEndY);
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
        const prevCurve = this.curve.getPrevCurve();
        if ((prevCurve instanceof BezierCurve) && config.alignHandle) {
            this.#alignHandleFlag = true;
            const prevOffsetX = prevCurve.getHandleRightX() - prevCurve.getAnchorEndX();
            const prevOffsetY = prevCurve.getHandleRightY() - prevCurve.getAnchorEndY();
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
            const prevCurve = this.curve.getPrevCurve();
            const scaledAngle = Math.atan2(
                -this.scaleY(newY) + this.scaleY(this._bufferAnchorStartY),
                this.scaleX(newX) - this.scaleX(this._bufferAnchorStartX)
            );
            const scaledX = this.scaleX(this._bufferAnchorStartX) + this.#lengthBuffer * Math.cos(scaledAngle + Math.PI);
            const scaledY = this.scaleY(this._bufferAnchorStartY) - this.#lengthBuffer * Math.sin(scaledAngle + Math.PI);
            (prevCurve as BezierCurve).setHandleRight(this.scaleX.invert(scaledX), this.scaleY.invert(scaledY), true);
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
        const nextCurve = this.curve.getNextCurve();
        if ((nextCurve instanceof BezierCurve) && config.alignHandle) {
            this.#alignHandleFlag = true;
            const nextOffsetX = nextCurve.getHandleLeftX() - nextCurve.getAnchorStartX();
            const nextOffsetY = nextCurve.getHandleLeftY() - nextCurve.getAnchorStartY();
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
            const nextCurve = this.curve.getNextCurve();
            const scaledAngle = Math.atan2(
                -this.scaleY(newY) + this.scaleY(this._bufferAnchorEndY),
                this.scaleX(newX) - this.scaleX(this._bufferAnchorEndX)
            );
            const scaledX = this.scaleX(this._bufferAnchorEndX) + this.#lengthBuffer * Math.cos(scaledAngle + Math.PI);
            const scaledY = this.scaleY(this._bufferAnchorEndY) - this.#lengthBuffer * Math.sin(scaledAngle + Math.PI);
            (nextCurve as BezierCurve).setHandleLeft(this.scaleX.invert(scaledX), this.scaleY.invert(scaledY), true);
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

// ハンドル (振動)
export class ElasticHandles extends Handles {
    handleAmpLeft;
    handleAmpRight;
    handleFreqDecayRoot;
    handleFreqDecay;
    handleLineAmp;
    handleLineFreqDecay;

    constructor(
        curve: ElasticCurve,
        g: d3.Selection<SVGGElement, unknown, HTMLElement, unknown>,
        scaleX: d3.ScaleLinear<number, number>,
        scaleY: d3.ScaleLinear<number, number>
    ) {
        super(curve, g, scaleX, scaleY);
        this.handleAmpLeft = g.append('circle')
            .attr('cx', scaleX(curve.getHandleAmpLeftX()))
            .attr('cy', scaleY(curve.getHandleAmpLeftY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleAmpRight = g.append('circle')
            .attr('cx', scaleX(curve.getHandleAmpRightX()))
            .attr('cy', scaleY(curve.getHandleAmpRightY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleFreqDecayRoot = g.append('rect')
            .attr('x', scaleX(curve.getHandleFreqDecayX()) - this.anchorRadius)
            .attr('y', scaleY(curve.getHandleFreqDecayRootY()) - this.anchorRadius)
            .attr('width', this.anchorRadius * 2)
            .attr('height', this.anchorRadius * 2)
            .attr('class', 'anchor');
        this.handleFreqDecay = g.append('circle')
            .attr('cx', scaleX(curve.getHandleFreqDecayX()))
            .attr('cy', scaleY(curve.getHandleFreqDecayY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleLineAmp = g.append('line')
            .attr('x1', scaleX(curve.getHandleAmpLeftX()))
            .attr('y1', scaleY(curve.getHandleAmpLeftY()))
            .attr('x2', scaleX(curve.getHandleAmpRightX()))
            .attr('y2', scaleY(curve.getHandleAmpRightY()))
            .attr('class', 'handle-line');
        this.handleLineFreqDecay = g.append('line')
            .attr('x1', scaleX(curve.getHandleFreqDecayX()))
            .attr('y1', scaleY(curve.getHandleFreqDecayRootY()))
            .attr('x2', scaleX(curve.getHandleFreqDecayX()))
            .attr('y2', scaleY(curve.getHandleFreqDecayY()))
            .attr('class', 'handle-line');
        this.handleAmpLeft.call(
            d3.drag<SVGCircleElement, unknown>()
                .on('drag', this.onHandleAmpLeftDrag.bind(this))
                .on('end', this.onDragEnd.bind(this))
        );
        this.handleAmpRight.call(
            d3.drag<SVGCircleElement, unknown>()
                .on('drag', this.onHandleAmpRightDrag.bind(this))
                .on('end', this.onDragEnd.bind(this))
        );
        this.handleFreqDecay.call(
            d3.drag<SVGCircleElement, unknown>()
                .on('drag', this.onHandleFreqDecayDrag.bind(this))
                .on('end', this.onDragEnd.bind(this))
        );
    }

    rescaleX(scaleX: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleX(scaleX, transition);
        const ampLeftX = this.scaleX((this.curve as ElasticCurve).getHandleAmpLeftX());
        const ampRightX = this.scaleX((this.curve as ElasticCurve).getHandleAmpRightX());
        const freqDecayX = this.scaleX((this.curve as ElasticCurve).getHandleFreqDecayX());
        if (transition) {
            this.handleAmpLeft.transition(transition)
                .attr('cx', ampLeftX);
            this.handleAmpRight.transition(transition)
                .attr('cx', ampRightX);
            this.handleFreqDecay.transition(transition)
                .attr('cx', freqDecayX);
            this.handleFreqDecayRoot.transition(transition)
                .attr('x', freqDecayX - this.anchorRadius);
            this.handleLineAmp.transition(transition)
                .attr('x1', ampLeftX)
                .attr('x2', ampRightX);
            this.handleLineFreqDecay.transition(transition)
                .attr('x1', freqDecayX)
                .attr('x2', freqDecayX);
        } else {
            this.handleAmpLeft
                .attr('cx', ampLeftX);
            this.handleAmpRight
                .attr('cx', ampRightX);
            this.handleFreqDecay
                .attr('cx', freqDecayX);
            this.handleFreqDecayRoot
                .attr('x', freqDecayX - this.anchorRadius);
            this.handleLineAmp
                .attr('x1', ampLeftX)
                .attr('x2', ampRightX);
            this.handleLineFreqDecay
                .attr('x1', freqDecayX)
                .attr('x2', freqDecayX);
        }
    }

    rescaleY(scaleY: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleY(scaleY, transition);
        const ampY = this.scaleY((this.curve as ElasticCurve).getHandleAmpLeftY());
        const freqDecayY = this.scaleY((this.curve as ElasticCurve).getHandleFreqDecayY());
        const freqDecayRootY = this.scaleY((this.curve as ElasticCurve).getHandleFreqDecayRootY());
        if (transition) {
            this.handleAmpLeft.transition(transition)
                .attr('cy', ampY);
            this.handleAmpRight.transition(transition)
                .attr('cy', ampY);
            this.handleFreqDecay.transition(transition)
                .attr('cy', freqDecayY);
            this.handleFreqDecayRoot.transition(transition)
                .attr('y', freqDecayRootY - this.anchorRadius);
            this.handleLineAmp.transition(transition)
                .attr('y1', ampY)
                .attr('y2', ampY);
            this.handleLineFreqDecay.transition(transition)
                .attr('y1', freqDecayRootY)
                .attr('y2', freqDecayY);
        } else {
            this.handleAmpLeft
                .attr('cy', ampY);
            this.handleAmpRight
                .attr('cy', ampY);
            this.handleFreqDecay
                .attr('cy', freqDecayY);
            this.handleFreqDecayRoot
                .attr('y', freqDecayRootY - this.anchorRadius);
            this.handleLineAmp
                .attr('y1', ampY)
                .attr('y2', ampY);
            this.handleLineFreqDecay
                .attr('y1', freqDecayRootY)
                .attr('y2', freqDecayY);
        }
    }

    updateHandles(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.updateHandles(transition);
        const ampLeftX = this.scaleX((this.curve as ElasticCurve).getHandleAmpLeftX());
        const ampRightX = this.scaleX((this.curve as ElasticCurve).getHandleAmpRightX());
        const ampY = this.scaleY((this.curve as ElasticCurve).getHandleAmpLeftY());
        const freqDecayX = this.scaleX((this.curve as ElasticCurve).getHandleFreqDecayX());
        const freqDecayY = this.scaleY((this.curve as ElasticCurve).getHandleFreqDecayY());
        const freqDecayRootY = this.scaleY((this.curve as ElasticCurve).getHandleFreqDecayRootY());
        if (transition) {
            this.handleAmpLeft.transition(transition)
                .attr('cx', ampLeftX)
                .attr('cy', ampY);
            this.handleAmpRight.transition(transition)
                .attr('cx', ampRightX)
                .attr('cy', ampY);
            this.handleFreqDecay.transition(transition)
                .attr('cx', freqDecayX)
                .attr('cy', freqDecayY);
            this.handleFreqDecayRoot.transition(transition)
                .attr('x', freqDecayX - this.anchorRadius)
                .attr('y', freqDecayRootY - this.anchorRadius);
            this.handleLineAmp.transition(transition)
                .attr('x1', ampLeftX)
                .attr('y1', ampY)
                .attr('x2', ampRightX)
                .attr('y2', ampY);
            this.handleLineFreqDecay.transition(transition)
                .attr('x1', freqDecayX)
                .attr('y1', freqDecayRootY)
                .attr('x2', freqDecayX)
                .attr('y2', freqDecayY);
        } else {
            this.handleAmpLeft
                .attr('cx', ampLeftX)
                .attr('cy', ampY);
            this.handleAmpRight
                .attr('cx', ampRightX)
                .attr('cy', ampY);
            this.handleFreqDecay
                .attr('cx', freqDecayX)
                .attr('cy', freqDecayY);
            this.handleFreqDecayRoot
                .attr('x', freqDecayX - this.anchorRadius)
                .attr('y', freqDecayRootY - this.anchorRadius);
            this.handleLineAmp
                .attr('x1', ampLeftX)
                .attr('y1', ampY)
                .attr('x2', ampRightX)
                .attr('y2', ampY);
            this.handleLineFreqDecay
                .attr('x1', freqDecayX)
                .attr('y1', freqDecayRootY)
                .attr('x2', freqDecayX)
                .attr('y2', freqDecayY);
        }
    }

    onAnchorStartDrag(event: DragEvent, bound = false) {
        super.onAnchorStartDrag(event, bound);
        this.update();
    }

    onAnchorEndDrag(event: DragEvent, bound = false) {
        super.onAnchorEndDrag(event, bound);
        this.update();
    }

    onHandleAmpLeftDrag(event: DragEvent) {
        const y = this.scaleY.invert(event.y);
        (this.curve as ElasticCurve).setHandleAmpLeft(y);
        const newX = (this.curve as ElasticCurve).getHandleAmpLeftX();
        const newY = (this.curve as ElasticCurve).getHandleAmpLeftY();
        this.handleAmpLeft
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.update();
    }

    onHandleAmpRightDrag(event: DragEvent) {
        const y = this.scaleY.invert(event.y);
        (this.curve as ElasticCurve).setHandleAmpRight(y);
        const newX = (this.curve as ElasticCurve).getHandleAmpRightX();
        const newY = (this.curve as ElasticCurve).getHandleAmpRightY();
        this.handleAmpRight
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.update();
    }

    onHandleFreqDecayDrag(event: DragEvent) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        (this.curve as ElasticCurve).setHandleFreqDecay(x, y);
        const newX = (this.curve as ElasticCurve).getHandleFreqDecayX();
        const newY = (this.curve as ElasticCurve).getHandleFreqDecayY();
        this.handleFreqDecay
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.update();
    }

    // ハンドル・アンカーを削除
    remove() {
        super.remove();
        this.handleAmpLeft.remove();
        this.handleAmpRight.remove();
        this.handleFreqDecay.remove();
        this.handleFreqDecayRoot.remove();
        this.handleLineAmp.remove();
        this.handleLineFreqDecay.remove();
    }
}

// ハンドル (バウンス)
export class BounceHandles extends Handles {
    handle;

    // コンストラクタ
    constructor(
        curve: BounceCurve,
        g: d3.Selection<SVGGElement, unknown, HTMLElement, unknown>,
        scaleX: d3.ScaleLinear<number, number>,
        scaleY: d3.ScaleLinear<number, number>
    ) {
        super(curve, g, scaleX, scaleY);
        this.handle = g.append('circle')
            .attr('cx', scaleX(curve.getHandleX()))
            .attr('cy', scaleY(curve.getHandleY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handle.call(
            d3.drag<SVGCircleElement, unknown>()
                .on('drag', this.onHandleDrag.bind(this))
                .on('end', this.onDragEnd.bind(this))
        );
    }

    rescaleX(scaleX: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleX(scaleX, transition);
        const x = this.scaleX((this.curve as BounceCurve).getHandleX());
        (transition ? this.handle.transition(transition) : this.handle)
            .attr('cx', x);
    }

    rescaleY(scaleY: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleY(scaleY, transition);
        const y = this.scaleY((this.curve as BounceCurve).getHandleY());
        (transition ? this.handle.transition(transition) : this.handle)
            .attr('cy', y);
    }

    updateHandles(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.updateHandles(transition);
        if (transition) {
            this.handle.transition(transition)
                .attr('cx', this.scaleX((this.curve as BounceCurve).getHandleX()))
                .attr('cy', this.scaleY((this.curve as BounceCurve).getHandleY()));
        } else {
            this.handle
                .attr('cx', this.scaleX((this.curve as BounceCurve).getHandleX()))
                .attr('cy', this.scaleY((this.curve as BounceCurve).getHandleY()));
        }
    }

    onAnchorStartDrag(event: DragEvent, bound = false) {
        super.onAnchorStartDrag(event, bound);
        this.update();
    }

    onAnchorEndDrag(event: DragEvent, bound = false) {
        super.onAnchorEndDrag(event, bound);
        this.update();
    }

    onHandleDrag(event: DragEvent) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        (this.curve as BounceCurve).setHandle(x, y);
        const newX = (this.curve as BounceCurve).getHandleX();
        const newY = (this.curve as BounceCurve).getHandleY();
        this.handle
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.updateCurvePath();
    }

    // ハンドル・アンカーを削除
    remove() {
        super.remove();
        this.handle.remove();
    }
}

class LinearHandles extends Handles {
    onAnchorStartDrag(event: DragEvent, bound = false) {
        super.onAnchorStartDrag(event, bound);
        this.update();
    }

    onAnchorEndDrag(event: DragEvent, bound = false) {
        super.onAnchorEndDrag(event, bound);
        this.update();
    }
}

// ハンドル (標準)
export class NormalHandles extends Handles {
    segmentHandlesArray;
    // コンストラクタ
    constructor(
        curve: NormalCurve,
        g: d3.Selection<SVGGElement, unknown, HTMLElement, unknown>,
        scaleX: d3.ScaleLinear<number, number>,
        scaleY: d3.ScaleLinear<number, number>
    ) {
        super(curve, g, scaleX, scaleY);
        const segments = curve.getSegments();
        this.segmentHandlesArray = Array(segments.length).fill(null);
        for (let i = 0; i < segments.length; i++) {
            this.segmentHandlesArray[i] = createHandles(segments[i], g, scaleX, scaleY);
        }
        for (let i = 0; i < segments.length; i++) {
            const prevHandleFunc = (i > 0) ? {
                onAnchorEndDragStart: this.segmentHandlesArray[i - 1].onAnchorEndDragStart.bind(this.segmentHandlesArray[i - 1]),
                onAnchorEndDrag: this.segmentHandlesArray[i - 1].onAnchorEndDrag.bind(this.segmentHandlesArray[i - 1]),
                onAnchorEndDragEnd: this.segmentHandlesArray[i - 1].onAnchorEndDragEnd.bind(this.segmentHandlesArray[i - 1]),
                update: this.segmentHandlesArray[i - 1].update.bind(this.segmentHandlesArray[i - 1])
            } : null;
            const nextHandleFunc = (i < segments.length - 1) ? {
                onAnchorStartDragStart: this.segmentHandlesArray[i + 1].onAnchorStartDragStart.bind(this.segmentHandlesArray[i + 1]),
                onAnchorStartDrag: this.segmentHandlesArray[i + 1].onAnchorStartDrag.bind(this.segmentHandlesArray[i + 1]),
                onAnchorStartDragEnd: this.segmentHandlesArray[i + 1].onAnchorStartDragEnd.bind(this.segmentHandlesArray[i + 1]),
                update: this.segmentHandlesArray[i + 1].update.bind(this.segmentHandlesArray[i + 1])
            } : null;
            this.segmentHandlesArray[i].setPrevHandleFunc(prevHandleFunc);
            this.segmentHandlesArray[i].setNextHandleFunc(nextHandleFunc);
        }
        for (let i = 0; i < segments.length; i++) {
            this.segmentHandlesArray[i].anchorStart.on('dblclick', (event: MouseEvent) => {
                event.stopPropagation();
                graphEditor.normal.deleteCurve(curve.id, this.segmentHandlesArray[i].curve.id);
                this.segmentHandlesArray[i].remove();
                this.segmentHandlesArray.splice(i, 1);
                for (let handle of this.segmentHandlesArray) {
                    handle.update();
                }
            });
        }
    }

    rescaleX(scaleX: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleX(scaleX, transition);
        this.segmentHandlesArray.forEach((handle) => {
            handle.rescaleX(scaleX, transition);
        });
    }

    rescaleY(scaleY: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleY(scaleY, transition);
        this.segmentHandlesArray.forEach((handle) => {
            handle.rescaleY(scaleY, transition);
        });
    }

    updateHandles(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.updateHandles(transition);
        this.segmentHandlesArray.forEach((handle) => {
            handle.updateHandles(transition);
        });
    }

    // ハンドル・アンカーを削除
    remove() {
        super.remove();
        this.segmentHandlesArray.forEach((handle) => {
            handle.remove();
        });
    }
}

// ハンドルを作成
export function createHandles(
    curve: BezierCurve | ElasticCurve | BounceCurve | LinearCurve | NormalCurve,
    g: d3.Selection<SVGGElement, unknown, HTMLElement, unknown>,
    scaleX: d3.ScaleLinear<number, number>,
    scaleY: d3.ScaleLinear<number, number>
) {
    if (curve instanceof BezierCurve) {
        return new BezierHandles(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof ElasticCurve) {
        return new ElasticHandles(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof BounceCurve) {
        return new BounceHandles(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof LinearCurve) {
        return new LinearHandles(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof NormalCurve) {
        return new NormalHandles(curve, g, scaleX, scaleY);
    }
    else {
        return null;
    }
}