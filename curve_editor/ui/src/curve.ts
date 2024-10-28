import { graphEditor } from './host_object';


export class Curve {
    #id: number;
    #parentId: number;
    constructor(id: number, parentId = 0) {
        this.#id = id;
        this.#parentId = parentId;
    }

    get id() {
        return this.#id;
    }

    get parentId() {
        return this.#parentId;
    }

    getAnchorStartX() {
        return graphEditor.graph.getAnchorStartX(this.id);
    }

    getAnchorStartY() {
        return graphEditor.graph.getAnchorStartY(this.id);
    }

    setAnchorStart(x: number, y: number) {
        graphEditor.graph.setAnchorStart(this.id, x, y);
    }

    getAnchorEndX() {
        return graphEditor.graph.getAnchorEndX(this.id);
    }

    getAnchorEndY() {
        return graphEditor.graph.getAnchorEndY(this.id);
    }

    setAnchorEnd(x: number, y: number) {
        graphEditor.graph.setAnchorEnd(this.id, x, y);
    }

    getPrevCurve() {
        const id = graphEditor.graph.getPrevCurveId(this.id);
        return createCurve(id);
    }

    getNextCurve() {
        const id = graphEditor.graph.getNextCurveId(this.id);
        return createCurve(id);
    }

    reverse() {
        graphEditor.graph.reverse(this.id);
    }
}

export class NumericCurve extends Curve {
    encode() {
        return graphEditor.numeric.encode(this.id);
    }

    decode(code: number) {
        return graphEditor.numeric.decode(this.id, code);
    }
}

export class BezierCurve extends NumericCurve {
    getHandleLeftX() {
        return graphEditor.bezier.getHandleLeftX(this.id);
    }

    getHandleLeftY() {
        return graphEditor.bezier.getHandleLeftY(this.id);
    }

    setHandleLeft(x: number, y: number, keepAngle = false) {
        graphEditor.bezier.setHandleLeft(this.id, x, y, keepAngle);
    }

    getHandleRightX() {
        return graphEditor.bezier.getHandleRightX(this.id);
    }

    getHandleRightY() {
        return graphEditor.bezier.getHandleRightY(this.id);
    }

    setHandleRight(x: number, y: number, keepAngle = false) {
        graphEditor.bezier.setHandleRight(this.id, x, y, keepAngle);
    }
}

export class ElasticCurve extends NumericCurve {
    getHandleAmpLeftX() {
        return graphEditor.elastic.getHandleAmpLeftX(this.id);
    }

    getHandleAmpLeftY() {
        return graphEditor.elastic.getHandleAmpLeftY(this.id);
    }

    setHandleAmpLeft(y: number) {
        graphEditor.elastic.setHandleAmpLeft(this.id, y);
    }

    getHandleAmpRightX() {
        return graphEditor.elastic.getHandleAmpRightX(this.id);
    }

    getHandleAmpRightY() {
        return graphEditor.elastic.getHandleAmpRightY(this.id);
    }

    setHandleAmpRight(y: number) {
        graphEditor.elastic.setHandleAmpRight(this.id, y);
    }

    getHandleFreqDecayX() {
        return graphEditor.elastic.getHandleFreqDecayX(this.id);
    }

    getHandleFreqDecayY() {
        return graphEditor.elastic.getHandleFreqDecayY(this.id);
    }

    getHandleFreqDecayRootY() {
        return graphEditor.elastic.getHandleFreqDecayRootY(this.id);
    }

    setHandleFreqDecay(x: number, y: number) {
        graphEditor.elastic.setHandleFreqDecay(this.id, x, y);
    }
}

export class BounceCurve extends NumericCurve {
    getHandleX() {
        return graphEditor.bounce.getHandleX(this.id);
    }

    getHandleY() {
        return graphEditor.bounce.getHandleY(this.id);
    }

    setHandle(x: number, y: number) {
        graphEditor.bounce.setHandle(this.id, x, y);
    }
}

export class LinearCurve extends Curve {
}

export class NormalCurve extends Curve {
    getSegments() {
        const idArray = graphEditor.normal.getIdArray(this.id);
        let segments = Array(idArray.length).fill(null);
        for (let i = 0; i < idArray.length; i++) {
            segments[i] = createCurve(idArray[i], this.id);
        }
        return segments;
    }

    addCurve(x: number, y: number, scaleX: number) {
        graphEditor.normal.addCurve(this.id, x, y, scaleX);
    }
}

export function createCurve(id: number, parentId = 0): BezierCurve | ElasticCurve | BounceCurve | LinearCurve | NormalCurve | null {
    switch (graphEditor.getCurveType(id)) {
        case 'bezier':
            return new BezierCurve(id, parentId);

        case 'elastic':
            return new ElasticCurve(id, parentId);

        case 'bounce':
            return new BounceCurve(id, parentId);

        case 'linear':
            return new LinearCurve(id, parentId);

        case 'normal':
            return new NormalCurve(id, parentId);
        
        default:
            return null;
    }
}