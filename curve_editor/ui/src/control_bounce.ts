import * as d3 from 'd3';
import Control from './control_base';
import BounceCurve from './curve_bounce';


// ハンドル (バウンス)
class BounceControl extends Control {
    // バッファ
    #bufferHandleX: number;
    #bufferHandleY: number;
    // D3オブジェクト
    handle;

    // コンストラクタ
    constructor(
        curve: BounceCurve,
        g: d3.Selection<SVGGElement, unknown, HTMLElement, unknown>,
        scaleX: d3.ScaleLinear<number, number>,
        scaleY: d3.ScaleLinear<number, number>
    ) {
        super(curve, g, scaleX, scaleY);
        this.#bufferHandleX = curve.getHandleX();
        this.#bufferHandleY = curve.getHandleY();
        this.handle = g.append('circle')
            .attr('cx', scaleX(this.#bufferHandleX))
            .attr('cy', scaleY(this.#bufferHandleY))
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
        const x = this.scaleX(this.#bufferHandleX);
        if (transition) {
            this.handle.transition(transition)
                .attr('cx', x);
        } else {
            this.handle
                .attr('cx', x);
        }
    }

    rescaleY(scaleY: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleY(scaleY, transition);
        const y = this.scaleY(this.#bufferHandleY);
        if (transition) {
            this.handle.transition(transition)
                .attr('cy', y);
        } else {
            this.handle
                .attr('cy', y);
        }
    }

    updateHandle(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.#bufferHandleX = (this.curve as BounceCurve).getHandleX();
        this.#bufferHandleY = (this.curve as BounceCurve).getHandleY();
        if (transition) {
            this.handle.transition(transition)
                .attr('cx', this.scaleX(this.#bufferHandleX))
                .attr('cy', this.scaleY(this.#bufferHandleY));
        } else {
            this.handle
                .attr('cx', this.scaleX(this.#bufferHandleX))
                .attr('cy', this.scaleY(this.#bufferHandleY));
        }
    }

    updateControl(transition?: d3.Transition<any, unknown, any, unknown> | null): void {
        super.updateControl(transition);
        this.updateHandle(transition);
    }

    onHandleDrag(event: DragEvent) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        (this.curve as BounceCurve).moveHandle(x, y);
        this.updateHandle();
        this.updateCurvePath();
    }

    // ハンドル・アンカーを削除
    remove() {
        super.remove();
        this.handle.remove();
    }
}

export default BounceControl;