import Curve from './curve_base';
import { editor } from './host_object';
import BezierCurve from './curve_bezier';
import ElasticCurve from './curve_elastic';
import BounceCurve from './curve_bounce';
import LinearCurve from './curve_linear';


const createSegment = (id: number, parentId = 0): BezierCurve | ElasticCurve | BounceCurve | LinearCurve | null => {
    switch (editor.getCurveName(id)) {
        case 'bezier':
            return new BezierCurve(id, parentId);

        case 'elastic':
            return new ElasticCurve(id, parentId);

        case 'bounce':
            return new BounceCurve(id, parentId);

        case 'linear':
            return new LinearCurve(id, parentId);
        
        default:
            return null;
    }
}

class NormalCurve extends Curve {
    getSegments() {
        const idArray = editor.graph.normal.getIdArray(this.id);
        let segments = Array(idArray.length).fill(null);
        for (let i = 0; i < idArray.length; i++) {
            segments[i] = createSegment(idArray[i], this.id);
        }
        return segments;
    }

    addCurve(x: number, y: number, scaleX: number) {
        editor.graph.normal.addCurve(this.id, x, y, scaleX);
    }
}

export default NormalCurve;