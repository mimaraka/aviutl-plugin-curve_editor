import Control from './control_base';
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
            this.segmentHandlesArray[i] = createSegmentControl(segments[i], g, scaleX, scaleY);
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
                editor.graph.normal.deleteCurve(curve.id, this.segmentHandlesArray[i].curve.id);
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

    updateControl(transition: d3.Transition<any, unknown, any, unknown> | null = null) {
        super.updateControl(transition);
        this.segmentHandlesArray.forEach((handle) => {
            handle.updateControl(transition);
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

export default NormalControl;