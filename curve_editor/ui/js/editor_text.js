require.config({ paths: { 'vs': 'libs/monaco-editor/min/vs' }});
require(['vs/editor/editor.main'], function() {
    monaco.editor.create(document.getElementById('container'), {
        value: 'return (ed - st) * t + st',
        language: 'lua',
        automaticLayout: true,
        minimap: { enabled: false },
        wordWrap: 'on',
    });
    monaco.editor.setTheme('vs-dark');
});