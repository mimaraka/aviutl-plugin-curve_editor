import React from 'react';
import CurveThumbnail from './curve_thumbnail';
import { editor, preset } from './host_object';
import './scss/preset_item.scss';


interface PresetItemProps {
    width: number;
    curveId: number;
    collectionId: number;
    name: string;
    date: number | null;
}

const PresetItem: React.FC<PresetItemProps> = React.memo((props: PresetItemProps) => {
    const [isDragging, setIsDragging] = React.useState(false);

    const getPresetTitle = () => {
        const name = props.name;
        const typeName = editor.getCurveDispName(props.curveId);
        const collectionName = preset.getCollectionName(props.collectionId);
        let result = `${name}\nタイプ: ${typeName}\nコレクション: ${collectionName}`;
        if (props.date) {
            const date = new Date(props.date * 1000);
            result += `\n作成日時: ${date.toLocaleString()}`;
        }
        return result;
    }

    const onMouseDown = (event: React.MouseEvent) => {
        if (event.button === 0) {
            setIsDragging(true);
        }
        else if (event.button === 2) {
            window.chrome.webview.postMessage({
                command: 'ContextMenuPresetItem',
                curveId: props.curveId
            });
        }
    }

    const onMouseLeave = (event: React.MouseEvent) => {
        if (isDragging) {
            const type = editor.getCurveName(props.curveId);
            if (type === 'bezier' || type === 'elastic' || type === 'bounce') {
                window.chrome.webview.postMessage({
                command: 'OnDndStart',
                curveId: props.curveId
            });
            }
        }
    }

    const onMouseUp = (event: React.MouseEvent) => {
        setIsDragging(false);
    }

    const onDoubleClick = (event: React.MouseEvent) => {
        window.chrome.webview.postMessage({
            command: 'ApplyPreset',
            curveId: props.curveId
        });
    }

    const onMessageFromHost = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'OnDndEnd':
                setIsDragging(false);
                break;
        }
    }

    React.useEffect(() => {
        window.chrome.webview.addEventListener('message', onMessageFromHost);

        return () => {
            window.chrome.webview.removeEventListener('message', onMessageFromHost);
        }
    }, []);

    return (
        <div className='preset-item' style={{ width: props.width, height: props.width + 16 /*<- TODO: 直す*/ }}>
            <div
                className='preset-item-thumbnail'
                title={getPresetTitle()}
                onMouseDown={onMouseDown}
                onMouseLeave={onMouseLeave}
                onMouseUp={onMouseUp}
                onDoubleClick={onDoubleClick}
            >
                <CurveThumbnail curveId={props.curveId} width={props.width - 4}/>
            </div>
            <div className='preset-item-text'>
                {props.name}
            </div>
        </div>
    );
});

export default PresetItem;