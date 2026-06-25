import React from 'react';
import CheckboxWidget from './widgets/checkbox_widget';
import SliderWidget from './widgets/slider_widget';
import ComboWidget from './widgets/combo_widget';
import ColorWidget from './widgets/color_widget';
import FilePathWidget from './widgets/file_path_widget';
import TextWidget from './widgets/text_widget';


// C++ の build_schema_json() が出力する1項目分のスキーマ
export interface PrefItem {
    key: string;
    widget: 'checkbox' | 'slider' | 'combo' | 'color' | 'filepath' | 'text';
    label: string;
    category: string;
    min?: number;
    max?: number;
    step?: number;
    options?: string[];
}

// 各ウィジェットに渡す共通 props
export interface WidgetProps {
    item: PrefItem;
    value: any;
    onChange: (value: any) => void;
}

interface SchemaFormProps {
    items: PrefItem[];
    category: string;
    values: Record<string, any>;
    onChange: (key: string, value: any) => void;
}

// スキーマを category でフィルタし、widget 種別ごとに対応するウィジェットを描画する汎用フォーム
const SchemaForm: React.FC<SchemaFormProps> = ({ items, category, values, onChange }) => {
    const renderWidget = (item: PrefItem) => {
        const props: WidgetProps = {
            item,
            value: values[item.key],
            onChange: (value: any) => onChange(item.key, value),
        };
        switch (item.widget) {
            case 'checkbox':
                return <CheckboxWidget {...props} />;
            case 'slider':
                return <SliderWidget {...props} />;
            case 'combo':
                return <ComboWidget {...props} />;
            case 'color':
                return <ColorWidget {...props} />;
            case 'filepath':
                return <FilePathWidget {...props} />;
            default:
                return <TextWidget {...props} />;
        }
    };

    return (
        <div className='pref-form'>
            {items
                .filter((item) => item.category === category)
                .map((item) => (
                    <div key={item.key} className='pref-item'>
                        {renderWidget(item)}
                    </div>
                ))}
        </div>
    );
};

export default SchemaForm;
