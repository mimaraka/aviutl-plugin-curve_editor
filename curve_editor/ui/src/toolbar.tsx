import React from 'react';
import { faClone, faSquareUpRight, faStar, faLock, faLockOpen, faTrash, faEllipsisVertical } from '@fortawesome/free-solid-svg-icons';
import { ToolbarButtonIcon, ToolbarButtonIconProps } from './button';
import './scss/toolbar.scss';


interface ToolbarProps {
    editMode: number;
    isUpdateAvailable: boolean;
}

const Toolbar: React.FC<ToolbarProps> = (props: ToolbarProps) => {
    const [isLocked, setIsLocked] = React.useState(false);
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
            disabled: !(props.editMode == 2 || props.editMode == 3 || props.editMode == 4)
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
            icon: isLocked? faLock : faLockOpen,
            title: isLocked ? 'カーブの編集はロックされています' : 'カーブは編集可能です',
            onClick: () => {
                window.chrome.webview.postMessage({
                    command: 'ButtonLock'
                });
                setIsLocked(!isLocked);
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
            disabled: isLocked
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