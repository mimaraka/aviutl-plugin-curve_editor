import React from 'react';
import { WidgetProps } from '../schema_form';


const SliderWidget: React.FC<WidgetProps> = ({ item, value, onChange }) => {
    const min = item.min ?? 0;
    const max = item.max ?? 100;
    const step = item.step ?? 1;
    // 小数ステップの場合は小数として扱い、表示も小数2桁にそろえる
    const isFloat = step < 1;
    const display = isFloat ? Number(value).toFixed(2) : String(value);

    return (
        <div className='pref-row pref-slider'>
            <span className='pref-label'>{item.label}</span>
            <div className='pref-slider-control'>
                <input
                    type='range'
                    min={min}
                    max={max}
                    step={step}
                    value={value}
                    onChange={(e) => onChange(isFloat ? parseFloat(e.target.value) : parseInt(e.target.value))}
                />
                <span className='pref-slider-value'>{display}</span>
            </div>
        </div>
    );
};

export default SliderWidget;
