import NumericCurve from './curve_numeric';
import { editor } from './host_object';


class BezierCurve extends NumericCurve {
    getHandleLeft() {
        let result = editor.graph.bezier.getHandleLeft(this.id);
        if (result.length !== 2) {
            result = [0, 0];
        }
        return {
            x: result[0],
            y: result[1]
        };
    }

    beginMoveHandleLeft(scaleX: number, scaleY: number) {
        editor.graph.bezier.beginMoveHandleLeft(this.id, scaleX, scaleY);
    }

    moveHandleLeft(x: number, y: number, keepAngle = false) {
        editor.graph.bezier.moveHandleLeft(this.id, x, y, keepAngle);
    }

    getHandleRight() {
        let result = editor.graph.bezier.getHandleRight(this.id);
        if (result.length !== 2) {
            result = [0, 0];
        }
        return {
            x: result[0],
            y: result[1]
        };
    }

    beginMoveHandleRight(scaleX: number, scaleY: number) {
        editor.graph.bezier.beginMoveHandleRight(this.id, scaleX, scaleY);
    }

    moveHandleRight(x: number, y: number, keepAngle = false) {
        editor.graph.bezier.moveHandleRight(this.id, x, y, keepAngle);
    }

    getPrevBezier() {
        if (editor.getCurveName(this.prevId) !== 'bezier') {
            return null;
        }
        return new BezierCurve(this.prevId);
    }

    getNextBezier() {
        if (editor.getCurveName(this.prevId) !== 'bezier') {
            return null;
        }
        return new BezierCurve(this.prevId);
    }
}

export default BezierCurve;