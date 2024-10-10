window.addEventListener('message', event => {
    if (event.data.to == 'panel-editor') {
        switch (event.data.command) {
            case 'changeId':
                updateIdButtons();
                break;

            case 'updateParam':
                updateParamButton();
                break;

            case 'goBackId':
                goBackId();
                break;

            case 'goForwardId':
                goForwardId();
                break;
        }
    }
    else if (event.data.to == 'editor-graph') {
        $('#editor')[0].contentWindow.postMessage(event.data, '*');
    }
    else {
        window.top.chrome.webview.postMessage(event.data);
    }
});

const goBackId = () => {
    editor.advanceIdx(-1);
    updateIdButtons();
    $('#editor')[0].contentWindow.postMessage({
        command: 'changeId'
    }, '*');
}

const goForwardId = () => {
    editor.advanceIdx(1);
    updateIdButtons();
    $('#editor')[0].contentWindow.postMessage({
        command: 'changeId'
    }, '*');
}


// IDボタン群を作成
const createIdButtons = () => {
    $('#buttons-param-id').append('<button class="menu-button" id="button-id-back" style="width: 25%;" title="前のIDに戻る"><i class="fa-solid fa-angle-left"></i></button>');
    $('#buttons-param-id').append('<button class="menu-button" id="button-id" style="width: 50%;" title="編集中のID"><p class="content"></p></button>');
    $('#buttons-param-id').append('<button class="menu-button" id="button-id-forward" style="width: 25%;"><i class="fa-solid fa-angle-right"></i></button>');

    $('#button-id').on('click', () => {
        window.top.postMessage({
            to: 'native',
            command: 'button-id'
        }, '*');
    });

    $('#button-id-back').on('mousedown', () => {
        goBackId();
    });

    $('#button-id-forward').on('mousedown', () => {
        goForwardId();
    });
}

// IDボタン群の状態を更新
const updateIdButtons = () => {
    $('#button-id-back').prop('disabled', editor.isIdxFirst);
    if (editor.isIdxLast) {
        $('#button-id-forward i').addClass('fa-plus').removeClass('fa-angle-right');
        $('#button-id-forward').prop('title', 'カーブを新規作成');
    }
    else {
        $('#button-id-forward i').addClass('fa-angle-right').removeClass('fa-plus');
        $('#button-id-forward').prop('title', '次のIDへ進む');
    }
    $('#button-id p').html(editor.currentIdx + 1);
}

const createParamButton = () => {
    $('#buttons-param-id').append('<button class="menu-button" id="button-param" title="カーブのパラメータ"></button>');
    $('#button-param').on('click', () => {
        window.top.postMessage({
            to: 'native',
            command: 'button-param'
        }, '*');
    });
}

const updateParamButton = () => {
    const editMode = config.editMode;
    let param;
    if (editMode == 2) {
        param = graphEditor.bezier.getParam(graphEditor.getCurrentCurvePtr());
    }
    else if (editMode == 3) {
        param = graphEditor.elastic.getParam(graphEditor.getCurrentCurvePtr());
    }
    else if (editMode == 4) {
        param = graphEditor.bounce.getParam(graphEditor.getCurrentCurvePtr());
    }
    if (param != undefined) {
        $('#button-param').html(param);
    }
}

const onEditModeChange = () => {
    const editMode = config.editMode;
    $('#edit-mode').val(editMode);
    $('#buttons-param-id').empty();
    if (editMode == 0 || editMode == 1 || editMode == 5) {
        createIdButtons();
        updateIdButtons();
    } else {
        createParamButton();
        updateParamButton();
    }
    if (editMode == 5) {
        $('#editor').attr('src', 'editor_text.html');
    } else {
        $('#editor').attr('src', 'editor_graph.html');
    }
    window.top.postMessage({
        to: 'toolbar',
        command: 'changeEditMode'
    }, '*');
}

$(document).ready(() => {
    // ドロップダウンメニューの項目を追加
    for (let i = 0; i < config.editModeNum; i++) {
        $('#edit-mode').append(`<option class="dropdown-option" value="${i}">${config.getEditModeName(i)}</option>`);
    }
    onEditModeChange();
    updateIdButtons();
});

$('#edit-mode').on('change', () => {
    config.editMode = Number($('#edit-mode option:selected').val());
    onEditModeChange();
});

$('#edit-mode').on('wheel', event => {
    event.preventDefault();
    const currentMode = Number($('#edit-mode option:selected').val());
    let newMode;
    if (event.originalEvent.deltaY > 0) {
        newMode = Math.min(currentMode + 1, 5);
    } else {
        newMode = Math.max(currentMode - 1, 0);
    }
    if (currentMode != newMode) {
        config.editMode = newMode;
        onEditModeChange();
    }
});