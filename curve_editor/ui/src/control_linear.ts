import Control from './control_base';


class LinearControl extends Control {
    onAnchorStartDrag(event: DragEvent, bound = false) {
        super.onAnchorStartDrag(event, bound);
        this.update();
    }

    onAnchorEndDrag(event: DragEvent, bound = false) {
        super.onAnchorEndDrag(event, bound);
        this.update();
    }
}

export default LinearControl;