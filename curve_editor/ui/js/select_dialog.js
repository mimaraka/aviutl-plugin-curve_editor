'use strict';

window.isSelectDialog = true;
window.editMode = 0;
window.idxNormal = 0;
window.idxScript = 0;

const handleMessage = event => {
    switch (event.data.to) {
        case 'native':
            window.chrome.webview.postMessage(event.data);
            break;

        case 'panel-editor':
        case 'editor-graph':
            $('#panel-editor')[0].contentWindow.postMessage(event.data, '*');
            break;
    }
}

window.addEventListener('message', handleMessage);
window.chrome.webview.addEventListener('message', handleMessage);

$('#button-cancel').on('click', () => {
    window.top.chrome.webview.postMessage({
        to: 'native',
        command: 'selectdlg-cancel'
    });
});

$('#button-ok').on('click', () => {
    let idOrIdx = 0;
    switch (window.editMode) {
        case 0:
            idOrIdx = window.top.idxNormal;
            break;

        case 1:
            //idOrIdx = window.top.idxValue;
            break;

        case 2:
            idOrIdx = graphEditor.bezier.getId(true);
            break;

        case 3:
            idOrIdx = graphEditor.elastic.getId(true);
            break;

        case 4:
            idOrIdx = graphEditor.bounce.getId(true);
            break;

        case 5:
            idOrIdx = window.top.idxScript;
            break
    }
    window.top.chrome.webview.postMessage({
        to: 'native',
        command: 'selectdlg-ok',
        mode: window.editMode,
        idOrIdx: idOrIdx
    });
});