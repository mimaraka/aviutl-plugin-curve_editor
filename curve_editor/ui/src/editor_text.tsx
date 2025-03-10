import React from 'react';
import * as monaco from 'monaco-editor';
import { config, editor } from './interface';
import './style/editor_text.scss';


let initialized = false;
const initMonacoEditor = () => {
    if (initialized) {
        return;
    }
    initialized = true;
    const identifiers = [
        {name: 'function', insertText: 'function ${1:name}(${2:args})\n\t${0}\nend', type: 'keyword'},
        {name: 'local', insertText: 'local ${1:name} = ${0}', type: 'keyword'},
        {name: 'end', type: 'keyword'},
        {name: 'if', insertText: 'if ${1:condition} then\n\t${0}\nend', type: 'keyword'},
        {name: 'then', type: 'keyword'},
        {name: 'else', insertText: 'else\n\t${0}', type: 'keyword'},
        {name: 'elseif', insertText: 'elseif ${1:condition} then\n\t${0}', type: 'keyword'},
        {name: 'for', insertText: 'for ${1:i} = ${2:1}, ${3:10} do\n\t${0}\nend', type: 'keyword'},
        {name: 'while', insertText: 'while ${1:condition} do\n\t${0}\nend', type: 'keyword'},
        {name: 'do', type: 'keyword'},
        {name: 'goto', type: 'keyword'},
        {name: 'return', type: 'keyword'},
        {name: 'repeat', type: 'keyword'},
        {name: 'until', type: 'keyword'},
        {name: 'break', type: 'keyword'},
        {name: 'in', type: 'keyword'},
        {name: 'nil', type: 'keyword'},
        {name: 'true', type: 'keyword'},
        {name: 'false', type: 'keyword'},
        {name: 'math', type: 'module'},
        {name: 't', type: 'variable'},
        {name: 'st', type: 'variable'},
        {name: 'ed', type: 'variable'},
        {name: 'noise', insertText: 'noise(${1:amp}, ${2:freq})', type: 'function'},
    ]
    const completionItemKindMap: {[key: string]: monaco.languages.CompletionItemKind} = {
        'keyword': monaco.languages.CompletionItemKind.Keyword,
        'module': monaco.languages.CompletionItemKind.Module,
        'variable': monaco.languages.CompletionItemKind.Variable,
        'function': monaco.languages.CompletionItemKind.Function,
    }

    monaco.languages.registerCompletionItemProvider('lua', {
        provideCompletionItems: (model, position) => {
            const word = model.getWordUntilPosition(position);
            const range = new monaco.Range(
            position.lineNumber,
            word.startColumn,
            position.lineNumber,
            word.endColumn
            );
            monaco.languages.CompletionItemKind.Module
            const suggestions: monaco.languages.CompletionItem[] = identifiers.map((identifier) => {
                return {
                    label: identifier.name,
                    kind: completionItemKindMap[identifier.type] ?? monaco.languages.CompletionItemKind.Text,
                    insertText: identifier.insertText ?? identifier.name,
                    insertTextRules: monaco.languages.CompletionItemInsertTextRule.InsertAsSnippet,
                    range: range,
                }
            });

            return { suggestions };
        },
    });
    monaco.languages.setMonarchTokensProvider('lua', {
        defaultToken: '',
        tokenPostfix: '.lua',
        keywords: identifiers.filter((identifier) => identifier.type === 'keyword').map((identifier) => identifier.name),
        functions: identifiers.filter((identifier) => identifier.type === 'function').map((identifier) => identifier.name),
        variables: identifiers.filter((identifier) => identifier.type === 'variable').map((identifier) => identifier.name),
        modules: identifiers.filter((identifier) => identifier.type === 'module').map((identifier) => identifier.name),
        brackets: [{
            token: 'delimiter.bracket',
            open: '{',
            close: '}'
        }, {
            token: 'delimiter.array',
            open: '[',
            close: ']'
        }, {
            token: 'delimiter.parenthesis',
            open: '(',
            close: ')'
        }],
        operators: ['+', '-', '*', '/', '%', '^', '#', '==', '~=', '<=', '>=', '<', '>', '=', ';', ':', ',', '.', '..', '...'],
        symbols: /[=><!~?:&|+\-*\/\^%]+/,
        escapes: /\\(?:[abfnrtv\\"']|x[0-9A-Fa-f]{1,4}|u[0-9A-Fa-f]{4}|U[0-9A-Fa-f]{8})/,
        tokenizer: {
            root: [
                [/[a-zA-Z_]\w*/, {
                    cases: {
                        '@keywords': {
                            token: 'keyword.$0'
                        },
                        '@functions': {
                            token: 'function'
                        },
                        '@variables': {
                            token: 'variable'
                        },
                        '@modules': {
                            token: 'module'
                        },
                        '@default': 'identifier'
                    }
                }], {
                    include: '@whitespace'
                },
                [/(,)(\s*)([a-zA-Z_]\w*)(\s*)(:)(?!:)/, ['delimiter', '', 'key', '', 'delimiter']],
                [/({)(\s*)([a-zA-Z_]\w*)(\s*)(:)(?!:)/, ['@brackets', '', 'key', '', 'delimiter']],
                [/[{}()\[\]]/, '@brackets'],
                [/@symbols/, {
                    cases: {
                        '@operators': 'delimiter',
                        '@default': ''
                    }
                }],
                [/\d*\.\d+([eE][\-+]?\d+)?/, 'number.float'],
                [/0[xX][0-9a-fA-F_]*[0-9a-fA-F]/, 'number.hex'],
                [/\d+?/, 'number'],
                [/[;,.]/, 'delimiter'],
                [/"([^"\\]|\\.)*$/, 'string.invalid'],
                [/'([^'\\]|\\.)*$/, 'string.invalid'],
                [/"/, 'string', '@string."'],
                [/'/, 'string', "@string.'"]
            ],
            whitespace: [
                [/[ \t\r\n]+/, ''],
                [/--\[([=]*)\[/, 'comment', '@comment.$1'],
                [/--.*$/, 'comment']
            ],
            comment: [
                [/[^\]]+/, 'comment'],
                [/\]([=]*)\]/, {
                    cases: {
                        '$1==$S2': {
                            token: 'comment',
                            next: '@pop'
                        },
                        '@default': 'comment'
                    }
                }],
                [/./, 'comment']
            ],
            string: [
                [/[^\\"']+/, 'string'],
                [/@escapes/, 'string.escape'],
                [/\\./, 'string.escape.invalid'],
                [/["']/, {
                    cases: {
                        '$#==$S2': {
                            token: 'string',
                            next: '@pop'
                        },
                        '@default': 'string'
                    }
                }]
            ]
        }
    });
    monaco.editor.defineTheme('customThemeDark', {
        base: 'vs-dark',
        inherit: true,
        rules: [
            { token: 'module', foreground: '4EC9B0' }, // 緑
            { token: 'function', foreground: 'DCDCAA' }, // 黄色
            { token: 'variable', foreground: '9CDCFE' }, // 水色
        ],
        colors: {
            'editor.background': '#1E1E1E', // 背景色
        },
    });
    monaco.editor.defineTheme('customThemeLight', {
        base: 'vs',
        inherit: true,
        rules: [
            { token: 'function', foreground: '000000' }, // 黒
            { token: 'variable', foreground: '0000FF' }, // 青
        ],
        colors: {
            'editor.background': '#FFFFFF', // 背景色
        },
    });
}


interface TextEditorPanelProps {
    idx: number;
}

const TextEditorPanel: React.FC<TextEditorPanelProps> = (props: TextEditorPanelProps) => {
    const editorRef = React.useRef<monaco.editor.IStandaloneCodeEditor | null>(null);
    const idxRef = React.useRef(props.idx);

    const updateEditor = () => {
        editorRef.current?.setValue(editor.script.getScript(editor.script.getId(props.idx)));
    }

    const UpdateEditability = () => {
        editorRef.current?.updateOptions({ readOnly: editor.isCurveLocked(editor.script.getId(props.idx)) });
    }

    const applyPreferences = () => {
        editorRef.current?.updateOptions({ wordWrap: config.wordWrap ? 'on' : 'off' });
        if (window.matchMedia('(prefers-color-scheme: dark)').matches) {
            monaco.editor.setTheme('customThemeDark');
            console.log('dark');
        } else {
            monaco.editor.setTheme('customThemeLight');
            console.log('light');
        }
    }

    const onMessage = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'UpdateCurveEditability':
                UpdateEditability();
                break;
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
            UpdateEditability();
        }
    }, [props.idx]);

    React.useEffect(() => {
        initMonacoEditor();

        const container = document.getElementById('container');
        if (container) {
            editorRef.current = monaco.editor.create(container, {
                language: 'lua',
                automaticLayout: true,
                minimap: { enabled: false },
            });
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

        applyPreferences();
        updateEditor();

        editorRef.current?.getModel()?.onDidChangeContent(event => {
            editor.script.setScript(editor.script.getId(idxRef.current), editorRef.current?.getValue() ?? '');
        });

        window.addEventListener('message', onMessage);
        window.chrome.webview.addEventListener('message', onMessageFromHost);

        return () => {
            editorRef.current?.dispose();
            window.removeEventListener('message', onMessage);
            window.chrome.webview.removeEventListener('message', onMessageFromHost);
        };
    }, []);

    return (
        <div id='container'></div>
    );
}

export default TextEditorPanel;