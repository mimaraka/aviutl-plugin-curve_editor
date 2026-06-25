import React from 'react';
import { WidgetProps } from '../schema_form';


const ComboWidget: React.FC<WidgetProps> = ({ item, value, onChange }) => (
    <div className='pref-row pref-combo'>
        <span className='pref-label'>{item.label}</span>
        <select value={value} onChange={(e) => onChange(parseInt(e.target.value))}>
            {(item.options ?? []).map((option, index) => (
                <option key={index} value={index}>{option}</option>
            ))}
        </select>
    </div>
);

export default ComboWidget;
