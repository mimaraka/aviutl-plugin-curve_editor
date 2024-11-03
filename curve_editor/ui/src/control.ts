import * as d3 from 'd3';
import BezierCurve from './curve_bezier';
import ElasticCurve from './curve_elastic';
import BounceCurve from './curve_bounce';
import LinearCurve from './curve_linear';
import NormalCurve from './curve_normal';
import BezierControl from './control_bezier';
import ElasticControl from './control_elastic';
import BounceControl from './control_bounce';
import LinearControl from './control_linear';
import NormalControl from './control_normal';


// ハンドルを作成
const createControl = (
    curve: BezierCurve | ElasticCurve | BounceCurve | LinearCurve | NormalCurve,
    g: d3.Selection<SVGGElement, unknown, HTMLElement, unknown>,
    scaleX: d3.ScaleLinear<number, number>,
    scaleY: d3.ScaleLinear<number, number>
) => {
    if (curve instanceof BezierCurve) {
        return new BezierControl(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof ElasticCurve) {
        return new ElasticControl(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof BounceCurve) {
        return new BounceControl(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof LinearCurve) {
        return new LinearControl(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof NormalCurve) {
        return new NormalControl(curve, g, scaleX, scaleY);
    }
    else {
        return null;
    }
}

export default createControl;