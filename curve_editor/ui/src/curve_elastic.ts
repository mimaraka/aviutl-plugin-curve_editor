import NumericCurve from './curve_numeric';
import { editor } from './interface';


class ElasticCurve extends NumericCurve {
    getHandleAmpLeft() {
        let result = editor.graph.elastic.getHandleAmpLeft(this.id);
        if (result.length !== 2) {
            result = [0, 0];
        }
        return {
            x: result[0],
            y: result[1]
        };
    }

    moveHandleAmpLeft(y: number) {
        editor.graph.elastic.moveHandleAmpLeft(this.id, y);
    }

    getHandleAmpRight() {
        let result = editor.graph.elastic.getHandleAmpRight(this.id);
        if (result.length !== 2) {
            result = [0, 0];
        }
        return {
            x: result[0],
            y: result[1]
        };
    }

    moveHandleAmpRight(y: number) {
        editor.graph.elastic.moveHandleAmpRight(this.id, y);
    }

    getHandleFreqDecay() {
        let result = editor.graph.elastic.getHandleFreqDecay(this.id);
        if (result.length !== 2) {
            result = [0, 0];
        }
        return {
            x: result[0],
            y: result[1]
        };
    }

    getHandleFreqDecayRootY() {
        return editor.graph.elastic.getHandleFreqDecayRootY(this.id);
    }

    moveHandleFreqDecay(x: number, y: number) {
        editor.graph.elastic.moveHandleFreqDecay(this.id, x, y);
    }
}

export default ElasticCurve;