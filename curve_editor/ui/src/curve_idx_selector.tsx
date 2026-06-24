import React from 'react';
import { config, editor } from './interface';
import CurveListItem from './curve_list_item';
import CurveGrid from './curve_grid';
import './style/curve_idx_selector.scss';


interface IdxInfo {
    idx: number;
    id: number;
}

const CurveIdxSelector: React.FC = () => {
    const [items] = React.useState<IdxInfo[]>(() => JSON.parse(editor.getCurveIdxArrayAsJson()));
    const currentIdx = React.useRef(editor.currentIdx);
    const [itemSize, setItemSize] = React.useState(config.presetSize);
    const containerRef = React.useRef<HTMLDivElement>(null);

    // Ctrl + ホイールでアイテムサイズを変更（プリセットリストと同様。設定への永続化はしない）
    const onWheel = (event: WheelEvent) => {
        if (event.ctrlKey) {
            event.preventDefault();
            event.stopPropagation();
            setItemSize((prev) => Math.max(30, Math.min(500, prev - event.deltaY / 100)));
        }
    }

    const onItemClick = (idx: number) => {
        window.chrome.webview.postMessage({
            command: 'JumpToIdx',
            idx: idx
        });
    }

    React.useEffect(() => {
        containerRef.current?.addEventListener('wheel', onWheel, { passive: false });
        return () => {
            containerRef.current?.removeEventListener('wheel', onWheel);
        }
    }, []);

    return (
        <div className='container-idx-selector' ref={containerRef}>
            <CurveGrid
                items={items}
                itemSize={itemSize}
                getKey={(info: IdxInfo) => info.id}
                renderItem={(info: IdxInfo, width: number) => (
                    <CurveListItem
                        width={width}
                        curveId={info.id}
                        label={String(info.idx + 1)}
                        title={String(info.idx + 1)}
                        isCurrent={info.idx === currentIdx.current}
                        onClick={() => onItemClick(info.idx)}
                    />
                )}
            />
        </div>
    );
}

export default CurveIdxSelector;
