import React from 'react';
import { WidgetProps } from '../schema_form';


const FilePathWidget: React.FC<WidgetProps> = ({ item, value, onChange }) => {
    const onBrowse = () => {
        // 選択結果は SetPrefValue メッセージで preferences_dialog 側の state に反映される
        window.chrome.webview.postMessage({
            command: 'PickFilePath',
            key: item.key,
        });
    };

    return (
        <div className='pref-row pref-filepath'>
            <span className='pref-label'>{item.label}</span>
            <div className='pref-filepath-control'>
                <input
                    type='text'
                    value={value ?? ''}
                    onChange={(e) => onChange(e.target.value)}
                />
                <button className='pref-button' onClick={onBrowse}>{window.stringTable['WordSelect']}</button>
            </div>
        </div>
    );
};

export default FilePathWidget;
