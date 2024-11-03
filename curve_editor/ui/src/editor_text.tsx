import React from 'react';
import * as monaco from 'monaco-editor';
import { config, editor } from './host_object';
import './scss/editor_text.scss';


interface TextEditorPanelProps {
    idx: number;
}

const TextEditorPanel: React.FC<TextEditorPanelProps> = (props: TextEditorPanelProps) => {
    const editorRef = React.useRef<monaco.editor.IStandaloneCodeEditor | null>(null);
    const idxRef = React.useRef(props.idx);

    const updateEditor = () => {
        editorRef.current?.setValue(editor.script.getScript(editor.script.getId(props.idx)));
    }

    const applyPreferences = () => {
        editorRef.current?.updateOptions({ wordWrap: config.wordWrap ? 'on' : 'off' });
        if (window.matchMedia('(prefers-color-scheme: dark)').matches) {
            monaco.editor.setTheme('vs-dark');
        } else {
            monaco.editor.setTheme('vs-light');
        }
    }

    const onMessageFromHost = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'UpdateEditor':
                updateEditor();
                break;

            case 'ApplyPreferences':
                applyPreferences();
                break;
        }
    }

    React.useEffect(() => {
        if (idxRef.current != props.idx) {
            idxRef.current = props.idx;
            updateEditor();
        }
    }, [props.idx]);

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
            editor.script.setScript(editor.script.getId(idxRef.current), editorRef.current?.getValue() ?? '');
        });

        window.chrome.webview.addEventListener('message', onMessageFromHost);

        return () => {
            editorRef.current?.dispose();
            window.chrome.webview.removeEventListener('message', onMessageFromHost);
        };
    }, []);

    return (
        <div id='container'></div>
    );
}

export default TextEditorPanel;