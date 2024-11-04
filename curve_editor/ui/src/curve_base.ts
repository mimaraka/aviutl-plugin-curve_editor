import { editor } from './host_object';


class Curve {
    #id: number;
    #parentId: number;
    #prevId: number;
    #nextId: number;
    constructor(id: number, parentId = 0, prevId = 0, nextId = 0) {
        this.#id = id;
        this.#parentId = parentId;
        this.#prevId = prevId;
        this.#nextId = nextId;
    }

    get id() {
        return this.#id;
    }

    get parentId() {
        return this.#parentId;
    }

    get prevId() {
        return this.#prevId;
    }

    get nextId() {
        return this.#nextId;
    }

    setPrevId(prevId: number) {
        this.#prevId = prevId;
    }

    setNextId(nextId: number) {
        this.#nextId = nextId;
    }

    getAnchorStartX() {
        return editor.graph.graph.getAnchorStartX(this.id);
    }

    getAnchorStartY() {
        return editor.graph.graph.getAnchorStartY(this.id);
    }

    beginMoveAnchorStart() {
        editor.graph.graph.beginMoveAnchorStart(this.id);
    }

    moveAnchorStart(x: number, y: number) {
        editor.graph.graph.moveAnchorStart(this.id, x, y);
    }

    getAnchorEndX() {
        return editor.graph.graph.getAnchorEndX(this.id);
    }

    getAnchorEndY() {
        return editor.graph.graph.getAnchorEndY(this.id);
    }

    beginMoveAnchorEnd() {
        editor.graph.graph.beginMoveAnchorEnd(this.id);
    }

    moveAnchorEnd(x: number, y: number) {
        editor.graph.graph.moveAnchorEnd(this.id, x, y);
    }

    reverse() {
        editor.graph.graph.reverse(this.id);
    }

    getPrevCurve() {
        if (this.prevId === 0) {
            return null;
        }
        return new Curve(this.prevId, this.parentId);
    }

    getNextCurve() {
        if (this.nextId === 0) {
            return null;
        }
        return new Curve(this.nextId, this.parentId);
    }
}

export default Curve;