import React from 'react';
import { WidgetProps } from '../schema_form';


const CheckboxWidget: React.FC<WidgetProps> = ({ item, value, onChange }) => (
    <label className='pref-row pref-checkbox'>
        <input
            type='checkbox'
            checked={!!value}
            onChange={(e) => onChange(e.target.checked)}
        />
        <span className='pref-label'>{item.label}</span>
    </label>
);

export default CheckboxWidget;
