'use strict';

const getEditMode = () => {
    if (window.top.isSelectDialog) {
        return window.top.editMode;
    }
    else {
        return config.editMode;
    }
}

const isEditModeNumeric = () => {
    const editMode = getEditMode();
    return editMode == 2 || editMode == 3 || editMode == 4;
}

const isEditModeId = () => !isEditModeNumeric();

const setEditMode = editMode => {
    if (window.top.isSelectDialog) {
        window.top.editMode = editMode;
    }
    else {
        config.editMode = editMode;
    }
}

const getEditorHostObject = () => {
    switch (getEditMode()) {
        case 0:
            return graphEditor.normal;

        case 1:
            //return graphEditor.value;
            return null;

        case 5:
            return scriptEditor;
    }
    return null;
}

const getIdx = () => {
    switch (getEditMode()) {
        case 0:
            return window.top.isSelectDialog ?  window.top.idxNormal : graphEditor.normal.idx;

        case 1:
            //return window.top.isSelectDialog ? window.top.idxValue : graphEditor.value.idx;
            return null;

        case 5:
            return window.top.isSelectDialog ? window.top.idxScript : scriptEditor.idx;
    }
    return null;
}

const getSize = () => {
    return getEditorHostObject()?.size;
}

const isIdxFirst = () => {
    return getIdx() == 0;
}

const isIdxLast = () => {
    return getIdx() == getSize() - 1;
}

const setIdx = idx => {
    let ret;
    if (idx < 0) {
        ret = 0;
    }
    else if (getSize() <= idx) {
        getEditorHostObject().appendIdx();
        ret = getSize() - 1;
    }
    else {
        ret = idx;
    }
    if (window.top.isSelectDialog) {
        switch (getEditMode()) {
            case 0:
                window.top.idxNormal = ret;
                break;

            case 1:
                //window.top.idxValue = ret;
                break;

            case 5:
                window.top.idxScript = ret;
                break;
        }
    } else {
        getEditorHostObject().idx = ret;
    }
}

const advanceIdx = n => {
    const currentIdx = getIdx();
    if (currentIdx != null) {
        setIdx(currentIdx + n);
    }
}