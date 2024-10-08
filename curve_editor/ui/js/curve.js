class Curve {
    constructor(curvePtr) {
        this._curvePtr = curvePtr;
    }

    getAnchorStartX() {
        return graphEditor.graph.getAnchorStartX(this._curvePtr);
    }

    getAnchorStartY() {
        return graphEditor.graph.getAnchorStartY(this._curvePtr);
    }

    setAnchorStart(x, y) {
        graphEditor.graph.setAnchorStart(this._curvePtr, x, y);
    }

    getAnchorEndX() {
        return graphEditor.graph.getAnchorEndX(this._curvePtr);
    }

    getAnchorEndY() {
        return graphEditor.graph.getAnchorEndY(this._curvePtr);
    }

    setAnchorEnd(x, y) {
        graphEditor.graph.setAnchorEnd(this._curvePtr, x, y);
    }

    getCurvePtr() {
        return this._curvePtr;
    }

    getPrevCurve() {
        const curvePtr = graphEditor.graph.getPrevCurvePtr(this._curvePtr);
        return createCurve(curvePtr);
    }

    getNextCurve() {
        const curvePtr = graphEditor.graph.getNextCurvePtr(this._curvePtr);
        return createCurve(curvePtr);
    }
}

class BezierCurve extends Curve {
    getHandleLeftX() {
        return graphEditor.bezier.getHandleLeftX(this._curvePtr);
    }

    getHandleLeftY() {
        return graphEditor.bezier.getHandleLeftY(this._curvePtr);
    }

    setHandleLeft(x, y, keepAngle = false) {
        graphEditor.bezier.setHandleLeft(this._curvePtr, x, y, keepAngle);
    }

    getHandleRightX() {
        return graphEditor.bezier.getHandleRightX(this._curvePtr);
    }

    getHandleRightY() {
        return graphEditor.bezier.getHandleRightY(this._curvePtr);
    }

    setHandleRight(x, y, keepAngle = false) {
        graphEditor.bezier.setHandleRight(this._curvePtr, x, y, keepAngle);
    }
}

class ElasticCurve extends Curve {
    getHandleAmpLeftX() {
        return graphEditor.elastic.getHandleAmpLeftX(this._curvePtr);
    }

    getHandleAmpLeftY() {
        return graphEditor.elastic.getHandleAmpLeftY(this._curvePtr);
    }

    setHandleAmpLeft(y) {
        graphEditor.elastic.setHandleAmpLeft(this._curvePtr, y);
    }

    getHandleAmpRightX() {
        return graphEditor.elastic.getHandleAmpRightX(this._curvePtr);
    }

    getHandleAmpRightY() {
        return graphEditor.elastic.getHandleAmpRightY(this._curvePtr);
    }

    setHandleAmpRight(y) {
        graphEditor.elastic.setHandleAmpRight(this._curvePtr, y);
    }

    getHandleFreqDecayX() {
        return graphEditor.elastic.getHandleFreqDecayX(this._curvePtr);
    }

    getHandleFreqDecayY() {
        return graphEditor.elastic.getHandleFreqDecayY(this._curvePtr);
    }

    getHandleFreqDecayRootY() {
        return graphEditor.elastic.getHandleFreqDecayRootY(this._curvePtr);
    }

    setHandleFreqDecay(x, y) {
        graphEditor.elastic.setHandleFreqDecay(this._curvePtr, x, y);
    }
}

class BounceCurve extends Curve {
    getHandleX() {
        return graphEditor.bounce.getHandleX(this._curvePtr);
    }

    getHandleY() {
        return graphEditor.bounce.getHandleY(this._curvePtr);
    }

    setHandle(x, y) {
        graphEditor.bounce.setHandle(this._curvePtr, x, y);
    }
}

class LinearCurve extends Curve {
}

class NormalCurve extends Curve {
    getSegments() {
        const ptrArray = graphEditor.normal.getPtrArray(this._curvePtr);
        let segments = Array(ptrArray.length).fill(null);
        for (let i = 0; i < ptrArray.length; i++) {
            segments[i] = createCurve(ptrArray[i]);
        }
        return segments;
    }

    addCurve(x, y, scaleX) {
        graphEditor.normal.addCurve(this._curvePtr, x, y, scaleX);
    }
}

function createCurve(curvePtr) {
    switch (graphEditor.getCurveType(curvePtr)) {
        case 'bezier':
            return new BezierCurve(curvePtr);

        case 'elastic':
            return new ElasticCurve(curvePtr);

        case 'bounce':
            return new BounceCurve(curvePtr);

        case 'linear':
            return new LinearCurve(curvePtr);

        case 'normal':
            return new NormalCurve(curvePtr);
        
        default:
            return null;
    }
}

function getCurrentCurve() {
    const currentCurvePtr = graphEditor.getCurrentCurvePtr();
    return createCurve(currentCurvePtr);
}