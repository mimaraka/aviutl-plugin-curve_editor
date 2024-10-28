import React, { useEffect } from 'react';
import EditorPanel from './panel_editor';
import { graphEditor, scriptEditor } from './host_object';
import './scss/select_dialog.scss';


interface SelectDialogProps {
    editMode: number;
    param: number;
}

const SelectDialog: React.FC<SelectDialogProps> = (props: SelectDialogProps) => {
    const [editMode, setEditMode] = React.useState(props.editMode);
    const [idxNormal, setIdxNormal] = React.useState(editMode == 0 ? props.param - 1 : 0);
    const [idxScript, setIdxScript] = React.useState(editMode == 5 ? props.param - 1 : 0);

    const buttonHeight = parseFloat(getComputedStyle(document.documentElement).getPropertyValue('--okcancel-height'));
    const margin = parseFloat(getComputedStyle(document.documentElement).getPropertyValue('--margin'));
    console.log('debug');

    const changeIdx = (idx: number) => {
        let newIdx = null;
        switch (editMode) {
            case 0:
                if (graphEditor.normal.size <= idx) {
                    graphEditor.normal.appendIdx();
                    newIdx = graphEditor.normal.size - 1;
                } else {
                    newIdx = Math.max(idx, 0);
                }
                setIdxNormal(newIdx);
                break;

            case 1:
                //setIdxValue(idx);
                break;

            case 5:
                if (scriptEditor.size <= idx) {
                    scriptEditor.appendIdx();
                    newIdx = scriptEditor.size - 1;
                } else {
                    newIdx = Math.max(idx, 0);
                }
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
                return graphEditor.normal.size;

            case 1:
                //return graphEditor.value.size;

            case 5:
                return scriptEditor.size;
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
                param = graphEditor.numeric.encode(graphEditor.bezier.getId(true));
                break;

            case 3:
                param = graphEditor.numeric.encode(graphEditor.elastic.getId(true));
                break;

            case 4:
                param = graphEditor.numeric.encode(graphEditor.bounce.getId(true));
                break;

            case 5:
                param = idxScript + 1;
                break
        }
        window.chrome.webview.postMessage({
            command: 'selectdlg-ok',
            mode: editMode,
            param: param
        });
    }

    const onCancelButtonClicked = () => {
        window.chrome.webview.postMessage({
            command: 'selectdlg-cancel'
        });
    }

    useEffect(() => {
        window.postMessage({
            command: 'setCurve',
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
                style={{ height: `${document.documentElement.clientHeight - buttonHeight - margin}px` }}
            />
            <div className='okcancel-row'>
                <button className="okcancel" id="button-ok" onClick={onOkButtonClicked}>OK</button>
                <button className="okcancel" id="button-cancel" onClick={onCancelButtonClicked}>キャンセル</button>
            </div>
        </div>
    );
}

export default SelectDialog;