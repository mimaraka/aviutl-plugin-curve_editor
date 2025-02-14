import React from 'react';
import { faClone, faSquareUpRight, faStar, faLock, faLockOpen, faTrash, faEllipsisVertical } from '@fortawesome/free-solid-svg-icons';
import { ToolbarButtonIcon, ToolbarButtonIconProps } from './button';
import { editor } from './host_object';
import './style/toolbar.scss';


interface ToolbarProps {
    curveId: number;
    isUpdateAvailable: boolean;
}

const Toolbar: React.FC<ToolbarProps> = (props: ToolbarProps) => {
    const [isLocked, setIsLocked] = React.useState(editor.isCurveLocked(props.curveId));
    const [showNotification, setShowNotification] = React.useState(props.isUpdateAvailable);

    const onMessageFromHost = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'NotifyUpdateAvailable':
                setShowNotification(true);
                break;
        }
    }

    React.useEffect(() => {
        window.chrome.webview.addEventListener('message', onMessageFromHost);

        return () => {
            window.chrome.webview.removeEventListener('message', onMessageFromHost);
        }
    }, []);

    const buttonsProps: ToolbarButtonIconProps[] = [
        {
            icon: faClone,
            title: 'カーブのコード/IDをコピー',
            onClick: () => {
                window.chrome.webview.postMessage({
                    command: 'ButtonCopy'
                });
            }
        },
        {
            icon: faSquareUpRight,
            title: 'カーブのコードを読み取り',
            onClick: () => {
                window.chrome.webview.postMessage({
                    command: 'ButtonCurveCode'
                });
            },
            disabled: !(editor.getCurveName(props.curveId) == "bezier"
                || editor.getCurveName(props.curveId) == "elastic"
                || editor.getCurveName(props.curveId) == "bounce")
        },
        {
            icon: faStar,
            title: 'プリセットに保存',
            onClick: () => {
                window.chrome.webview.postMessage({
                    command: 'ButtonSave'
                });
            }
        },
        {
            icon: editor.isCurveLocked(props.curveId)? faLock : faLockOpen,
            title: editor.isCurveLocked(props.curveId) ? 'カーブの編集はロックされています' : 'カーブは編集可能です',
            onClick: () => {
                setIsLocked(!editor.isCurveLocked(props.curveId));
                editor.setIsCurveLocked(editor.currentCurveId, !editor.isCurveLocked(props.curveId));
                window.postMessage({
                    command: 'UpdateCurveEditability'
                }, '*');
            }
        },
        {
            icon: faTrash,
            title: 'カーブを削除',
            onClick: () => {
                window.chrome.webview.postMessage({
                    command: 'ButtonClear'
                });
            },
            disabled: editor.isCurveLocked(props.curveId)
        },
        {
            icon: faEllipsisVertical,
            title: 'その他',
            onMouseDown: (event: React.MouseEvent) => {
                if (event.button === 0) {
                    window.chrome.webview.postMessage({
                        command: 'ButtonOthers'
                    });
                }
            },
            badge: showNotification
        }
    ];
    
    return (
        <div className='menu-top'>
            <div className='menu-row'>
                {buttonsProps.map((buttonProps, i) => <ToolbarButtonIcon {...buttonProps} key={i}/>)}
            </div>
        </div>
    );
}

export default Toolbar;