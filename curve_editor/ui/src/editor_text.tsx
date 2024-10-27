import React from 'react';
import * as monaco from 'monaco-editor';
import { config, scriptEditor } from './host_object';
import './scss/editor_text.scss';


interface TextEditorPanelProps {
    idx: number;
}

const TextEditorPanel: React.FC<TextEditorPanelProps> = (props: TextEditorPanelProps) => {
    const editorRef = React.useRef<monaco.editor.IStandaloneCodeEditor | null>(null);
    const idxRef = React.useRef(props.idx);

    const getIdx = () => {
        return props.idx;
    }

    const updateEditor = () => {
        editorRef.current?.setValue(scriptEditor.getScript(scriptEditor.getId(props.idx)));
        console.log(props.idx, scriptEditor.getScript(scriptEditor.getId(props.idx)));
    }

    const applyPreferences = () => {
        editorRef.current?.updateOptions({ wordWrap: config.wordWrap ? 'on' : 'off' });
        if (window.matchMedia('(prefers-color-scheme: dark)').matches) {
            monaco.editor.setTheme('vs-dark');
        } else {
            monaco.editor.setTheme('vs-light');
        }
    }

    const onMessageFromNative = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'updateEditor':
                updateEditor();
                break;

            case 'applyPreferences':
                applyPreferences();
                break;
        }
    }

    if (idxRef.current != props.idx) {
        idxRef.current = props.idx;
        updateEditor();
    }

    React.useEffect(() => {
        const container = document.getElementById('container');
        if (container) {
            editorRef.current = monaco.editor.create(container, {
                language: 'lua',
                automaticLayout: true,
                minimap: { enabled: false },
            });
            applyPreferences();
            updateEditor();
        }

        // window.editor.addAction({
        //     id: 'show-graph',
        //     label: 'グラフエディタを表示',
        //     contextMenuGroupId: 'navigation',
        //     contextMenuOrder: 1,
        //     run: function(ed) {
        //         console.log(ed);
        //     }
        // });

        editorRef.current?.getModel()?.onDidChangeContent(event => {
            scriptEditor.setScript(scriptEditor.getId(getIdx()), editorRef.current?.getValue() ?? '');
            console.log(getIdx(), scriptEditor.getId(getIdx()), editorRef.current?.getValue() ?? '', scriptEditor.getScript(scriptEditor.getId(props.idx)));
        });

        window.chrome.webview.addEventListener('message', onMessageFromNative);

        return () => {
            editorRef.current?.dispose();
            window.chrome.webview.removeEventListener('message', onMessageFromNative);
        };
    }, []);

    return (
        <div id='container'></div>
    );
}

export default TextEditorPanel;