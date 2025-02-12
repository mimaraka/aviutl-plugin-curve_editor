import React from 'react';
import { createRoot } from 'react-dom/client';
import MainPanel from './panel_main';
import SelectDialog from './select_dialog';
import CurveIdxSelector from './curve_idx_selector';


window.chrome.webview.addEventListener('message', (event: MessageEvent) => {
    if (event.data.command == 'InitComponent') {
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

            default:
                return;
        }
        root.render(component);
    }
});