window.addEventListener('message', function(event) {
    if (event.data.to == 'editor-graph') {
        $('#panel-editor')[0].contentWindow.postMessage(event.data, '*');
    }
    this.top.chrome.webview.postMessage(event.data);
});

function updateSelect() {
    $('#edit-mode').val(config.editMode);
}

function updateEditor() {
    if (config.editMode == 5) {
        $('#editor').attr('src', 'editor_text.html');
    } else {
        $('#editor').attr('src', 'editor_graph.html');
    }
}

// IDボタンの状態を更新
function updateIdButtons() {
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

$(document).ready(function() {
    // ドロップダウンメニューの項目を追加
    for (let i = 0; i < config.editModeNum; i++) {
        $('#edit-mode').append(`<option class="dropdown-option" value="${i}">${config.getEditModeName(i)}</option>`);
    }
    updateSelect();
    updateEditor();
    updateIdButtons();
});

$('#edit-mode').on('change', function() {
    config.editMode = Number($('#edit-mode option:selected').val());
    updateEditor();
});

$('#edit-mode').on('wheel', function(event) {
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
        updateSelect();
        updateEditor();
    }
});

$('#button-id').on('click', function() {
    window.top.postMessage({
        command: 'button-id'
    }, '*');
});

$('#button-id-back').on('mousedown', function() {
    editor.advanceIdx(-1);
    updateIdButtons();
    $('#editor')[0].contentWindow.postMessage({
        command: 'changeId'
    }, '*');
});

$('#button-id-forward').on('mousedown', function() {
    editor.advanceIdx(1);
    updateIdButtons();
    $('#editor')[0].contentWindow.postMessage({
        command: 'changeId'
    }, '*');
});