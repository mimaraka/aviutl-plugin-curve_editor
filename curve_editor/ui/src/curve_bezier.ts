import NumericCurve from './curve_numeric';
import { editor } from './host_object';


class BezierCurve extends NumericCurve {
    getHandleLeftX() {
        return editor.graph.bezier.getHandleLeftX(this.id);
    }

    getHandleLeftY() {
        return editor.graph.bezier.getHandleLeftY(this.id);
    }

    beginMoveHandleLeft(scaleX: number, scaleY: number) {
        editor.graph.bezier.beginMoveHandleLeft(this.id, scaleX, scaleY);
    }

    moveHandleLeft(x: number, y: number, keepAngle = false) {
        editor.graph.bezier.moveHandleLeft(this.id, x, y, keepAngle);
    }

    getHandleRightX() {
        return editor.graph.bezier.getHandleRightX(this.id);
    }

    getHandleRightY() {
        return editor.graph.bezier.getHandleRightY(this.id);
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