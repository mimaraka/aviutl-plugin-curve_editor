import NumericCurve from './curve_numeric';
import { editor } from './host_object';


class ElasticCurve extends NumericCurve {
    getHandleAmpLeftX() {
        return editor.graph.elastic.getHandleAmpLeftX(this.id);
    }

    getHandleAmpLeftY() {
        return editor.graph.elastic.getHandleAmpLeftY(this.id);
    }

    moveHandleAmpLeft(y: number) {
        editor.graph.elastic.moveHandleAmpLeft(this.id, y);
    }

    getHandleAmpRightX() {
        return editor.graph.elastic.getHandleAmpRightX(this.id);
    }

    getHandleAmpRightY() {
        return editor.graph.elastic.getHandleAmpRightY(this.id);
    }

    moveHandleAmpRight(y: number) {
        editor.graph.elastic.moveHandleAmpRight(this.id, y);
    }

    getHandleFreqDecayX() {
        return editor.graph.elastic.getHandleFreqDecayX(this.id);
    }

    getHandleFreqDecayY() {
        return editor.graph.elastic.getHandleFreqDecayY(this.id);
    }

    getHandleFreqDecayRootY() {
        return editor.graph.elastic.getHandleFreqDecayRootY(this.id);
    }

    moveHandleFreqDecay(x: number, y: number) {
        editor.graph.elastic.moveHandleFreqDecay(this.id, x, y);
    }
}

export default ElasticCurve;