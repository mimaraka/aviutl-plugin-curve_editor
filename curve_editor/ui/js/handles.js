// ハンドル (アンカー)
class Handles {
    // コンストラクタ
    constructor(curve, g, scaleX, scaleY) {
        this.handleRadius = 4.8;
        this.anchorRadius = 3.6;
        this.prevHandleFunc = null;
        this.nextHandleFunc = null;
        this.scaleX = scaleX;
        this.scaleY = scaleY;
        this.curve = curve;
        this._bufferAnchorStartX = curve.getAnchorStartX();
        this._bufferAnchorStartY = curve.getAnchorStartY();
        this._bufferAnchorEndX = curve.getAnchorEndX();
        this._bufferAnchorEndY = curve.getAnchorEndY();
        this.anchorStart = g.append('rect')
            .attr('x', scaleX(this._bufferAnchorStartX) - this.anchorRadius)
            .attr('y', scaleY(this._bufferAnchorStartY) - this.anchorRadius)
            .attr('width', this.anchorRadius * 2)
            .attr('height', this.anchorRadius * 2)
            .attr('class', 'anchor')
            .on('mousedown', event => {
                if (event.button === 2) {
                    event.stopPropagation();
                    window.top.postMessage({
                        command: 'contextmenu-curve-segment',
                        curvePtr: curve.getCurvePtr()
                    });
                }
            });
        this.anchorEnd = g.append('rect')
            .attr('x', scaleX(this._bufferAnchorEndX) - this.anchorRadius)
            .attr('y', scaleY(this._bufferAnchorEndY) - this.anchorRadius)
            .attr('width', this.anchorRadius * 2)
            .attr('height', this.anchorRadius * 2)
            .attr('class', 'anchor');

        this.anchorStart.call(
            d3.drag()
                .on('start', event => { this.onAnchorStartDragStart(event); })
                .on('drag', event => { this.onAnchorStartDrag(event); })
                .on('end', event => { this.onAnchorStartDragEnd(event); })
        );
        this.anchorEnd.call(
            d3.drag()
                .on('start', event => { this.onAnchorEndDragStart(event); })
                .on('drag', event => { this.onAnchorEndDrag(event); })
                .on('end', event => { this.onAnchorEndDragEnd(event); })
        );
    }

    setPrevHandleFunc(func) {
        this.prevHandleFunc = func;
    }

    setNextHandleFunc(func) {
        this.nextHandleFunc = func;
    }

    rescaleX(scaleX, transition = null) {
        this.scaleX = scaleX;
        const startX = this.scaleX(this._bufferAnchorStartX);
        const endX = this.scaleX(this._bufferAnchorEndX);
        if (transition) {
            this.anchorStart.transition(transition)
                .attr('x', startX - this.anchorRadius);
            this.anchorEnd.transition(transition)
                .attr('x', endX - this.anchorRadius);
        } else {
            this.anchorStart
                .attr('x', startX - this.anchorRadius);
            this.anchorEnd
                .attr('x', endX - this.anchorRadius);
        }
    }

    rescaleY(scaleY, transition = null) {
        this.scaleY = scaleY;
        const startY = this.scaleY(this._bufferAnchorStartY);
        const endY = this.scaleY(this._bufferAnchorEndY);
        if (transition) {
            this.anchorStart.transition(transition)
                .attr('y', startY - this.anchorRadius);
            this.anchorEnd.transition(transition)
                .attr('y', endY - this.anchorRadius);
        } else {
            this.anchorStart
                .attr('y', startY - this.anchorRadius);
            this.anchorEnd
                .attr('y', endY - this.anchorRadius);
        }
    }

    update() {
        this.updateHandles();
        this.updateCurvePath();
    }

    updateHandles() {
        this.anchorStart
            .attr('x', this.scaleX(this.curve.getAnchorStartX()) - this.anchorRadius)
            .attr('y', this.scaleY(this.curve.getAnchorStartY()) - this.anchorRadius);
        this.anchorEnd
            .attr('x', this.scaleX(this.curve.getAnchorEndX()) - this.anchorRadius)
            .attr('y', this.scaleY(this.curve.getAnchorEndY()) - this.anchorRadius);
    }

    updateCurvePath() {
        window.postMessage({command: 'updateCurvePath'});
    }

    // 開始アンカーのドラッグ開始
    onAnchorStartDragStart(event, bound = false) {
        if (!bound) {
            this.prevHandleFunc?.onAnchorEndDragStart(event, true);
        }
    }

    // 開始アンカーのドラッグ
    onAnchorStartDrag(event, bound = false) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        this.curve.setAnchorStart(x, y);
        const newX = this.curve.getAnchorStartX();
        const newY = this.curve.getAnchorStartY();
        this.anchorStart
            .attr('x', this.scaleX(newX) - this.anchorRadius)
            .attr('y', this.scaleY(newY) - this.anchorRadius);
        
        if (!bound) {
            this.prevHandleFunc?.onAnchorEndDrag(event, true);
        }
    }

    // 開始アンカーのドラッグ終了
    onAnchorStartDragEnd(event, bound = false) {
        this._bufferAnchorStartX = this.curve.getAnchorStartX();
        this._bufferAnchorStartY = this.curve.getAnchorStartY();
        if (!bound) {
            this.prevHandleFunc?.onAnchorEndDragEnd(event, true);
        }
    }

    // 終了アンカーのドラッグ開始
    onAnchorEndDragStart(event, bound = false) {
        if (!bound) {
            this.nextHandleFunc?.onAnchorStartDragStart(event, true);
        }
    }

    // 終了アンカーのドラッグ
    onAnchorEndDrag(event, bound = false) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        this.curve.setAnchorEnd(x, y);
        const newX = this.curve.getAnchorEndX();
        const newY = this.curve.getAnchorEndY();
        this.anchorEnd
            .attr('x', this.scaleX(newX) - this.anchorRadius)
            .attr('y', this.scaleY(newY) - this.anchorRadius);
        if (!bound) {
            this.nextHandleFunc?.onAnchorStartDrag(event, true);
        }
    }

    // 終了アンカーのドラッグ終了
    onAnchorEndDragEnd(event, bound = false) {
        this._bufferAnchorEndX = this.curve.getAnchorEndX();
        this._bufferAnchorEndY = this.curve.getAnchorEndY();
        if (!bound) {
            this.nextHandleFunc?.onAnchorStartDragEnd(event, true);
        }
    }

    // ハンドル・アンカーを削除
    remove() {
        this.anchorStart.remove();
        this.anchorEnd.remove();
    }
}

// ハンドル (ベジェ)
class BezierHandles extends Handles {
    #bufferHandleLeftOffsetX;
    #bufferHandleLeftOffsetY;
    #bufferHandleRightOffsetX;
    #bufferHandleRightOffsetY;
    #lengthBuffer;
    #alignHandleFlag = false;

    // コンストラクタ
    constructor(curve, g, scaleX, scaleY) {
        super(curve, g, scaleX, scaleY);
        this.handleLeft = g.append('circle')
            .attr('cx', scaleX(curve.getHandleLeftX()))
            .attr('cy', scaleY(curve.getHandleLeftY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleRight = g.append('circle')
            .attr('cx', scaleX(curve.getHandleRightX()))
            .attr('cy', scaleY(curve.getHandleRightY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleLineLeft = g.append('line')
            .attr('x1', scaleX(this._bufferAnchorStartX))
            .attr('y1', scaleY(this._bufferAnchorStartY))
            .attr('x2', scaleX(curve.getHandleLeftX()))
            .attr('y2', scaleY(curve.getHandleLeftY()))
            .attr('class', 'handle-line');
        this.handleLineRight = g.append('line')
            .attr('x1', scaleX(this._bufferAnchorEndX))
            .attr('y1', scaleY(this._bufferAnchorEndY))
            .attr('x2', scaleX(curve.getHandleRightX()))
            .attr('y2', scaleY(curve.getHandleRightY()))
            .attr('class', 'handle-line');
        this.handleLeft.call(
            d3.drag()
                .on('start', event => { this.onHandleLeftDragStart(event); })
                .on('drag', event => { this.onHandleLeftDrag(event); })
                .on('end', event => { this.onHandleLeftDragEnd(event); })
        );
        this.handleRight.call(
            d3.drag()
                .on('start', event => { this.onHandleRightDragStart(event); })
                .on('drag', event => { this.onHandleRightDrag(event); })
                .on('end', event => { this.onHandleRightDragEnd(event); })
        );
    }

    rescaleX(scaleX, transition = null) {
        super.rescaleX(scaleX, transition);
        const leftX = this.scaleX(this.curve.getHandleLeftX());
        const rightX = this.scaleX(this.curve.getHandleRightX());
        const startX = this.scaleX(this._bufferAnchorStartX);
        const endX = this.scaleX(this._bufferAnchorEndX);
        if (transition) {
            this.handleLeft.transition(transition)
                .attr('cx', leftX);
            this.handleRight.transition(transition)
                .attr('cx', rightX);
            this.handleLineLeft.transition(transition)
                .attr('x1', startX)
                .attr('x2', leftX);
            this.handleLineRight.transition(transition)
                .attr('x1', endX)
                .attr('x2', rightX);
        } else {
            this.handleLeft
                .attr('cx', leftX);
            this.handleRight
                .attr('cx', rightX);
            this.handleLineLeft
                .attr('x1', startX)
                .attr('x2', leftX);
            this.handleLineRight
                .attr('x1', endX)
                .attr('x2', rightX);
        }
    }

    rescaleY(scaleY, transition = null) {
        super.rescaleY(scaleY, transition);
        const leftY = this.scaleY(this.curve.getHandleLeftY());
        const rightY = this.scaleY(this.curve.getHandleRightY());
        const startY = this.scaleY(this._bufferAnchorStartY);
        const endY = this.scaleY(this._bufferAnchorEndY);
        if (transition) {
            this.handleLeft.transition(transition)
                .attr('cy', leftY);
            this.handleRight.transition(transition)
                .attr('cy', rightY);
            this.handleLineLeft.transition(transition)
                .attr('y1', startY)
                .attr('y2', leftY);
            this.handleLineRight.transition(transition)
                .attr('y1', endY)
                .attr('y2', rightY);
        } else {
            this.handleLeft
                .attr('cy', leftY);
            this.handleRight
                .attr('cy', rightY);
            this.handleLineLeft
                .attr('y1', startY)
                .attr('y2', leftY);
            this.handleLineRight
                .attr('y1', endY)
                .attr('y2', rightY);
        }
    }

    updateHandles() {
        super.updateHandles();
        const anchorStartX = this.scaleX(this.curve.getAnchorStartX());
        const anchorStartY = this.scaleY(this.curve.getAnchorStartY());
        const anchorEndX = this.scaleX(this.curve.getAnchorEndX());
        const anchorEndY = this.scaleY(this.curve.getAnchorEndY());
        const handleLeftX = this.scaleX(this.curve.getHandleLeftX());
        const handleLeftY = this.scaleY(this.curve.getHandleLeftY());
        const handleRightX = this.scaleX(this.curve.getHandleRightX());
        const handleRightY = this.scaleY(this.curve.getHandleRightY());
        this.handleLeft
            .attr('cx', handleLeftX)
            .attr('cy', handleLeftY);
        this.handleRight
            .attr('cx', handleRightX)
            .attr('cy', handleRightY);
        this.handleLineLeft
            .attr('x1', anchorStartX)
            .attr('y1', anchorStartY)
            .attr('x2', handleLeftX)
            .attr('y2', handleLeftY);
        this.handleLineRight
            .attr('x1', anchorEndX)
            .attr('y1', anchorEndY)
            .attr('x2', handleRightX)
            .attr('y2', handleRightY);
    }

    onAnchorDragStart() {
        this.#bufferHandleLeftOffsetX = this.curve.getHandleLeftX() - this.curve.getAnchorStartX();
        this.#bufferHandleLeftOffsetY = this.curve.getHandleLeftY() - this.curve.getAnchorStartY();
        this.#bufferHandleRightOffsetX = this.curve.getHandleRightX() - this.curve.getAnchorEndX();
        this.#bufferHandleRightOffsetY = this.curve.getHandleRightY() - this.curve.getAnchorEndY();
    }

    onAnchorDrag() {
        const anchorStartX = this.curve.getAnchorStartX();
        const anchorStartY = this.curve.getAnchorStartY();
        const anchorEndX = this.curve.getAnchorEndX();
        const anchorEndY = this.curve.getAnchorEndY();
        this.curve.setHandleLeft(anchorStartX + this.#bufferHandleLeftOffsetX, anchorStartY + this.#bufferHandleLeftOffsetY, true);
        this.curve.setHandleRight(anchorEndX + this.#bufferHandleRightOffsetX, anchorEndY + this.#bufferHandleRightOffsetY, true);
        this.updateHandles();
    }

    // 開始アンカーのドラッグ開始
    onAnchorStartDragStart(event, bound = false) {
        super.onAnchorStartDragStart(event, bound);
        this.onAnchorDragStart();
    }

    // 開始アンカーのドラッグ
    onAnchorStartDrag(event, bound = false) {
        super.onAnchorStartDrag(event, bound);
        this.onAnchorDrag();
        if (!bound) {
            this.updateCurvePath();
        }
    }

    // 終了アンカーのドラッグ開始
    onAnchorEndDragStart(event, bound = false) {
        super.onAnchorEndDragStart(event, bound);
        this.onAnchorDragStart();
    }

    // 終了アンカーのドラッグ
    onAnchorEndDrag(event, bound = false) {
        super.onAnchorEndDrag(event, bound);
        this.onAnchorDrag();
        if (!bound) {
            this.updateCurvePath();
        }
    }

    // 左ハンドルのドラッグ開始
    onHandleLeftDragStart(event) {
        const prevCurve = this.curve.getPrevCurve();
        if ((prevCurve instanceof BezierCurve) && config.alignHandle) {
            this.#alignHandleFlag = true;
            const prevOffsetX = prevCurve.getHandleRightX() - prevCurve.getAnchorEndX();
            const prevOffsetY = prevCurve.getHandleRightY() - prevCurve.getAnchorEndY();
            const scaledPrevOffsetX = this.scaleX(prevOffsetX) - this.scaleX(0);
            const scaledPrevOffsetY = this.scaleY(prevOffsetY) - this.scaleY(0);
            this.#lengthBuffer = Math.sqrt(scaledPrevOffsetX ** 2 + scaledPrevOffsetY ** 2);
        }
    }

    // 左ハンドルのドラッグ
    onHandleLeftDrag(event) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        this.curve.setHandleLeft(x, y);
        const newX = this.curve.getHandleLeftX();
        const newY = this.curve.getHandleLeftY();
        this.handleLeft
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.handleLineLeft
            .attr('x2', this.scaleX(newX))
            .attr('y2', this.scaleY(newY));
        // 左ハンドルをドラッグしたときに、前のカーブの右ハンドルも移動する
        if (this.#alignHandleFlag) {
            const prevCurve = this.curve.getPrevCurve();
            const scaledAngle = Math.atan2(
                -this.scaleY(newY) + this.scaleY(this._bufferAnchorStartY),
                this.scaleX(newX) - this.scaleX(this._bufferAnchorStartX)
            );
            const scaledX = this.scaleX(this._bufferAnchorStartX) + this.#lengthBuffer * Math.cos(scaledAngle + Math.PI);
            const scaledY = this.scaleY(this._bufferAnchorStartY) - this.#lengthBuffer * Math.sin(scaledAngle + Math.PI);
            prevCurve.setHandleRight(this.scaleX.invert(scaledX), this.scaleY.invert(scaledY), true);
            this.prevHandleFunc?.update();
        } else {
            this.updateCurvePath();
        }
    }

    // 左ハンドルのドラッグ終了
    onHandleLeftDragEnd(event) {
        this.#alignHandleFlag = false;
    }

    // 右ハンドルのドラッグ開始
    onHandleRightDragStart(event) {
        const nextCurve = this.curve.getNextCurve();
        if ((nextCurve instanceof BezierCurve) && config.alignHandle) {
            this.#alignHandleFlag = true;
            const nextOffsetX = nextCurve.getHandleLeftX() - nextCurve.getAnchorStartX();
            const nextOffsetY = nextCurve.getHandleLeftY() - nextCurve.getAnchorStartY();
            const scaledNextOffsetX = this.scaleX(nextOffsetX) - this.scaleX(0);
            const scaledNextOffsetY = this.scaleY(nextOffsetY) - this.scaleY(0);
            this.#lengthBuffer = Math.sqrt(scaledNextOffsetX ** 2 + scaledNextOffsetY ** 2);
        }
    }

    // 右ハンドルのドラッグ
    onHandleRightDrag(event) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        this.curve.setHandleRight(x, y);
        const newX = this.curve.getHandleRightX();
        const newY = this.curve.getHandleRightY();
        this.handleRight
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.handleLineRight
            .attr('x2', this.scaleX(newX))
            .attr('y2', this.scaleY(newY));
        // 右ハンドルをドラッグしたときに、次のカーブの左ハンドルも移動する
        if (this.#alignHandleFlag) {
            const nextCurve = this.curve.getNextCurve();
            const scaledAngle = Math.atan2(
                -this.scaleY(newY) + this.scaleY(this._bufferAnchorEndY),
                this.scaleX(newX) - this.scaleX(this._bufferAnchorEndX)
            );
            const scaledX = this.scaleX(this._bufferAnchorEndX) + this.#lengthBuffer * Math.cos(scaledAngle + Math.PI);
            const scaledY = this.scaleY(this._bufferAnchorEndY) - this.#lengthBuffer * Math.sin(scaledAngle + Math.PI);
            nextCurve.setHandleLeft(this.scaleX.invert(scaledX), this.scaleY.invert(scaledY), true);
            this.nextHandleFunc?.update();
        } else {
            this.updateCurvePath();
        }
    }

    // 右ハンドルのドラッグ終了
    onHandleRightDragEnd(event) {
        this.#alignHandleFlag = false;
    }

    // ハンドル・アンカーを削除
    remove() {
        super.remove();
        this.handleLeft.remove();
        this.handleRight.remove();
        this.handleLineLeft.remove();
        this.handleLineRight.remove();
    }
}

// ハンドル (振動)
class ElasticHandles extends Handles {
    constructor(curve, g, scaleX, scaleY) {
        super(curve, g, scaleX, scaleY);
        this.handleAmpLeft = g.append('circle')
            .attr('cx', scaleX(curve.getHandleAmpLeftX()))
            .attr('cy', scaleY(curve.getHandleAmpLeftY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleAmpRight = g.append('circle')
            .attr('cx', scaleX(curve.getHandleAmpRightX()))
            .attr('cy', scaleY(curve.getHandleAmpRightY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleFreqDecay = g.append('circle')
            .attr('cx', scaleX(curve.getHandleFreqDecayX()))
            .attr('cy', scaleY(curve.getHandleFreqDecayY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handleFreqDecayRoot = g.append('rect')
            .attr('x', scaleX(curve.getHandleFreqDecayX()) - this.anchorRadius)
            .attr('y', scaleY(curve.getHandleFreqDecayRootY()) - this.anchorRadius)
            .attr('width', this.anchorRadius * 2)
            .attr('height', this.anchorRadius * 2)
            .attr('class', 'anchor');
        this.handleLineAmp = g.append('line')
            .attr('x1', scaleX(curve.getHandleAmpLeftX()))
            .attr('y1', scaleY(curve.getHandleAmpLeftY()))
            .attr('x2', scaleX(curve.getHandleAmpRightX()))
            .attr('y2', scaleY(curve.getHandleAmpRightY()))
            .attr('class', 'handle-line');
        this.handleLineFreqDecay = g.append('line')
            .attr('x1', scaleX(curve.getHandleFreqDecayX()))
            .attr('y1', scaleY(curve.getHandleFreqDecayRootY()))
            .attr('x2', scaleX(curve.getHandleFreqDecayX()))
            .attr('y2', scaleY(curve.getHandleFreqDecayY()))
            .attr('class', 'handle-line');
        this.handleAmpLeft.call(
            d3.drag()
                .on('drag', event => { this.onHandleAmpLeftDrag(event); })
        );
        this.handleAmpRight.call(
            d3.drag()
                .on('drag', event => { this.onHandleAmpRightDrag(event); })
        );
        this.handleFreqDecay.call(
            d3.drag()
                .on('drag', event => { this.onHandleFreqDecayDrag(event); })
        );
    }

    rescaleX(scaleX, transition = null) {
        super.rescaleX(scaleX, transition);
        const ampLeftX = this.scaleX(this.curve.getHandleAmpLeftX());
        const ampRightX = this.scaleX(this.curve.getHandleAmpRightX());
        const freqDecayX = this.scaleX(this.curve.getHandleFreqDecayX());
        if (transition) {
            this.handleAmpLeft.transition(transition)
                .attr('cx', ampLeftX);
            this.handleAmpRight.transition(transition)
                .attr('cx', ampRightX);
            this.handleFreqDecay.transition(transition)
                .attr('cx', freqDecayX);
            this.handleFreqDecayRoot.transition(transition)
                .attr('x', freqDecayX - this.anchorRadius);
            this.handleLineAmp.transition(transition)
                .attr('x1', ampLeftX)
                .attr('x2', ampRightX);
            this.handleLineFreqDecay.transition(transition)
                .attr('x1', freqDecayX)
                .attr('x2', freqDecayX);
        }
        else {
            this.handleAmpLeft
                .attr('cx', ampLeftX);
            this.handleAmpRight
                .attr('cx', ampRightX);
            this.handleFreqDecay
                .attr('cx', freqDecayX);
            this.handleFreqDecayRoot
                .attr('x', freqDecayX - this.anchorRadius);
            this.handleLineAmp
                .attr('x1', ampLeftX)
                .attr('x2', ampRightX);
            this.handleLineFreqDecay
                .attr('x1', freqDecayX)
                .attr('x2', freqDecayX);
        }
    }

    rescaleY(scaleY, transition = null) {
        super.rescaleY(scaleY, transition);
        const ampY = this.scaleY(this.curve.getHandleAmpLeftY());
        const freqDecayY = this.scaleY(this.curve.getHandleFreqDecayY());
        const freqDecayRootY = this.scaleY(this.curve.getHandleFreqDecayRootY());
        if (transition) {
            this.handleAmpLeft.transition(transition)
                .attr('cy', ampY);
            this.handleAmpRight.transition(transition)
                .attr('cy', ampY);
            this.handleFreqDecay.transition(transition)
                .attr('cy', freqDecayY);
            this.handleFreqDecayRoot.transition(transition)
                .attr('y', freqDecayRootY - this.anchorRadius);
            this.handleLineAmp.transition(transition)
                .attr('y1', ampY)
                .attr('y2', ampY);
            this.handleLineFreqDecay.transition(transition)
                .attr('y1', freqDecayRootY)
                .attr('y2', freqDecayY);
        }
        else {
            this.handleAmpLeft
                .attr('cy', ampY);
            this.handleAmpRight
                .attr('cy', ampY);
            this.handleFreqDecay
                .attr('cy', freqDecayY);
            this.handleFreqDecayRoot
                .attr('y', freqDecayRootY - this.anchorRadius);
            this.handleLineAmp
                .attr('y1', ampY)
                .attr('y2', ampY);
            this.handleLineFreqDecay
                .attr('y1', freqDecayRootY)
                .attr('y2', freqDecayY);
        }
    }

    updateHandles() {
        super.updateHandles();
        const ampLeftX = this.scaleX(this.curve.getHandleAmpLeftX());
        const ampRightX = this.scaleX(this.curve.getHandleAmpRightX());
        const ampY = this.scaleY(this.curve.getHandleAmpLeftY());
        const freqDecayX = this.scaleX(this.curve.getHandleFreqDecayX());
        const freqDecayY = this.scaleY(this.curve.getHandleFreqDecayY());
        const freqDecayRootY = this.scaleY(this.curve.getHandleFreqDecayRootY());
        this.handleAmpLeft
            .attr('cx', ampLeftX)
            .attr('cy', ampY);
        this.handleAmpRight
            .attr('cx', ampRightX)
            .attr('cy', ampY);
        this.handleFreqDecay
            .attr('cx', freqDecayX)
            .attr('cy', freqDecayY);
        this.handleFreqDecayRoot
            .attr('x', freqDecayX - this.anchorRadius)
            .attr('y', freqDecayRootY - this.anchorRadius);
        this.handleLineAmp
            .attr('x1', ampLeftX)
            .attr('y1', ampY)
            .attr('x2', ampRightX)
            .attr('y2', ampY);
        this.handleLineFreqDecay
            .attr('x1', freqDecayX)
            .attr('y1', freqDecayRootY)
            .attr('x2', freqDecayX)
            .attr('y2', freqDecayY);
    }

    onAnchorStartDrag(event) {
        this.update();
    }

    onAnchorEndDrag(event) {
        this.update();
    }

    onHandleAmpLeftDrag(event) {
        const y = this.scaleY.invert(event.y);
        this.curve.setHandleAmpLeft(y);
        const newX = this.curve.getHandleAmpLeftX();
        const newY = this.curve.getHandleAmpLeftY();
        this.handleAmpLeft
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.update();
    }

    onHandleAmpRightDrag(event) {
        const y = this.scaleY.invert(event.y);
        this.curve.setHandleAmpRight(y);
        const newX = this.curve.getHandleAmpRightX();
        const newY = this.curve.getHandleAmpRightY();
        this.handleAmpRight
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.update();
    }

    onHandleFreqDecayDrag(event) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        this.curve.setHandleFreqDecay(x, y);
        const newX = this.curve.getHandleFreqDecayX();
        const newY = this.curve.getHandleFreqDecayY();
        this.handleFreqDecay
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.update();
    }

    // ハンドル・アンカーを削除
    remove() {
        super.remove();
        this.handleAmpLeft.remove();
        this.handleAmpRight.remove();
        this.handleFreqDecay.remove();
        this.handleFreqDecayRoot.remove();
        this.handleLineAmp.remove();
        this.handleLineFreqDecay.remove();
    }
}

// ハンドル (バウンス)
class BounceHandles extends Handles {
    // コンストラクタ
    constructor(curve, g, scaleX, scaleY) {
        super(curve, g, scaleX, scaleY);
        this.handle = g.append('circle')
            .attr('cx', scaleX(curve.getHandleX()))
            .attr('cy', scaleY(curve.getHandleY()))
            .attr('r', this.handleRadius)
            .attr('class', 'handle');
        this.handle.call(
            d3.drag()
                .on('drag', event => { this.onHandleDrag(event); })
        );
    }

    rescaleX(scaleX, transition = null) {
        super.rescaleX(scaleX, transition);
        const x = this.scaleX(this.curve.getHandleX());
        if (transition) {
            this.handle.transition(transition)
                .attr('cx', x);
        }
        else {
            this.handle
                .attr('cx', x);
        }
    }

    rescaleY(scaleY, transition = null) {
        super.rescaleY(scaleY, transition);
        const y = this.scaleY(this.curve.getHandleY());
        if (transition) {
            this.handle.transition(transition)
                .attr('cy', y);
        }
        else {
            this.handle
                .attr('cy', y);
        }
    }

    updateHandles() {
        super.updateHandles();
        this.handle
            .attr('cx', this.scaleX(this.curve.getHandleX()))
            .attr('cy', this.scaleY(this.curve.getHandleY()));
    }

    onAnchorStartDrag(event) {
        this.update();
    }

    onAnchorEndDrag(event) {
        this.update();
    }

    onHandleDrag(event) {
        const x = this.scaleX.invert(event.x);
        const y = this.scaleY.invert(event.y);
        this.curve.setHandle(x, y);
        const newX = this.curve.getHandleX();
        const newY = this.curve.getHandleY();
        this.handle
            .attr('cx', this.scaleX(newX))
            .attr('cy', this.scaleY(newY));
        this.updateCurvePath();
    }

    // ハンドル・アンカーを削除
    remove() {
        super.remove();
        this.handle.remove();
    }
}

// ハンドル (標準)
class NormalHandles extends Handles {
    // コンストラクタ
    constructor(curve, g, scaleX, scaleY) {
        super(curve, g, scaleX, scaleY);
        const segments = curve.getSegments();
        this.segmentHandlesArray = Array(segments.length).fill(null);
        for (let i = 0; i < segments.length; i++) {
            this.segmentHandlesArray[i] = createHandles(segments[i], g, scaleX, scaleY);
        }
        for (let i = 0; i < segments.length; i++) {
            const prevHandleFunc = (i > 0) ? {
                onAnchorEndDragStart: this.segmentHandlesArray[i - 1].onAnchorEndDragStart.bind(this.segmentHandlesArray[i - 1]),
                onAnchorEndDrag: this.segmentHandlesArray[i - 1].onAnchorEndDrag.bind(this.segmentHandlesArray[i - 1]),
                onAnchorEndDragEnd: this.segmentHandlesArray[i - 1].onAnchorEndDragEnd.bind(this.segmentHandlesArray[i - 1]),
                update: this.segmentHandlesArray[i - 1].update.bind(this.segmentHandlesArray[i - 1])
            } : null;
            const nextHandleFunc = (i < segments.length - 1) ? {
                onAnchorStartDragStart: this.segmentHandlesArray[i + 1].onAnchorStartDragStart.bind(this.segmentHandlesArray[i + 1]),
                onAnchorStartDrag: this.segmentHandlesArray[i + 1].onAnchorStartDrag.bind(this.segmentHandlesArray[i + 1]),
                onAnchorStartDragEnd: this.segmentHandlesArray[i + 1].onAnchorStartDragEnd.bind(this.segmentHandlesArray[i + 1]),
                update: this.segmentHandlesArray[i + 1].update.bind(this.segmentHandlesArray[i + 1])
            } : null;
            this.segmentHandlesArray[i].setPrevHandleFunc(prevHandleFunc);
            this.segmentHandlesArray[i].setNextHandleFunc(nextHandleFunc);
        }
    }

    rescaleX(scaleX, transition = null) {
        super.rescaleX(scaleX, transition);
        this.segmentHandlesArray.forEach((handle) => {
            handle.rescaleX(scaleX, transition);
        });
    }

    rescaleY(scaleY, transition = null) {
        super.rescaleY(scaleY, transition);
        this.segmentHandlesArray.forEach((handle) => {
            handle.rescaleY(scaleY, transition);
        });
    }

    // ハンドル・アンカーを削除
    remove() {
        super.remove();
        this.segmentHandlesArray.forEach((handle) => {
            handle.remove();
        });
    }
}

// ハンドルを作成
function createHandles(curve, g, scaleX, scaleY) {
    if (curve instanceof BezierCurve) {
        return new BezierHandles(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof ElasticCurve) {
        return new ElasticHandles(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof BounceCurve) {
        return new BounceHandles(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof LinearCurve) {
        return new Handles(curve, g, scaleX, scaleY);
    }
    else if (curve instanceof NormalCurve) {
        return new NormalHandles(curve, g, scaleX, scaleY);
    }
}