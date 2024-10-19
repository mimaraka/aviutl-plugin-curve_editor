'use strict';

require.config({ paths: { 'vs': 'libs/monaco-editor/min/vs' }});
require(['vs/editor/editor.main'], function() {
    window.editor = monaco.editor.create(document.getElementById('container'), {
        value: scriptEditor.script,
        language: 'lua',
        automaticLayout: true,
        minimap: { enabled: false },
        wordWrap: config.wordWrap ? 'on' : 'off',
    });
    monaco.editor.setTheme('vs-dark');
});

// window.editor.addAction({
//     id: 'show-graph',
//     label: 'グラフエディタを表示',
//     contextMenuGroupId: 'navigation',
//     contextMenuOrder: 1,
//     run: function(ed) {
//         console.log(ed);
//     }
// });

window.editor.getModel().onDidChangeContent(event => {
    scriptEditor.script = window.editor.getValue();
});

window.addEventListener('message', event => {
    switch (event.data.command) {
        case 'updateEditor':
        case 'changeCurve':
            window.editor.setValue(scriptEditor.script);
            break;
    }
});