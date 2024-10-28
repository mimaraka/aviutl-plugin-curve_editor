import React from 'react';
import { faClone, faSquareUpRight, faStar, faLock, faLockOpen, faTrash, faEllipsisVertical } from '@fortawesome/free-solid-svg-icons';
import { ToolbarButtonIcon, ToolbarButtonIconProps } from './button';
import { config } from './host_object';
import './scss/toolbar.scss';


interface ToolbarProps {
    editMode: number;
}

const Toolbar: React.FC<ToolbarProps> = (props: ToolbarProps) => {
    const [isLocked, setIsLocked] = React.useState(false);
    const [showNotification, setShowNotification] = React.useState(false);

    React.useEffect(() => {
        if (config.notifyUpdate) {
            fetch('https://api.github.com/repos/mimaraka/aviutl-plugin-curve_editor/releases/latest').then(response => {
                return response.json();
            }).then(data => {
                if (!config.isLatestVersion(data.tag_name)) {
                    setShowNotification(true);
                }
            });
        }
    }, []);

    const buttonsProps: ToolbarButtonIconProps[] = [
        {
            icon: faClone,
            title: 'カーブのコード/IDをコピー',
            onClick: () => {
                window.chrome.webview.postMessage({
                    to: 'native',
                    command: 'copy'
                });
            }
        },
        {
            icon: faSquareUpRight,
            title: 'カーブのコードを読み取り',
            onClick: () => {
                window.chrome.webview.postMessage({
                    command: 'read'
                });
            },
            disabled: !(props.editMode == 2 || props.editMode == 3 || props.editMode == 4)
        },
        {
            icon: faStar,
            title: 'プリセットに保存',
            onClick: () => {
                window.chrome.webview.postMessage({
                    command: 'save'
                });
            }
        },
        {
            icon: isLocked? faLock : faLockOpen,
            title: isLocked ? 'カーブの編集はロックされています' : 'カーブは編集可能です',
            onClick: () => {
                window.chrome.webview.postMessage({
                    command: 'lock'
                });
                setIsLocked(!isLocked);
            }
        },
        {
            icon: faTrash,
            title: 'カーブを削除',
            onClick: () => {
                window.chrome.webview.postMessage({
                    command: 'clear'
                });
            },
            disabled: isLocked
        },
        {
            icon: faEllipsisVertical,
            title: 'その他',
            onClick: () => {
                window.chrome.webview.postMessage({
                    command: 'others'
                });
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