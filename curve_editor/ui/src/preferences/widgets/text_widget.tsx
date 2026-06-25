import React from 'react';
import { WidgetProps } from '../schema_form';


const TextWidget: React.FC<WidgetProps> = ({ item, value, onChange }) => (
    <div className='pref-row pref-text'>
        <span className='pref-label'>{item.label}</span>
        <input
            type='text'
            value={value ?? ''}
            onChange={(e) => onChange(e.target.value)}
        />
    </div>
);

export default TextWidget;
