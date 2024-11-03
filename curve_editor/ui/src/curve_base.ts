import { editor } from './host_object';


class Curve {
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
        return editor.graph.graph.getAnchorStartX(this.id);
    }

    getAnchorStartY() {
        return editor.graph.graph.getAnchorStartY(this.id);
    }

    setAnchorStart(x: number, y: number) {
        editor.graph.graph.setAnchorStart(this.id, x, y);
    }

    getAnchorEndX() {
        return editor.graph.graph.getAnchorEndX(this.id);
    }

    getAnchorEndY() {
        return editor.graph.graph.getAnchorEndY(this.id);
    }

    setAnchorEnd(x: number, y: number) {
        editor.graph.graph.setAnchorEnd(this.id, x, y);
    }

    reverse() {
        editor.graph.graph.reverse(this.id);
    }
}

export default Curve;