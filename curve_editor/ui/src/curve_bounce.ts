import NumericCurve from './curve_numeric';
import { editor } from './host_object';


class BounceCurve extends NumericCurve {
    getHandleX() {
        return editor.graph.bounce.getHandleX(this.id);
    }

    getHandleY() {
        return editor.graph.bounce.getHandleY(this.id);
    }

    setHandle(x: number, y: number) {
        editor.graph.bounce.setHandle(this.id, x, y);
    }
}

export default BounceCurve;