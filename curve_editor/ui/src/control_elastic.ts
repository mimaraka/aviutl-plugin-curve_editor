import * as d3 from 'd3';
import Control from './control_base';
import ElasticCurve from './curve_elastic';


// ハンドル (振動)
class ElasticControl extends Control {
    // バッファ
    #bufferHandleAmpLeftX: number;
    #bufferHandleAmpRightX: number;
    #bufferHandleAmpY: number;
    #bufferHandleFreqDecayX: number;
    #bufferHandleFreqDecayY: number;
    #bufferHandleFreqDecayRootY: number;
    // D3オブジェクト
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
        this.#bufferHandleAmpLeftX = curve.getHandleAmpLeftX();
        this.#bufferHandleAmpRightX = curve.getHandleAmpRightX();
        this.#bufferHandleAmpY = curve.getHandleAmpLeftY();
        this.#bufferHandleFreqDecayX = curve.getHandleFreqDecayX();
        this.#bufferHandleFreqDecayY = curve.getHandleFreqDecayY();
        this.#bufferHandleFreqDecayRootY = curve.getHandleFreqDecayRootY();

        this.handleAmpLeft = g.append('circle')
            .attr('cx', scaleX(this.#bufferHandleAmpLeftX))
            .attr('cy', scaleY(this.#bufferHandleAmpY))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleAmpRight = g.append('circle')
            .attr('cx', scaleX(this.#bufferHandleAmpRightX))
            .attr('cy', scaleY(this.#bufferHandleAmpY))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleFreqDecayRoot = g.append('rect')
            .attr('x', scaleX(this.#bufferHandleFreqDecayX) - this.anchorRadius)
            .attr('y', scaleY(this.#bufferHandleFreqDecayRootY) - this.anchorRadius)
            .attr('width', this.anchorRadius * 2)
            .attr('height', this.anchorRadius * 2)
            .attr('class', 'anchor');
        this.handleFreqDecay = g.append('circle')
            .attr('cx', scaleX(this.#bufferHandleFreqDecayX))
            .attr('cy', scaleY(this.#bufferHandleFreqDecayY))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleLineAmp = g.append('line')
            .attr('x1', scaleX(this.#bufferHandleAmpLeftX))
            .attr('y1', scaleY(this.#bufferHandleAmpY))
            .attr('x2', scaleX(this.#bufferHandleAmpRightX))
            .attr('y2', scaleY(this.#bufferHandleAmpY))
            .attr('class', 'handle-line');
        this.handleLineFreqDecay = g.append('line')
            .attr('x1', scaleX(this.#bufferHandleFreqDecayX))
            .attr('y1', scaleY(this.#bufferHandleFreqDecayRootY))
            .attr('x2', scaleX(this.#bufferHandleFreqDecayX))
            .attr('y2', scaleY(this.#bufferHandleFreqDecayY))
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
        const ampLeftX = this.scaleX(this.#bufferHandleAmpLeftX);
        const ampRightX = this.scaleX(this.#bufferHandleAmpRightX);
        const freqDecayX = this.scaleX(this.#bufferHandleFreqDecayX);
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
        const ampY = this.scaleY(this.#bufferHandleAmpY);
        const freqDecayY = this.scaleY(this.#bufferHandleFreqDecayY);
        const freqDecayRootY = this.scaleY(this.#bufferHandleFreqDecayRootY);
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

    updateHandleAmpLeft(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.#bufferHandleAmpLeftX = (this.curve as ElasticCurve).getHandleAmpLeftX();
        this.#bufferHandleAmpY = (this.curve as ElasticCurve).getHandleAmpLeftY();
        const x = this.scaleX(this.#bufferHandleAmpLeftX);
        const y = this.scaleY(this.#bufferHandleAmpY);
        if (transition) {
            this.handleAmpLeft.transition(transition)
                .attr('cx', x)
                .attr('cy', y);
            this.handleLineAmp.transition(transition)
                .attr('x1', x)
                .attr('y1', y);
        } else {
            this.handleAmpLeft
                .attr('cx', x)
                .attr('cy', y);
            this.handleLineAmp
                .attr('x1', x)
                .attr('y1', y);
        }
    }

    updateHandleAmpRight(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.#bufferHandleAmpRightX = (this.curve as ElasticCurve).getHandleAmpRightX();
        const x = this.scaleX(this.#bufferHandleAmpRightX);
        const y = this.scaleY(this.#bufferHandleAmpY);
        if (transition) {
            this.handleAmpRight.transition(transition)
                .attr('cx', x)
                .attr('cy', y);
            this.handleLineAmp.transition(transition)
                .attr('x2', x)
                .attr('y2', y);
        } else {
            this.handleAmpRight
                .attr('cx', x)
                .attr('cy', y);
            this.handleLineAmp
                .attr('x2', x)
                .attr('y2', y);
        }
    }

    updateHandleFreqDecay(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        this.#bufferHandleFreqDecayX = (this.curve as ElasticCurve).getHandleFreqDecayX();
        this.#bufferHandleFreqDecayY = (this.curve as ElasticCurve).getHandleFreqDecayY();
        this.#bufferHandleFreqDecayRootY = (this.curve as ElasticCurve).getHandleFreqDecayRootY();
        const x = this.scaleX(this.#bufferHandleFreqDecayX);
        const y = this.scaleY(this.#bufferHandleFreqDecayY);
        const rootY = this.scaleY(this.#bufferHandleFreqDecayRootY);
        if (transition) {
            this.handleFreqDecay.transition(transition)
                .attr('cx', x)
                .attr('cy', y);
            this.handleFreqDecayRoot.transition(transition)
                .attr('x', x - this.anchorRadius)
                .attr('y', rootY - this.anchorRadius);
            this.handleLineFreqDecay.transition(transition)
                .attr('x1', x)
                .attr('y1', rootY)
                .attr('x2', x)
                .attr('y2', y);
        } else {
            this.handleFreqDecay
                .attr('cx', x)
                .attr('cy', y);
            this.handleFreqDecayRoot
                .attr('x', x - this.anchorRadius)
                .attr('y', rootY - this.anchorRadius);
            this.handleLineFreqDecay
                .attr('x1', x)
                .attr('y1', rootY)
                .attr('x2', x)
                .attr('y2', y);
        }
    }

    updateHandle(transition?: d3.Transition<any, unknown, any, unknown> | null): void {
        this.updateHandleAmpLeft(transition);
        this.updateHandleAmpRight(transition);
        this.updateHandleFreqDecay(transition);
    }

    updateControl(transition?: d3.Transition<any, unknown, any, unknown> | null): void {
        super.updateControl(transition);
        this.updateHandle(transition);
    }

    onHandleAmpLeftDrag(event: DragEvent) {
        const y = this.scaleY.invert(event.y);
        (this.curve as ElasticCurve).moveHandleAmpLeft(y);
        this.updateHandleAmpLeft();
        this.updateHandleAmpRight();
        this.updateCurvePath();
    }

    onHandleAmpRightDrag(event: DragEvent) {
        const y = this.scaleY.invert(event.y);
        (this.curve as ElasticCurve).moveHandleAmpRight(y);
        this.updateHandleAmpLeft();
        this.updateHandleAmpRight();
        this.updateCurvePath();
    }

    onHandleFreqDecayDrag(event: DragEvent) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        (this.curve as ElasticCurve).moveHandleFreqDecay(x, y);
        this.updateHandleFreqDecay();
        this.updateCurvePath();
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

export default ElasticControl;