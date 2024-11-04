import NumericCurve from './curve_numeric';
import { editor } from './host_object';


class BounceCurve extends NumericCurve {
    getHandleX() {
        return editor.graph.bounce.getHandleX(this.id);
    }

    getHandleY() {
        return editor.graph.bounce.getHandleY(this.id);
    }

    moveHandle(x: number, y: number) {
        editor.graph.bounce.moveHandle(this.id, x, y);
    }
}

export default BounceCurve;