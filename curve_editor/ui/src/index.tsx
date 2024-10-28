import React from 'react';
import { createRoot } from 'react-dom/client';
import MainPanel from './panel_main';
import SelectDialog from './select_dialog';


window.chrome.webview.addEventListener('message', (event: MessageEvent) => {
    if (event.data.command == 'init') {
        const container = document.getElementById('root');
        const root = createRoot(container!);
        const isSelectDialog = event.data.isSelectDialog ?? false;
        let component: React.ReactElement;
        if (isSelectDialog) {
            const editMode = event.data.mode ?? 0;
            const param = event.data.param ?? 0;
            component = <SelectDialog editMode={editMode} param={param}/>;
        } else {
            component = <MainPanel/>;
        }
        root.render(component);
    }
});