import React from 'react';
import PresetPanel from './preset';
import Toolbar from './toolbar';
import EditorPanel from './panel_editor';
import { config, editor } from './interface';
import './style/panel_main.scss';


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

interface MainPanelProps {
    isUpdateAvailable: boolean;
}

const MainPanel: React.FC<MainPanelProps> = (props: MainPanelProps) => {
    const [isSeparatorDragging, setIsDraggingSeparator] = React.useState(false);
    const [isDragging, setIsDragging] = React.useState(false);
    const [separatorPos, setSeparatorPos] = React.useState(config.separatorPos);
    const [width, setWidth] = React.useState(document.documentElement.clientWidth);
    const [height, setHeight] = React.useState(document.documentElement.clientHeight);
    const [editMode, setEditMode] = React.useState(config.editMode);
    const [layoutMode, setLayoutMode] = React.useState(config.layoutMode);
    // TODO: 8がマジックナンバー
    const [applyButtonHeight, setApplyButtonHeight] = React.useState(config.applyButtonHeight - 8);
    const [idx, setIdx] = React.useState(0);
    const ref = React.useRef<HTMLDivElement | null>(null);
    const [applyModeOptions, setApplyModeOptions] = React.useState([] as React.JSX.Element[]);
    const separatorPosRef = React.useRef<number>(separatorPos);
    const toolbarHeight = parseFloat(getComputedStyle(document.documentElement).getPropertyValue('--menu-row-height'));
    const separatorThickness = parseFloat(getComputedStyle(document.documentElement).getPropertyValue('--separator-height'));

    const changeEditMode = (editMode: number) => {
        setEditMode(editMode);
        config.editMode = editMode;
    }

    const createApplyModeOptions = () => {
        return [...Array(config.applyModeNum)].map((_, i) => {
            return <option className='dropdown-option' value={i} key={i}>{`${i == config.applyMode ? '☑ ' : ''}${config.getApplyModeName(i)}`}</option>;
        });
    }

    const getIdx = () => {
        switch (config.editMode) {
            case 0:
                return editor.graph.normal.idx;

            case 1:
                //return editor.graph.value.idx;
                return 0;

            case 5:
                return editor.script.idx;
        }
        return 0;
    }

    const changeIdx = (idx: number) => {
        let newIdx = null;
        switch (config.editMode) {
            case 0:
                if (editor.graph.normal.size <= idx) {
                    editor.graph.normal.appendIdx();
                    newIdx = editor.graph.normal.size - 1;
                } else {
                    newIdx = Math.max(idx, 0);
                }
                editor.graph.normal.idx = newIdx;
                break;

            case 1:
                //editor.graph.value.idx = idx;
                break;

            case 5:
                if (editor.script.size <= idx) {
                    editor.script.appendIdx();
                    newIdx = editor.script.size - 1;
                } else {
                    newIdx = Math.max(idx, 0);
                }
                editor.script.idx = newIdx;
                break;
        }
        if (newIdx !== null) {
            setIdx(newIdx);
        }
    }

    const getSize = () => {
        switch (config.editMode) {
            case 0:
                return editor.graph.normal.size;

            case 1:
                //return editor.graph.value.size;
                return 0;

            case 5:
                return editor.script.size;
        }
        return 0;
    }

    const onMessage = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'ChangeIdx':
                changeIdx(event.data.idx);
                break;
        }
    }

    const onMessageFromHost = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'OnDndEnd':
                onDrop();
                break;

            case 'ChangeLayoutMode':
                setLayoutMode(config.layoutMode);
                break;

            case 'ApplyPreferences':
                // TODO: 8がマジックナンバー
                setApplyButtonHeight(config.applyButtonHeight - 8);

            case 'ChangeEditMode':
                setEditMode(config.editMode);
                break;

            case 'UpdateEditor':
                setEditMode(config.editMode);
                changeIdx(getIdx());
                break;
        }
    }

    React.useEffect(() => {
        if (isSeparatorDragging) {
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
    }, [isSeparatorDragging]);

    React.useEffect(() => {
        setApplyModeOptions(createApplyModeOptions());
    }, [config.applyMode]);

    React.useEffect(() => {
        window.addEventListener('message', onMessage);
        window.chrome.webview.addEventListener('message', onMessageFromHost);

        const observer = new ResizeObserver((entries) => {
            entries.forEach((entry) => {
                setWidth(document.documentElement.clientWidth);
                setHeight(document.documentElement.clientHeight);
            });
        });
        observer.observe(ref.current!);

        return () => {
            window.removeEventListener('message', onMessage);
            window.chrome.webview.removeEventListener('message', onMessageFromHost);
            observer.disconnect();
        };
    }, []);

    const onApplyButtonMouseDown = (event: React.MouseEvent) => {
        if (event.button === 0) {
            setIsDragging(true);
            window.chrome.webview.postMessage({
                command: 'OnDndStart',
                curveId: editor.currentCurveId
            });
        }
    }

    const onApplyButtonKeyDown = (event: React.KeyboardEvent) => {
        if (event.code === 'Space') {
            event.preventDefault();
        }
    }

    const onDrop = () => {
        setIsDragging(false);
    }

    // ドラッグ開始時のイベントハンドラ
    const handleMouseDown = (event: React.MouseEvent) => {
        setIsDraggingSeparator(true);
        event.preventDefault();
        event.stopPropagation();
    };

    // ドラッグ中のイベントハンドラ
    const handleMouseMove = (e: MouseEvent) => {
        if (config.layoutMode == 1) {
            separatorPosRef.current = Math.max(Math.min((e.clientX - separatorThickness * 0.5) / (width - separatorThickness), 1), 0);
        } else {
            const editorPresetHeight = height - toolbarHeight * 2 - applyButtonHeight - separatorThickness;
            const tmp = (e.clientY - toolbarHeight * 2 - applyButtonHeight - separatorThickness * 0.5) / editorPresetHeight;
            separatorPosRef.current = Math.max(Math.min(tmp, 1), 0);
        }
        setSeparatorPos(separatorPosRef.current);
    };

    // ドラッグ終了時のイベントハンドラ
    const handleMouseUp = () => {
        setIsDraggingSeparator(false);
        config.separatorPos = separatorPosRef.current;
    };

    const onSelectChange = (event: React.ChangeEvent<HTMLSelectElement>) => {
        config.applyMode = parseInt(event.target.value);
        setApplyModeOptions(createApplyModeOptions());
    }

    const editorPresetHeight = height - toolbarHeight * 2 - applyButtonHeight - separatorThickness;
    const pointerEvents = isSeparatorDragging ? 'none' : 'auto';
    const userSelect = isSeparatorDragging ? 'none' : 'auto';

    return (
        <div className='container-panel-main' ref={ref} style={{ flexDirection: layoutMode ? 'row' : 'column' }}>
            <div
                className='panel-upside'
                style={{
                    width: layoutMode ? (width - separatorThickness) * separatorPos : '100%',
                    height: layoutMode ? '100%' : `${toolbarHeight * 2 + editorPresetHeight * separatorPos + applyButtonHeight}px`
                }}
            >
                <Toolbar curveId={editor.currentCurveId} isUpdateAvailable={props.isUpdateAvailable}/>
                <EditorPanel
                    isSelectDialog={false}
                    editMode={editMode}
                    setEditMode={changeEditMode}
                    idx={getIdx()}
                    size={getSize()}
                    setIdx={changeIdx}
                    // TODO: 28がマジックナンバー
                    style={{
                        height: layoutMode ? `calc(100% - ${applyButtonHeight + 36}px)` : `${toolbarHeight + editorPresetHeight * separatorPos}px`,
                        pointerEvents: pointerEvents,
                        userSelect: userSelect
                    }}
                />
                <div className='dropdown-container' id='select-apply-mode' style={{height: applyButtonHeight}}>
                    <select className='dropdown' name='apply-mode' id='apply-mode' value={config.applyMode} onChange={onSelectChange}>
                        {applyModeOptions}
                    </select>
                        <button className='button-apply' id='button-apply' title={`${window.stringTable['TooltipButtonApply']} (${config.getApplyModeName(config.applyMode)})`} onMouseDown={onApplyButtonMouseDown} onKeyDown={onApplyButtonKeyDown}>
                        {isDragging? window.stringTable['LabelApplyDnD'] : window.stringTable['WordApply']}
                    </button>
                </div>
            </div>
            <div
                id='separator'
                className={layoutMode ? 'separator-horizontal' : 'separator-vertical'}
                onMouseDown={handleMouseDown}
                style={{ cursor: layoutMode ? 'col-resize' : 'row-resize'}}
            >
                <div
                    id='separator-handle'
                    className={layoutMode ? 'separator-handle-horizontal' : 'separator-handle-vertical'}
                    style={isSeparatorDragging ? { opacity: 1} : {}}
                >
                </div>
            </div>
            <PresetPanel style={{
                width: layoutMode ? (width - separatorThickness) * (1 - separatorPos) : '100%',
                height: layoutMode ? '100%' : `${editorPresetHeight * (1 - separatorPos)}px`,
                pointerEvents: pointerEvents,
                userSelect: userSelect
            }}/>
        </div>
    );
}

export default MainPanel;