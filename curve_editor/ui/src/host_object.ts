declare global {
    interface Window {
        chrome: Chrome;
    }

    interface Chrome {
        webview: WebView;
    }

    interface WebView {
        hostObjects: HostObjects;
        addEventListener: (type: string, listener: (event: any) => void) => void;
        removeEventListener: (type: string, listener: (event: any) => void) => void;
        postMessage: (message: any) => void;
    }

    interface HostObjects {
        sync: SyncProxy;
    }

    interface SyncProxy {
        config: ConfigHostObject;
        editor: EditorHostObject;
        preset: PresetHostObject;
    }

    interface ConfigHostObject {
        editMode: number;
        editModeNum: number;
        getEditModeName: (editMode: number) => string;
        layoutMode: number;
        showXLabel: boolean;
        showYLabel: boolean;
        showHandle: boolean;
        alignHandle: boolean;
        enableHotkeys: boolean;
        showVelocityGraph: boolean;
        enableAnimation: boolean;
        wordWrap: boolean;
        invertWheel: boolean;
        setBackgroundImage: boolean;
        backgroundImagePath: string;
        backgroundImageOpacity: number;
        curveResolution: number;
        curveColor: string;
        curveThickness: number;
        notifyUpdate: boolean;
        separatorPos: number;
        presetSize: number;
    }

    interface EditorHostObject {
        getCurveName: (id: number) => string;
        getCurveDispName: (id: number) => string;
        graph: GraphEditorHostObject;
        script: ScriptEditorHostObject;
    }

    interface GraphEditorHostObject {
        getCurveValueArray: (id: number, startX: number, startY: number, endX: number, endY: number, resolution: number) => number[];
        getCurveVelocityArray: (id: number, startX: number, startY: number, endX: number, endY: number, resolution: number) => number[];
        graph: GraphCurveHostObject;
        numeric: NumericGraphCurveHostObject;
        bezier: BezierCurveHostObject;
        elastic: ElasticCurveHostObject;
        bounce: BounceCurveHostObject;
        normal: NormalCurveHostObject;
    }

    interface GraphCurveHostObject {
        getAnchorStartX: (id: number) => number;
        getAnchorStartY: (id: number) => number;
        beginMoveAnchorStart: (id: number) => void;
        moveAnchorStart: (id: number, x: number, y: number) => void;
        getAnchorEndX: (id: number) => number;
        getAnchorEndY: (id: number) => number;
        beginMoveAnchorEnd: (id: number) => void;
        moveAnchorEnd: (id: number, x: number, y: number) => void;
        getPrevCurveId: (id: number) => number;
        getNextCurveId: (id: number) => number;
        reverse: (id: number) => void;
    }

    interface NumericGraphCurveHostObject {
        encode: (id: number) => number;
        decode: (id: number, code: number) => boolean;
    }

    interface BezierCurveHostObject {
        getId: (isSelectDialog: boolean) => number;
        getHandleLeftX: (id: number) => number;
        getHandleLeftY: (id: number) => number;
        beginMoveHandleLeft: (id: number, scaleX: number, scaleY: number) => void;
        moveHandleLeft: (id: number, x: number, y: number, keepAngle: boolean) => void;
        getHandleRightX: (id: number) => number;
        getHandleRightY: (id: number) => number;
        beginMoveHandleRight: (id: number, scaleX: number, scaleY: number) => void;
        moveHandleRight: (id: number, x: number, y: number, keepAngle: boolean) => void;
        getParam: (id: number) => string;
    }

    interface ElasticCurveHostObject {
        getId: (isSelectDialog: boolean) => number;
        getHandleAmpLeftX: (id: number) => number;
        getHandleAmpLeftY: (id: number) => number;
        moveHandleAmpLeft: (id: number, y: number) => void;
        getHandleAmpRightX: (id: number) => number;
        getHandleAmpRightY: (id: number) => number;
        moveHandleAmpRight: (id: number, y: number) => void;
        getHandleFreqDecayX: (id: number) => number;
        getHandleFreqDecayY: (id: number) => number;
        getHandleFreqDecayRootY: (id: number) => number;
        moveHandleFreqDecay: (id: number, x: number, y: number) => void;
        getParam: (id: number) => string;
    }

    interface BounceCurveHostObject {
        getId: (isSelectDialog: boolean) => number;
        getHandleX: (id: number) => number;
        getHandleY: (id: number) => number;
        moveHandle: (id: number, x: number, y: number) => void;
        getParam: (id: number) => string;
    }

    interface NormalCurveHostObject {
        getId: (idx: number) => number;
        getIdArray: (id: number) => number[];
        addCurve: (id: number, x: number, y: number, scaleX: number) => void;
        deleteCurve: (id: number, segmentId: number) => void;
        idx: number;
        size: number;
        appendIdx: () => void;
        popIdx: () => void;
        isIdxMax: boolean;
    }

    interface ScriptEditorHostObject {
        getId: (idx: number) => number;
        getScript: (id: number) => string;
        setScript: (id: number, script: string) => void;
        idx: number;
        size: number;
        appendIdx: () => void;
        popIdx: () => void;
        isIdxMax: boolean;
    }

    interface PresetHostObject {
        getPresetsAsJson: () => string;
        getCollectionsAsJson: () => string;
        getCollectionName: (id: number) => string;
        currentCollectionId: number;
    }
}


export const config = (window.top ?? window).chrome.webview.hostObjects.sync.config;
export const editor = (window.top ?? window).chrome.webview.hostObjects.sync.editor;
export const preset = (window.top ?? window).chrome.webview.hostObjects.sync.preset;