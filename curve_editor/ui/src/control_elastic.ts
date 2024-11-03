import * as d3 from 'd3';
import Control from './control_base';
import ElasticCurve from './curve_elastic';


// ハンドル (振動)
class ElasticControl extends Control {
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

    updateControl(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.updateControl(transition);
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

export default ElasticControl;