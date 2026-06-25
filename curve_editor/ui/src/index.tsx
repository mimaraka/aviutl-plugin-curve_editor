import React from 'react';
import { createRoot } from 'react-dom/client';
import editorWorker from 'monaco-editor/esm/vs/editor/editor.worker?worker';
import MainPanel from './panels/panel_main';
import SelectDialog from './dialogs/select_dialog';
import CurveIdxSelector from './dialogs/curve_idx_selector';
import PreferencesDialog from './dialogs/preferences_dialog';


// Monaco エディタの Worker 設定（旧 monaco-editor-webpack-plugin の代替）
self.MonacoEnvironment = {
    getWorker() {
        return new editorWorker();
    },
};


window.chrome.webview.addEventListener('message', (event: MessageEvent) => {
    switch (event.data.command) {
        case 'InitComponent':
            const container = document.getElementById('root');
            const root = createRoot(container!);
            const page = event.data.page;
            const isUpdateAvailable = event.data.isUpdateAvailable ?? false;
            let component: React.ReactElement;
            switch (page) {
                case 'MainPanel':
                    component = <MainPanel isUpdateAvailable={isUpdateAvailable}/>;
                    break;

                case 'SelectDialog':
                    const editMode = event.data.mode ?? 0;
                    const param = event.data.param ?? 0;
                    component = <SelectDialog editMode={editMode} param={param}/>;
                    break;

                case 'CurveIdxSelector':
                    component = <CurveIdxSelector/>;
                    break;

                case 'PreferencesDialog':
                    component = <PreferencesDialog/>;
                    break;

                default:
                    return;
            }
            root.render(component);
            break;

        case 'LoadStringTable':
            window.stringTable = event.data.stringTable;
            break;
    }
});