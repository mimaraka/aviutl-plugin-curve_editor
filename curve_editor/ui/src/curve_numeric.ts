import Curve from './curve_base';
import { editor } from './host_object';


class NumericCurve extends Curve {
    encode() {
        return editor.graph.numeric.encode(this.id);
    }

    decode(code: number) {
        return editor.graph.numeric.decode(this.id, code);
    }
}

export default NumericCurve;