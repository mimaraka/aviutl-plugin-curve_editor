import Curve from './curve_base';
import { editor } from './host_object';
import BezierCurve from './curve_bezier';
import ElasticCurve from './curve_elastic';
import BounceCurve from './curve_bounce';
import LinearCurve from './curve_linear';


const createSegment = (id: number, parentId: number, prevId: number = 0, nextId: number = 0): BezierCurve | ElasticCurve | BounceCurve | LinearCurve | null => {
    switch (editor.getCurveName(id)) {
        case 'bezier':
            return new BezierCurve(id, parentId, prevId, nextId);

        case 'elastic':
            return new ElasticCurve(id, parentId, prevId, nextId);

        case 'bounce':
            return new BounceCurve(id, parentId, prevId, nextId);

        case 'linear':
            return new LinearCurve(id, parentId, prevId, nextId);
        
        default:
            return null;
    }
}

class NormalCurve extends Curve {
    getSegments() {
        const idArray = editor.graph.normal.getIdArray(this.id);
        let segments = Array(idArray.length).fill(null);
        for (let i = 0; i < idArray.length; i++) {
            let prevId = 0;
            let nextId = 0;
            if (i !== 0) {
                prevId = idArray[i - 1];
            }
            if (i !== idArray.length - 1) {
                nextId = idArray[i + 1];
            }
            segments[i] = createSegment(idArray[i], this.id, prevId, nextId);
        }
        return segments;
    }

    addCurve(x: number, y: number, scaleX: number) {
        editor.graph.normal.addCurve(this.id, x, y, scaleX);
    }
}

export default NormalCurve;