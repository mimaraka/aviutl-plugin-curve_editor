import NumericCurve from './curve_numeric';
import { editor } from './host_object';


class BezierCurve extends NumericCurve {
    getHandleLeftX() {
        return editor.graph.bezier.getHandleLeftX(this.id);
    }

    getHandleLeftY() {
        return editor.graph.bezier.getHandleLeftY(this.id);
    }

    setHandleLeft(x: number, y: number, keepAngle = false) {
        editor.graph.bezier.setHandleLeft(this.id, x, y, keepAngle);
    }

    getHandleRightX() {
        return editor.graph.bezier.getHandleRightX(this.id);
    }

    getHandleRightY() {
        return editor.graph.bezier.getHandleRightY(this.id);
    }

    setHandleRight(x: number, y: number, keepAngle = false) {
        editor.graph.bezier.setHandleRight(this.id, x, y, keepAngle);
    }

    getPrevBezier() {
        const id = editor.graph.graph.getPrevCurveId(this.id);
        if (editor.getCurveName(id) !== 'bezier') {
            return null;
        }
        return new BezierCurve(id);
    }

    getNextBezier() {
        const id = editor.graph.graph.getNextCurveId(this.id);
        if (editor.getCurveName(id) !== 'bezier') {
            return null;
        }
        return new BezierCurve(id);
    }
}

export default BezierCurve;