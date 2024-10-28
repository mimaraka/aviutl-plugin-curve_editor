import React from 'react';
import Preset from './preset';
import Toolbar from './toolbar';
import EditorPanel from './panel_editor';
import { config, graphEditor, scriptEditor } from './host_object';
import './scss/panel_main.scss';


// const disablePanel = () => {
//     $('body').append('<div id="disabled-overlay"></div>');
//     $('#disabled-overlay').on('mousedown', () => {
//         window.chrome.webview.postMessage({
//             to: 'native',
//             command: 'flash'
//         });
//     });
// }

// const enablePanel = () => {
//     $('#disabled-overlay').remove();
// }

const MainPanel: React.FC = () => {
    const [isDraggingSeparator, setIsDraggingSeparator] = React.useState(false);
    const [isDragging, setIsDragging] = React.useState(false);
    const [separatorPos, setSeparatorPos] = React.useState(config.separatorPos);
    const [height, setHeight] = React.useState(document.documentElement.clientHeight);
    const [editMode, setEditMode] = React.useState(config.editMode);
    const [idx, setIdx] = React.useState(0);
    const ref = React.useRef<HTMLDivElement | null>(null);
    const separatorPosRef = React.useRef<number>(separatorPos);

    const toolbarHeight = parseFloat(getComputedStyle(document.documentElement).getPropertyValue('--menu-row-height'));
    const separatorHeight = parseFloat(getComputedStyle(document.documentElement).getPropertyValue('--separator-height'));
    const applyButtonHeight = parseFloat(getComputedStyle(document.documentElement).getPropertyValue('--apply-button-height'));

    const changeEditMode = (editMode: number) => {
        setEditMode(editMode);
        config.editMode = editMode;
    }

    const getIdx = () => {
        switch (editMode) {
            case 0:
                return graphEditor.normal.idx;

            case 1:
                //return graphEditor.value.idx;
                return 0;

            case 5:
                return scriptEditor.idx;
        }
        return 0;
    }

    const changeIdx = (idx: number) => {
        let newIdx = null;
        switch (editMode) {
            case 0:
                if (graphEditor.normal.size <= idx) {
                    graphEditor.normal.appendIdx();
                    newIdx = graphEditor.normal.size - 1;
                } else {
                    newIdx = Math.max(idx, 0);
                }
                graphEditor.normal.idx = newIdx;
                break;

            case 1:
                //graphEditor.value.idx = idx;
                break;

            case 5:
                if (scriptEditor.size <= idx) {
                    scriptEditor.appendIdx();
                    newIdx = scriptEditor.size - 1;
                } else {
                    newIdx = Math.max(idx, 0);
                }
                scriptEditor.idx = newIdx;
                break;
        }
        if (newIdx !== null) {
            setIdx(newIdx);
        }
    }

    const getSize = () => {
        switch (editMode) {
            case 0:
                return graphEditor.normal.size;

            case 1:
                //return graphEditor.value.size;
                return 0;

            case 5:
                return scriptEditor.size;
        }
        return 0;
    }

    const onMessageFromNative = (e: MessageEvent) => {
        switch (e.data.command) {
            case 'drop':
                onDrop();
                break;
        }
    }

    React.useEffect(() => {
        if (isDraggingSeparator) {
        // マウス移動とマウスアップのイベントをwindowに追加
            window.addEventListener('mousemove', handleMouseMove);
            window.addEventListener('mouseup', handleMouseUp);
        } else {
        // ドラッグが終了したらイベントリスナーを削除
            window.removeEventListener('mousemove', handleMouseMove);
            window.removeEventListener('mouseup', handleMouseUp);
        }

        // クリーンアップ: コンポーネントがアンマウントされた時にイベントリスナーを削除
        return () => {
            window.removeEventListener('mousemove', handleMouseMove);
            window.removeEventListener('mouseup', handleMouseUp);
        };
    }, [isDraggingSeparator]);

    React.useEffect(() => {
        window.chrome.webview.addEventListener('message', onMessageFromNative);

        const observer = new ResizeObserver((entries) => {
            entries.forEach((entry) => {
                setHeight(document.documentElement.clientHeight);
            });
        });
        observer.observe(ref.current!);

        return () => {
            window.chrome.webview.removeEventListener('message', onMessageFromNative);
            observer.disconnect();
        };
    }, []);

    const onDrag = () => {
        setIsDragging(true);
        window.chrome.webview.postMessage({
            to: 'native',
            command: 'drag-and-drop'
        });
    }

    const onDrop = () => {
        setIsDragging(false);
    }

    // ドラッグ開始時のイベントハンドラ
    const handleMouseDown = () => {
        setIsDraggingSeparator(true);
    };

    // ドラッグ中のイベントハンドラ
    const handleMouseMove = (e: MouseEvent) => {
        const editorPresetHeight = height - toolbarHeight * 2 - applyButtonHeight - separatorHeight;
        const tmp = (e.clientY - toolbarHeight * 2 - applyButtonHeight - separatorHeight * 0.5) / editorPresetHeight;
        separatorPosRef.current = Math.max(Math.min(tmp, 1), 0);
        setSeparatorPos(separatorPosRef.current);
    };

    // ドラッグ終了時のイベントハンドラ
    const handleMouseUp = () => {
        setIsDraggingSeparator(false);
        config.separatorPos = separatorPosRef.current;
    };

    const editorPresetHeight = height - toolbarHeight * 2 - applyButtonHeight - separatorHeight;
    const pointerEvents = isDraggingSeparator ? 'none' : 'auto';
    const userSelect = isDraggingSeparator ? 'none' : 'auto';

    return (
        <div className='container-panel-main' ref={ref}>
            <Toolbar editMode={editMode}/>
            <EditorPanel
                isSelectDialog={false}
                editMode={editMode}
                setEditMode={changeEditMode}
                idx={getIdx()}
                size={getSize()}
                setIdx={changeIdx}
                style={{
                    height: `${toolbarHeight + editorPresetHeight * separatorPos}px`,
                    pointerEvents: pointerEvents,
                    userSelect: userSelect
                }}
            />
            <button className='button-apply' id='button-apply' onMouseDown={onDrag}>
                {isDragging? 'トラックバーにドラッグ&ドロップして適用' : '適用'}
            </button>
            <div id='separator' onMouseDown={handleMouseDown}>
                <div id='separator-handle'></div>
            </div>
            <Preset style={{
                height: `${editorPresetHeight * (1 - separatorPos)}px`,
                pointerEvents: pointerEvents,
                userSelect: userSelect
            }}/>
        </div>
    );
}

export default MainPanel;