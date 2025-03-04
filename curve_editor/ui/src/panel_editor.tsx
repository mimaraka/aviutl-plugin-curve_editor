import React from 'react';
import { faPlus, faAngleLeft, faAngleRight } from '@fortawesome/free-solid-svg-icons';
import { config, editor } from './host_object';
import { ToolbarButtonIcon, ToolbarButtonText } from './button';
import GraphEditorPanel from './editor_graph';
import TextEditorPanel from './editor_text';
import './style/panel_editor.scss';


interface ParamButtonProps {
    editMode: number;
    isSelectDialog: boolean;
}

const ParamButton: React.FC<ParamButtonProps> = (props: ParamButtonProps) => {
    const editModeRef = React.useRef(props.editMode);

    const onClick = () => {
        window.chrome.webview.postMessage({
            command: 'ButtonParam',
            curveId: getCurveHostObject()?.getId(props.isSelectDialog) ?? 0
        });
    }

    const getCurveHostObject = () => {
        switch (props.editMode) {
            case 2:
                return editor.graph.bezier;

            case 3:
                return editor.graph.elastic;

            case 4:
                return editor.graph.bounce;
        }
        return null;
    }

    const getParam = () => {
        return getCurveHostObject()?.getParam(getCurveHostObject()?.getId(props.isSelectDialog) ?? 0);
    }

    const [_, setParam] = React.useState(getParam());

    const onMessage = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'UpdateParam':
                setParam(getParam());
                break;
        }
    }

    React.useEffect(() => {
        if (editModeRef.current != props.editMode) {
            editModeRef.current = props.editMode;
            window.removeEventListener('message', onMessage);
        }
        window.addEventListener('message', onMessage);
        return () => {
            window.removeEventListener('message', onMessage);
        }
    }, [props.editMode]);

    return (
        <ToolbarButtonText onClick={onClick} title='カーブのパラメータ' label={getParam() ?? ''} />
    );
}

interface IdButtonsProps {
    idx: number;
    size: number;
    setIdx: (idx: number) => void;
}

const IdButtons: React.FC<IdButtonsProps> = (props: IdButtonsProps) => {
    const refTimeOutBack = React.useRef(0);
    const refIntervalBack = React.useRef(0);
    const refTimeOutForward = React.useRef(0);
    const refIntervalForward = React.useRef(0);
    const tmpIdx = React.useRef(props.idx);
    const holdTime = 500;
    const intervalTime = 60;

    const isIdxFirst = (idx: number) => {
        return idx <= 0;
    }

    const isIdxLast = (idx: number) => {
        return props.size - 1 <= idx;
    }

    const goBack = () => {
        props.setIdx(--tmpIdx.current);
    }

    const goForward = () => {
        props.setIdx(++tmpIdx.current);
    }

    const onBackButtonHoldStart = (event: React.MouseEvent) => {
        if (event.button == 0) {
            tmpIdx.current = props.idx;
            goBack();
            refTimeOutBack.current = window.setTimeout(() => {
                refIntervalBack.current = window.setInterval(() => {
                    goBack();
                    if (isIdxFirst(tmpIdx.current)) {
                        clearInterval(refIntervalBack.current);
                    }
                }, intervalTime);
            }, holdTime);
        }
    }

    const onBackButtonHoldEnd = () => {
        clearTimeout(refTimeOutBack.current);
        clearInterval(refIntervalBack.current);
    }

    const onIdxButtonClick = (event: React.MouseEvent) => {
        window.chrome.webview.postMessage({
            command: 'ButtonIdx'
        });
    }

    const onIdxButtonMouseDown = (event: React.MouseEvent) => {
        if (event.button == 2) {
            window.chrome.webview.postMessage({
                command: 'ContextMenuIdx'
            });
        }
    }

    const onForwardButtonHoldStart = (event: React.MouseEvent) => {
        if (event.button == 0) {
            tmpIdx.current = props.idx;
            goForward();
            refTimeOutForward.current = window.setTimeout(() => {
                refIntervalForward.current = window.setInterval(() => {
                    goForward();
                    if (isIdxLast(tmpIdx.current)) {
                        clearInterval(refIntervalForward.current);
                    }
                }, intervalTime);
            }, holdTime);
        }
    }

    const onForwardButtonHoldEnd = () => {
        clearTimeout(refTimeOutForward.current);
        clearInterval(refIntervalForward.current);
    }

    return (
        <>
            <ToolbarButtonIcon
                style={{ width: '25%' }}
                icon={faAngleLeft}
                title='前のIDに戻る'
                onMouseDown={onBackButtonHoldStart}
                onMouseUp={onBackButtonHoldEnd}
                onMouseLeave={onBackButtonHoldEnd}
                disabled={isIdxFirst(props.idx)}
            />
            <ToolbarButtonText
                style={{ width: '50%' }}
                title='編集中のID'
                label={String(props.idx + 1)}
                onClick={onIdxButtonClick}
                onMouseDown={onIdxButtonMouseDown}
            />
            <ToolbarButtonIcon
                style={{ width: '25%' }}
                icon={isIdxLast(props.idx) ? faPlus : faAngleRight}
                title={isIdxLast(props.idx) ? 'カーブを新規作成' : '次のIDへ進む'}
                onMouseDown={onForwardButtonHoldStart}
                onMouseUp={onForwardButtonHoldEnd}
                onMouseLeave={onForwardButtonHoldEnd}
            />
        </>
    );
}

interface EditorPanelProps {
    style: React.CSSProperties;
    isSelectDialog: boolean;
    editMode: number;
    idx: number;
    size: number;
    setIdx: (idx: number) => void;
    setEditMode: (editMode: number) => void;
}

const EditorPanel: React.FC<EditorPanelProps> = (props: EditorPanelProps) => {
    const editModeOptions = React.useMemo(() => [...Array(config.editModeNum)].map((_, i) => {
        // TODO: disabled~の部分は値指定モードを実装したら削除
        return <option className='dropdown-option' value={i} key={i} disabled={i == 1}>{config.getEditModeName(i)}</option>;
    }), [config.editModeNum]);

    const onSelectChange = (event: React.ChangeEvent<HTMLSelectElement>) => {
        const newMode = parseInt(event.target.value);
        props.setEditMode(newMode);
    }

    const onWheel = (event: React.WheelEvent<HTMLSelectElement>) => {
        //event.preventDefault();
        let newMode;
        if (event.deltaY > 0) {
            // TODO: 値指定モードを実装したらelse以外を削除
            if (props.editMode == 0) {
                newMode = 2;
            } else {
                newMode = Math.min(props.editMode + 1, 5);
            }
        } else {
            // TODO: 値指定モードを実装したらelse以外を削除
            if (props.editMode == 2) {
                newMode = 0;
            } else {
                newMode = Math.max(props.editMode - 1, 0);
            }
        }
        if (props.editMode != newMode) {
            props.setEditMode(newMode);
        }
    }

    return (
        <div className='container-panel-editor' style={props.style}>
            <div className='menu-bottom'>
                <div className='menu-row'>
                    <div className='dropdown-container' id='edit-mode-container'>
                        <select className='dropdown' name='edit-mode' id='edit-mode' title={`編集モード (${config.getEditModeName(props.editMode)})`} value={props.editMode} onChange={onSelectChange} onWheel={onWheel}>
                            {editModeOptions}
                        </select>
                    </div>
                    <div id='buttons-param-id' className='buttons'>
                        {
                            (props.editMode == 2 || props.editMode == 3 || props.editMode == 4) ?
                                <ParamButton isSelectDialog={props.isSelectDialog} editMode={props.editMode} /> :
                                <IdButtons idx={props.idx} size={props.size} setIdx={props.setIdx}/>
                        }
                    </div>
                </div>
            </div>
            {
                props.editMode == 5 ?
                <TextEditorPanel idx={props.idx} /> :
                <GraphEditorPanel
                    isSelectDialog={props.isSelectDialog}
                    editMode={props.editMode}
                    idx={props.idx}
                    size={props.size}
                    setIdx={props.setIdx}
                />
            }
        </div>
    );
}

export default EditorPanel;