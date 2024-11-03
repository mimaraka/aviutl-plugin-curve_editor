import * as d3 from 'd3';
import Control from './control_base';
import BounceCurve from './curve_bounce';


// ハンドル (バウンス)
class BounceControl extends Control {
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

    updateControl(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.updateControl(transition);
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

export default BounceControl;