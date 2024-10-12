require.config({ paths: { 'vs': 'libs/monaco-editor/min/vs' }});
require(['vs/editor/editor.main'], function() {
    window.editor = monaco.editor.create(document.getElementById('container'), {
        value: scriptEditor.script,
        language: 'lua',
        automaticLayout: true,
        minimap: { enabled: false },
        wordWrap: 'on',
    });
    monaco.editor.setTheme('vs-dark');
});

window.editor.getModel().onDidChangeContent(event => {
    scriptEditor.script = window.editor.getValue();
});

window.addEventListener('message', event => {
    switch (event.data.command) {
        case 'updateEditor':
        case 'changeId':
        window.editor.setValue(scriptEditor.script);
            break;
    }
});