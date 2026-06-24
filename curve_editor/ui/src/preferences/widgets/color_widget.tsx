import React from 'react';
import { WidgetProps } from '../schema_form';


// value は COLORREF (0x00BBGGRR)。色選択はネイティブの ChooseColor に委譲する
const ColorWidget: React.FC<WidgetProps> = ({ item, value }) => {
    const colorref = value ?? 0;
    const r = colorref & 0xff;
    const g = (colorref >> 8) & 0xff;
    const b = (colorref >> 16) & 0xff;

    const onPick = () => {
        // 選択結果は SetPrefValue メッセージで preferences_dialog 側の state に反映される
        window.chrome.webview.postMessage({
            command: 'PickColor',
            key: item.key,
            value: colorref,
        });
    };

    return (
        <div className='pref-row pref-color'>
            <span className='pref-label'>{item.label}</span>
            <button className='pref-color-button' onClick={onPick}>
                <span className='pref-color-swatch' style={{ backgroundColor: `rgb(${r}, ${g}, ${b})` }} />
            </button>
        </div>
    );
};

export default ColorWidget;
