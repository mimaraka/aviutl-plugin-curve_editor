import React from 'react';
import { Grid, type CellComponentProps } from 'react-window';


// セル間の余白（preset.scss の column-gap / row-gap と対応）と、アイテムのテキスト領域の高さ
const COLUMN_GAP = 8;
const ROW_GAP = 12;
const ITEM_TEXT_HEIGHT = 16;
// 列数算出時にスクロールバー分の幅を差し引く（preset.scss の ::-webkit-scrollbar width と対応）
const SCROLLBAR_WIDTH = 6;

interface CurveGridCellProps {
    items: any[];
    itemSize: number;
    columns: number;
    getKey: (item: any) => React.Key;
    renderItem: (item: any, width: number) => React.ReactNode;
}

// react-window の Grid が各セルを描画するためのコンポーネント。
// 可視範囲のセルのみがマウントされるため、サムネイル描画の負荷が件数に依らず一定になる。
const CurveGridCell = ({
    columnIndex, rowIndex, style, items, itemSize, columns, getKey, renderItem,
}: CellComponentProps<CurveGridCellProps>): React.ReactElement | null => {
    const index = rowIndex * columns + columnIndex;
    if (index >= items.length) {
        return null;
    }
    const item = items[index];
    return (
        // style（絶対配置・セルサイズ）はセルのルート要素に適用する。
        // getKey でアイテムを keying し、セルが別のアイテムに再利用された際に
        // CurveThumbnail（useEffect([]) で初回のみ d3 描画）が確実に再マウント＝再描画されるようにする
        <div style={style}>
            <React.Fragment key={getKey(item)}>
                {renderItem(item, itemSize)}
            </React.Fragment>
        </div>
    );
};


interface CurveGridProps<T> {
    items: T[];
    itemSize: number;
    getKey: (item: T) => React.Key;
    renderItem: (item: T, width: number) => React.ReactNode;
    emptyLabel?: string;
}

// プリセットリストと ID 一覧で共通の、仮想化グリッドレイアウト。
function CurveGrid<T>({ items, itemSize, getKey, renderItem, emptyLabel }: CurveGridProps<T>) {
    const [gridWidth, setGridWidth] = React.useState(0);

    // Grid の実寸幅（onResize で更新）から1行あたりの列数を算出する
    const columns = Math.max(1, Math.floor((gridWidth - SCROLLBAR_WIDTH) / (itemSize + COLUMN_GAP)));

    if (items.length === 0) {
        return <div className='no-presets'>{emptyLabel ?? ''}</div>;
    }

    return (
        <Grid
            className='preset-grid'
            cellComponent={CurveGridCell}
            cellProps={{ items, itemSize, columns, getKey, renderItem }}
            columnCount={columns}
            columnWidth={itemSize + COLUMN_GAP}
            rowCount={Math.ceil(items.length / columns)}
            rowHeight={itemSize + ITEM_TEXT_HEIGHT + ROW_GAP}
            overscanCount={2}
            onResize={(size) => setGridWidth(size.width)}
            style={{ width: '100%', height: '100%' }}
        />
    );
}

export default CurveGrid;
