import NumericCurve from './curve_numeric';
import { editor } from './interface';


class BounceCurve extends NumericCurve {
    getHandle() {
        let result = editor.graph.bounce.getHandle(this.id);
        if (result.length !== 2) {
            result = [0, 0];
        }
        return {
            x: result[0],
            y: result[1]
        };
    }

    moveHandle(x: number, y: number) {
        editor.graph.bounce.moveHandle(this.id, x, y);
    }
}

export default BounceCurve;