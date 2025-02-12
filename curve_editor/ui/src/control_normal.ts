import Control, { PrevHandleFunc, NextHandleFunc } from './control_base';
import BezierControl from './control_bezier';
import ElasticControl from './control_elastic';
import BounceControl from './control_bounce';
import LinearControl from './control_linear';
import BezierCurve from './curve_bezier';
import ElasticCurve from './curve_elastic';
import BounceCurve from './curve_bounce';
import LinearCurve from './curve_linear';
import NormalCurve from './curve_normal';
import { editor } from './host_object';


// ハンドルを作成
const createSegmentControl = (
    curve: BezierCurve | ElasticCurve | BounceCurve | LinearCurve,
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
    else {
        return null;
    }
}


// ハンドル (標準)
class NormalControl extends Control {
    segmentControlArray : (BezierControl | ElasticControl | BounceControl | LinearControl | null)[];
    // コンストラクタ
    constructor(
        curve: NormalCurve,
        g: d3.Selection<SVGGElement, unknown, HTMLElement, unknown>,
        scaleX: d3.ScaleLinear<number, number>,
        scaleY: d3.ScaleLinear<number, number>
    ) {
        super(curve, g, scaleX, scaleY);
        const segments = curve.getSegments();
        this.segmentControlArray = Array(segments.length).fill(null);
        for (let i = 0; i < segments.length; i++) {
            this.segmentControlArray[i] = createSegmentControl(segments[i], g, scaleX, scaleY);
        }
        this.segmentControlArray.forEach((control) => { control?.updateHandleOrder(); });
        for (let i = 0; i < segments.length; i++) {
            const prevHandleFunc: PrevHandleFunc | null = (i > 0) ? {
                updateAnchorEnd: this.segmentControlArray[i - 1]?.updateAnchorEnd.bind(this.segmentControlArray[i - 1]) ?? null,
                updateHandle: this.segmentControlArray[i - 1]?.updateHandle.bind(this.segmentControlArray[i - 1]) ?? null,
                updateHandleRight: (this.segmentControlArray[i - 1] instanceof BezierControl) ? (this.segmentControlArray[i - 1] as BezierControl)?.updateHandleRight.bind(this.segmentControlArray[i - 1]) : null
            } : null;
            const nextHandleFunc: NextHandleFunc | null = (i < segments.length - 1) ? {
                updateAnchorStart: this.segmentControlArray[i + 1]?.updateAnchorStart.bind(this.segmentControlArray[i + 1]) ?? null,
                updateHandle: this.segmentControlArray[i + 1]?.updateHandle.bind(this.segmentControlArray[i + 1]) ?? null,
                updateHandleLeft: (this.segmentControlArray[i + 1] instanceof BezierControl) ? (this.segmentControlArray[i + 1] as BezierControl)?.updateHandleLeft.bind(this.segmentControlArray[i + 1]) : null
            } : null;
            this.segmentControlArray[i]?.setPrevHandleFunc(prevHandleFunc);
            this.segmentControlArray[i]?.setNextHandleFunc(nextHandleFunc);
        }
        for (let i = 0; i < segments.length; i++) {
            this.segmentControlArray[i]?.anchorStart.on('dblclick', (event: MouseEvent) => {
                event.stopPropagation();
                editor.graph.normal.deleteCurve(curve.id, this.segmentControlArray[i]?.curve.id ?? 0);
                window.postMessage({
                    command: 'UpdateControl'
                }, '*');
            });
        }
    }

    rescaleX(scaleX: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleX(scaleX, transition);
        this.segmentControlArray.forEach((control) => {
            control?.rescaleX(scaleX, transition);
        });
    }

    rescaleY(scaleY: d3.ScaleLinear<number, number>, transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.rescaleY(scaleY, transition);
        this.segmentControlArray.forEach((control) => {
            control?.rescaleY(scaleY, transition);
        });
    }

    // ハンドル・アンカーを削除
    remove() {
        super.remove();
        this.segmentControlArray.forEach((control) => {
            control?.remove();
        });
    }
}

export default NormalControl;