import React from 'react';
import { preferences } from '../interface';
import SchemaForm, { PrefItem } from '../preferences/schema_form';
import '../style/preferences_dialog.scss';


const PreferencesDialog: React.FC = () => {
    // スキーマと現在値を起動時に同期取得(編集中は UI ローカル state のみを操作する)
    const [schema] = React.useState<PrefItem[]>(() => JSON.parse(preferences.getSchemaJson()));
    const [values, setValues] = React.useState<Record<string, any>>(() => JSON.parse(preferences.getValuesJson()));
    // 言語変更の検出用に、適用済みの言語を保持する
    const appliedLanguage = React.useRef<number>(values.language);

    // スキーマの出現順からカテゴリ一覧を導出する
    const categories = React.useMemo(() => {
        const result: string[] = [];
        schema.forEach((item) => {
            if (!result.includes(item.category)) {
                result.push(item.category);
            }
        });
        return result;
    }, [schema]);
    const [activeCategory, setActiveCategory] = React.useState(categories[0] ?? '');

    const onChange = (key: string, value: any) => {
        setValues((prev) => ({ ...prev, [key]: value }));
    };

    // 現在の state を C++ に書き込み、言語が変わったかどうかを返す
    const writeValues = (): boolean => {
        preferences.setValuesJson(JSON.stringify(values));
        return values.language !== appliedLanguage.current;
    };

    const onApply = () => {
        const languageChanged = writeValues();
        window.chrome.webview.postMessage({ command: 'PreferencesApply', languageChanged });
        appliedLanguage.current = values.language;
    };

    const onOk = () => {
        const languageChanged = writeValues();
        window.chrome.webview.postMessage({ command: 'PreferencesOk', languageChanged });
    };

    const onCancel = () => {
        window.chrome.webview.postMessage({ command: 'PreferencesCancel' });
    };

    const onReset = () => {
        // デフォルト値で state を置換するのみ(適用するまで永続化しない)
        setValues(JSON.parse(preferences.getDefaultValuesJson()));
    };

    // 色・ファイル選択ダイアログの結果を受け取り、該当キーの値を更新する
    React.useEffect(() => {
        const onMessage = (event: MessageEvent) => {
            if (event.data.command === 'SetPrefValue') {
                const { key, value } = event.data;
                setValues((prev) => ({ ...prev, [key]: value }));
            }
        };
        window.chrome.webview.addEventListener('message', onMessage);
        return () => {
            window.chrome.webview.removeEventListener('message', onMessage);
        };
    }, []);

    return (
        <div className='preferences-container'>
            <div className='preferences-body'>
                <ul className='preferences-categories'>
                    {categories.map((category) => (
                        <li
                            key={category}
                            className={category === activeCategory ? 'active' : ''}
                            onClick={() => setActiveCategory(category)}
                        >
                            {category}
                        </li>
                    ))}
                </ul>
                <div className='preferences-content'>
                    <SchemaForm
                        items={schema}
                        category={activeCategory}
                        values={values}
                        onChange={onChange}
                    />
                </div>
            </div>
            <div className='preferences-footer'>
                <button className='pref-button pref-button-reset' onClick={onReset}>{window.stringTable['LabelReset']}</button>
                <div className='pref-footer-spacer' />
                <button className='pref-button' onClick={onApply}>{window.stringTable['WordApply']}</button>
                <button className='pref-button pref-button-ok' onClick={onOk}>{window.stringTable['WordOK']}</button>
                <button className='pref-button' onClick={onCancel}>{window.stringTable['WordCancel']}</button>
            </div>
        </div>
    );
};

export default PreferencesDialog;
