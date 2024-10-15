$(window).on('message', event => {
    const data = event.originalEvent.data;
    if (data.to == 'panel-editor') {
        switch (data.command) {
            case 'changeCurve':
                updateIdButtons();
                break;

            case 'updateParam':
                updateParamButton();
                break;

            case 'updateEditor':
                $('#editor')[0].contentWindow.postMessage(data, '*');
                updateIdButtons();
                updateParamButton();
                break;

            case 'goBackIdx':
                goBackIdx();
                break;

            case 'goForwardIdx':
                goForwardIdx();
                break;

            case 'setCurve':
                if (data.mode != undefined) {
                    setEditMode(data.mode);
                    onEditModeChange();
                }
                if (data.param != undefined) {
                    if (isEditModeNumeric()) {
                        $('#editor')[0].contentWindow.postMessage({
                            command: 'decode',
                            code: data.param
                        }, '*');
                    } else {
                        setIdx(data.param - 1);
                        updateIdButtons();
                    }
                }
                break;
        }
    }
    else if (data.to == 'editor-graph') {
        $('#editor')[0].contentWindow.postMessage(data, '*');
    }
    else {
        window.top.chrome.webview.postMessage(data);
    }
});

const goBackIdx = () => {
    advanceIdx(-1);
    updateIdButtons();
    $('#editor')[0].contentWindow.postMessage({
        command: 'changeCurve'
    }, '*');
}

const goForwardIdx = () => {
    advanceIdx(1);
    updateIdButtons();
    $('#editor')[0].contentWindow.postMessage({
        command: 'changeCurve'
    }, '*');
}

// IDボタン群を作成
const createIdButtons = () => {
    $('#buttons-param-id').append('<button class="menu-button" id="button-id-back" style="width: 25%;" title="前のIDに戻る"><i class="fa-solid fa-angle-left fa-sm"></i></button>');
    $('#buttons-param-id').append('<button class="menu-button" id="button-id" style="width: 50%;" title="編集中のID"><p class="content"></p></button>');
    $('#buttons-param-id').append('<button class="menu-button" id="button-id-forward" style="width: 25%;"><i class="fa-solid fa-angle-right fa-sm"></i></button>');

    $('#button-id').on('click', () => {
        window.top.postMessage({
            to: 'native',
            command: 'button-id'
        }, '*');
    });

    let timeOutIdBack = 0;
    let intervalIdBack = 0;
    let timeOutIdForward = 0;
    let intervalIdForward = 0;
    const holdTime = 500;
    const intervalTime = 60;
    $('#button-id-back').on('mousedown', event => {
        if (event.button == 0) {
            timeOutIdBack = setTimeout(() => {
                intervalIdBack = setInterval(() => {
                    goBackIdx();
                    if (isIdxFirst()) {
                        clearInterval(intervalIdBack);
                    }
                }, intervalTime);
            }, holdTime);
            goBackIdx();
        }
    }).on('mouseup mouseleave', () => {
        clearTimeout(timeOutIdBack);
        clearInterval(intervalIdBack);
    });

    $('#button-id-forward').on('mousedown', event => {
        if (event.button == 0) {
            timeOutIdForward = setTimeout(() => {
                intervalIdForward = setInterval(() => {
                    goForwardIdx();
                    if (isIdxLast()) {
                        clearInterval(intervalIdForward);
                    }
                }, intervalTime);
            }, holdTime);
            goForwardIdx();
        }
    }).on('mouseup mouseleave', () => {
        clearTimeout(timeOutIdForward);
        clearInterval(intervalIdForward);
    });


}

// IDボタン群の状態を更新
const updateIdButtons = () => {
    $('#button-id-back').prop('disabled', isIdxFirst());
    if (isIdxLast()) {
        $('#button-id-forward i').addClass('fa-plus').removeClass('fa-angle-right');
        $('#button-id-forward').prop('title', 'カーブを新規作成');
    }
    else {
        $('#button-id-forward i').addClass('fa-angle-right').removeClass('fa-plus');
        $('#button-id-forward').prop('title', '次のIDへ進む');
    }
    $('#button-id p').html(getIdx() + 1);
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
    let param;
    switch (getEditMode()) {
        case 2:
            param = graphEditor.bezier.getParam(graphEditor.bezier.getId(window.top.isSelectDialog));
            break;

        case 3:
            param = graphEditor.elastic.getParam(graphEditor.elastic.getId(window.top.isSelectDialog));
            break;

        case 4:
            param = graphEditor.bounce.getParam(graphEditor.bounce.getId(window.top.isSelectDialog));
            break;
    }
    if (param != undefined) {
        $('#button-param').html(param);
    }
}

const onEditModeChange = () => {
    $('#edit-mode').val(getEditMode());
    $('#buttons-param-id').empty();
    if (isEditModeId()) {
        createIdButtons();
        updateIdButtons();
    } else {
        createParamButton();
        updateParamButton();
    }
    if (getEditMode() == 5) {
        $('#editor').attr('src', 'editor_text.html');
    } else if ($('#editor').attr('src') != 'editor_graph.html') {
        $('#editor').attr('src', 'editor_graph.html');
    }
    $('#editor')[0].contentWindow.postMessage({
        command: 'changeCurve'
    }, '*');
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
    // TODO: 値指定モードを実装したら削除
    $('#edit-mode option[value="1"]').attr('disabled', true);
    onEditModeChange();
    updateIdButtons();
});

$('#edit-mode').on('change', () => {
    setEditMode(Number($('#edit-mode option:selected').val()));
    onEditModeChange();
});

$('#edit-mode').on('wheel', event => {
    event.preventDefault();
    const currentMode = Number($('#edit-mode option:selected').val());
    let newMode;
    if (event.originalEvent.deltaY > 0) {
        // TODO: 値指定モードを実装したらelse以外を削除
        if (currentMode == 0) {
            newMode = 2;
        } else {
            newMode = Math.min(currentMode + 1, 5);
        }
    } else {
        // TODO: 値指定モードを実装したらelse以外を削除
        if (currentMode == 2) {
            newMode = 0;
        } else {
            newMode = Math.max(currentMode - 1, 0);
        }
    }
    if (currentMode != newMode) {
        setEditMode(newMode);
        onEditModeChange();
    }
});