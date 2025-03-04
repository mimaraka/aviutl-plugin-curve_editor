import React from 'react';
import EditorPanel from './panel_editor';
import { editor } from './host_object';
import './style/select_dialog.scss';


interface SelectDialogProps {
    editMode: number;
    param: number;
}

const SelectDialog: React.FC<SelectDialogProps> = (props: SelectDialogProps) => {
    const [editMode, setEditMode] = React.useState(props.editMode);
    const [idxNormal, setIdxNormal] = React.useState(editMode == 0 ? Math.max(props.param - 1, 0) : 0);
    const [idxScript, setIdxScript] = React.useState(editMode == 5 ? Math.max(props.param - 1, 0) : 0);

    const buttonHeight = parseFloat(getComputedStyle(document.documentElement).getPropertyValue('--okcancel-height'));
    const margin = parseFloat(getComputedStyle(document.documentElement).getPropertyValue('--margin'));

    const changeIdx = (idx: number) => {
        let newIdx = null;
        switch (editMode) {
            case 0:
                if (editor.graph.normal.size <= idx) {
                    editor.graph.normal.appendIdx();
                    newIdx = editor.graph.normal.size - 1;
                } else {
                    newIdx = Math.max(idx, 0);
                }
                setIdxNormal(newIdx);
                break;

            case 1:
                //setIdxValue(idx);
                break;

            case 5:
                if (editor.script.size <= idx) {
                    editor.script.appendIdx();
                    newIdx = editor.script.size - 1;
                } else {
                    newIdx = Math.max(idx, 0);
                }
                setIdxScript(newIdx);
                break;
        }
    }

    const getIdx = () => {
        switch (editMode) {
            case 0:
                return idxNormal;

            case 1:
                //return idxValue;

            case 5:
                return idxScript;
        }
        return 0;
    }

    const getSize = () => {
        switch (editMode) {
            case 0:
                return editor.graph.normal.size;

            case 1:
                //return editor.graph.value.size;

            case 5:
                return editor.script.size;
        }
        return 0;
    }

    const onOkButtonClicked = () => {
        let param = 0;
        switch (editMode) {
            case 0:
                param = idxNormal + 1;
                break;

            case 1:
                //param = idxValue + 1;
                break;

            case 2:
                param = editor.graph.numeric.encode(editor.graph.bezier.getId(true));
                break;

            case 3:
                param = editor.graph.numeric.encode(editor.graph.elastic.getId(true));
                break;

            case 4:
                param = editor.graph.numeric.encode(editor.graph.bounce.getId(true));
                break;

            case 5:
                param = idxScript + 1;
                break
        }
        window.chrome.webview.postMessage({
            command: 'SelectCurveOk',
            mode: editMode,
            param: param
        });
    }

    const onCancelButtonClicked = () => {
        window.chrome.webview.postMessage({
            command: 'SelectCurveCancel'
        });
    }

    React.useEffect(() => {
        window.postMessage({
            command: 'SetCurve',
            mode: editMode,
            param: props.param
        }, '*');
    }, []);

    return (
        <div className='selectdialog-container'>
            <EditorPanel
                isSelectDialog={true}
                editMode={editMode}
                setEditMode={setEditMode}
                idx={getIdx()}
                size={getSize()}
                setIdx={changeIdx}
                style={{ height: `calc(100% - ${buttonHeight - margin}px)` }}
            />
            <div className='okcancel-row'>
                <button className='okcancel' id='button-ok' onClick={onOkButtonClicked}>OK</button>
                <button className='okcancel' id='button-cancel' onClick={onCancelButtonClicked}>キャンセル</button>
            </div>
        </div>
    );
}

export default SelectDialog;