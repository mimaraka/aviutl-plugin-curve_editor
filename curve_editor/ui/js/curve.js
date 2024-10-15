class Curve {
    #id;
    constructor(id) {
        this.#id = id;
    }

    get id() {
        return this.#id;
    }

    getAnchorStartX() {
        return graphEditor.graph.getAnchorStartX(this.id);
    }

    getAnchorStartY() {
        return graphEditor.graph.getAnchorStartY(this.id);
    }

    setAnchorStart(x, y) {
        graphEditor.graph.setAnchorStart(this.id, x, y);
    }

    getAnchorEndX() {
        return graphEditor.graph.getAnchorEndX(this.id);
    }

    getAnchorEndY() {
        return graphEditor.graph.getAnchorEndY(this.id);
    }

    setAnchorEnd(x, y) {
        graphEditor.graph.setAnchorEnd(this.id, x, y);
    }

    getId() {
        return this.id;
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

class NumericCurve extends Curve {
    encode() {
        return graphEditor.numeric.encode(this.id);
    }

    decode(code) {
        return graphEditor.numeric.decode(this.id, code);
    }
}

class BezierCurve extends NumericCurve {
    getHandleLeftX() {
        return graphEditor.bezier.getHandleLeftX(this.id);
    }

    getHandleLeftY() {
        return graphEditor.bezier.getHandleLeftY(this.id);
    }

    setHandleLeft(x, y, keepAngle = false) {
        graphEditor.bezier.setHandleLeft(this.id, x, y, keepAngle);
    }

    getHandleRightX() {
        return graphEditor.bezier.getHandleRightX(this.id);
    }

    getHandleRightY() {
        return graphEditor.bezier.getHandleRightY(this.id);
    }

    setHandleRight(x, y, keepAngle = false) {
        graphEditor.bezier.setHandleRight(this.id, x, y, keepAngle);
    }
}

class ElasticCurve extends NumericCurve {
    getHandleAmpLeftX() {
        return graphEditor.elastic.getHandleAmpLeftX(this.id);
    }

    getHandleAmpLeftY() {
        return graphEditor.elastic.getHandleAmpLeftY(this.id);
    }

    setHandleAmpLeft(y) {
        graphEditor.elastic.setHandleAmpLeft(this.id, y);
    }

    getHandleAmpRightX() {
        return graphEditor.elastic.getHandleAmpRightX(this.id);
    }

    getHandleAmpRightY() {
        return graphEditor.elastic.getHandleAmpRightY(this.id);
    }

    setHandleAmpRight(y) {
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

    setHandleFreqDecay(x, y) {
        graphEditor.elastic.setHandleFreqDecay(this.id, x, y);
    }
}

class BounceCurve extends NumericCurve {
    getHandleX() {
        return graphEditor.bounce.getHandleX(this.id);
    }

    getHandleY() {
        return graphEditor.bounce.getHandleY(this.id);
    }

    setHandle(x, y) {
        graphEditor.bounce.setHandle(this.id, x, y);
    }
}

class LinearCurve extends Curve {
}

class NormalCurve extends Curve {
    getSegments() {
        const idArray = graphEditor.normal.getIdArray(this.id);
        let segments = Array(idArray.length).fill(null);
        for (let i = 0; i < idArray.length; i++) {
            segments[i] = createCurve(idArray[i]);
        }
        return segments;
    }

    addCurve(x, y, scaleX) {
        graphEditor.normal.addCurve(this.id, x, y, scaleX);
    }
}

function createCurve(id) {
    switch (graphEditor.getCurveType(id)) {
        case 'bezier':
            return new BezierCurve(id);

        case 'elastic':
            return new ElasticCurve(id);

        case 'bounce':
            return new BounceCurve(id);

        case 'linear':
            return new LinearCurve(id);

        case 'normal':
            return new NormalCurve(id);
        
        default:
            return null;
    }
}