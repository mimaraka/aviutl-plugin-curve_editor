import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faList, faMagnifyingGlass, faFolderPlus, faPenToSquare } from '@fortawesome/free-solid-svg-icons';
import { Grid, type CellComponentProps } from 'react-window';
import { ToolbarButtonIcon } from './button';
import PresetItem from './preset_item';
import { config, editor, preset } from './interface';
import './style/preset.scss';


interface PresetInfo {
    id: number;
    collectionId: number;
    name: string;
    date: number | null;
}

// セル間の余白（preset.scss の column-gap / row-gap と対応）と、PresetItem のテキスト領域の高さ
const COLUMN_GAP = 8;
const ROW_GAP = 12;
const ITEM_TEXT_HEIGHT = 16;
// 列数算出時にスクロールバー分の幅を差し引く（preset.scss の ::-webkit-scrollbar width と対応）
const SCROLLBAR_WIDTH = 6;

interface PresetCellProps {
    presets: PresetInfo[];
    presetSize: number;
    columns: number;
}

// react-window の Grid が各セルを描画するためのコンポーネント。
// 可視範囲のセルのみがマウントされるため、サムネイル描画の負荷が件数に依らず一定になる。
const PresetCell: React.FC<CellComponentProps<PresetCellProps>> = ({
    columnIndex, rowIndex, style, presets, presetSize, columns,
}) => {
    const index = rowIndex * columns + columnIndex;
    if (index >= presets.length) {
        return null;
    }
    const info = presets[index];
    return (
        // style（絶対配置・セルサイズ）はセルのルート要素に適用する。
        // 余白（COLUMN_GAP / ROW_GAP）はセルサイズに織り込んであり、PresetItem 自身の幅は presetSize。
        <div style={style}>
            {/* curveId で keying し、セルが別の curveId に再利用された際に
                CurveThumbnail（useEffect([]) で初回のみ d3 描画）が確実に再マウント＝再描画されるようにする */}
            <PresetItem
                key={info.id}
                curveId={info.id}
                collectionId={info.collectionId}
                name={info.name}
                date={info.date}
                width={presetSize}
            />
        </div>
    );
};


interface PresetProps {
    style: React.CSSProperties;
}

const PresetPanel: React.FC<PresetProps> = ({ style }) => {
    const [presetSize, setPresetSize] = React.useState(config.presetSize);
    const [gridWidth, setGridWidth] = React.useState(0);
    const [collectionInfo, setCollectionInfo] = React.useState(JSON.parse(preset.getCollectionsAsJson()));
    const originalPresetsInfo = React.useRef(JSON.parse(preset.getPresetsAsJson()));
    const [presetsInfo, setPresetsInfo] = React.useState(originalPresetsInfo.current);
    const [simpleView, setSimpleView] = React.useState(config.presetSimpleView);
    const presetContainer = React.useRef<HTMLDivElement>(null);

    // Grid の実寸幅（onResize で更新）から1行あたりの列数を算出する
    const columns = Math.max(1, Math.floor((gridWidth - SCROLLBAR_WIDTH) / (presetSize + COLUMN_GAP)));

    const onWheel = (event: WheelEvent) => {
        if (event.ctrlKey) {
            event.preventDefault();
            event.stopPropagation();
            config.presetSize = Math.max(30, Math.min(500, config.presetSize - event.deltaY / 100));
            setPresetSize(config.presetSize);
        }
    }

    const filterPresets = (presets: PresetInfo[]) => {
        const searchText = (document.getElementById('searchbar')! as HTMLInputElement).value;
        const filterInfo = JSON.parse(preset.getFilterInfoAsJson());
        return presets.filter(
            (info: PresetInfo) => {
                const includesSearchText = info.name.toLowerCase().includes(searchText.toLowerCase());
                const isCollectionIdSame = info.collectionId === preset.currentCollectionId;
                const curveName = editor.getCurveName(info.id);
                const filterTypeNormal = !filterInfo.typeNormal && curveName === 'normal';
                const filterTypeValue = !filterInfo.typeValue && curveName === 'value';
                const filterTypeBezier = !filterInfo.typeBezier && curveName === 'bezier';
                const filterTypeElastic = !filterInfo.typeElastic && curveName === 'elastic';
                const filterTypeBounce = !filterInfo.typeBounce && curveName === 'bounce';
                const filterTypeScript = !filterInfo.typeScript && curveName === 'script';
                return (
                    includesSearchText
                    && (preset.currentCollectionId == 0 ? true : isCollectionIdSame)
                    && !(filterTypeNormal || filterTypeValue || filterTypeBezier || filterTypeElastic || filterTypeBounce || filterTypeScript)
                );
            }
        );
    }

    const sortPresets = (presets: PresetInfo[]) => {
        return presets.sort((a: PresetInfo, b: PresetInfo) => {
            let result = 0;
            if (preset.sortBy === 'Date') {
                result = (a.date || 0) - (b.date || 0);
            }
            if (preset.sortBy !== 'Null' && result === 0) {
                const nameA = a.name.toLowerCase();
                const nameB = b.name.toLowerCase();
                result = nameA < nameB ? -1 : nameA > nameB ? 1 : 0;
            }
            return preset.sortOrder === 'Asc' ? result : -result;
        });
    }

    const updatePresets = () => {
        setPresetsInfo(sortPresets(filterPresets(originalPresetsInfo.current)));
    }

    const onSearchbarChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        updatePresets();
    }

    const onSelectChange = (event: React.ChangeEvent<HTMLSelectElement>) => {
        preset.currentCollectionId = parseInt(event.target.value);
        updatePresets();
    }

    const onSelectWheel = (event: React.WheelEvent<HTMLSelectElement>) => {
        //event.preventDefault();
        let newId;
        if (event.deltaY > 0) {
            newId = Math.min(preset.currentCollectionId + 1, preset.numCollections - 1);
        } else {
            newId = Math.max(preset.currentCollectionId - 1, 0);
        }
        if (preset.currentCollectionId != newId) {
            preset.currentCollectionId = newId;
            updatePresets();
        }
    }

    const onMessageFromHost = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'UpdatePresets':
                setCollectionInfo(JSON.parse(preset.getCollectionsAsJson()));
                originalPresetsInfo.current = JSON.parse(preset.getPresetsAsJson());
                updatePresets();
                break;

            case 'UpdatePresetLayout':
                setSimpleView(config.presetSimpleView);
                break;
        }
    }

    const onPresetMouseDown = (event: React.MouseEvent<HTMLDivElement>) => {
        if (event.button === 2) {
            window.chrome.webview.postMessage({
                command: 'ContextMenuPreset',
            });
        }
    }

    React.useEffect(() => {
        window.chrome.webview.addEventListener('message', onMessageFromHost);
        updatePresets();

        presetContainer.current?.addEventListener('wheel', onWheel, { passive: false });

        return () => {
            window.chrome.webview.removeEventListener('message', onMessageFromHost);
            presetContainer.current?.removeEventListener('wheel', onWheel);
        }
    }, []);

    return (
        <div className='container-panel-preset' style={style}>
            <div className='menu-row' style={{ display: simpleView ? 'none' : 'flex' }}>
                <div className='searchbar-container'>
                    <div className='searchbar-icon'>
                        <FontAwesomeIcon icon={faMagnifyingGlass} size='sm'/>
                    </div>
                    <input type='text' id='searchbar' name='searchbar' autoComplete='off' placeholder={window.stringTable['LabelSearchPresets']} onChange={onSearchbarChange}></input>
                </div>
                <div className='index-buttons'>
                    <ToolbarButtonIcon icon={faList} title={window.stringTable['TooltipButtonListConfig']} onClick={() => {
                        window.chrome.webview.postMessage({
                            command: 'ButtonPresetListSetting'
                        });
                    }}/>
                </div>
            </div>
            <div className='menu-row' style={{ display: simpleView ? 'none' : 'flex' }}>
                <div className='menu-button dropdown-container' id='collection-container'>
                    <div className='presets-count'>{`(${presetsInfo.length})`}</div>
                    <select
                        className='dropdown'
                        name='collection'
                        id='collection'
                        value={preset.currentCollectionId}
                        title={`${window.stringTable['WordCollection']} (${preset.getCollectionName(preset.currentCollectionId)})`}
                        onChange={onSelectChange}
                        onWheel={onSelectWheel}
                    >
                        {collectionInfo.map(({ id, name }: {id: number, name: string}, index: number) => {
                            return <option key={index} value={id}>{name}</option>
                        })}
                    </select>
                </div>
                <div className='collection-buttons'>
                    <ToolbarButtonIcon icon={faFolderPlus} title={window.stringTable['TooltipButtonCollectionAdd']} onClick={() => {
                        window.chrome.webview.postMessage({
                            command: 'ButtonCollectionAdd'
                        });
                    }}/>
                    <ToolbarButtonIcon icon={faPenToSquare} title={window.stringTable['TooltipButtonCollectionEdit']} onClick={() => {
                        window.chrome.webview.postMessage({
                            command: 'ButtonCollection'
                        });
                    }}/>
                </div>
            </div>
            <div className='container-preset' ref={presetContainer} onMouseDown={onPresetMouseDown}>
                {
                    presetsInfo.length > 0 ? (
                        <Grid
                            className='preset-grid'
                            cellComponent={PresetCell}
                            cellProps={{ presets: presetsInfo, presetSize, columns }}
                            columnCount={columns}
                            columnWidth={presetSize + COLUMN_GAP}
                            rowCount={Math.ceil(presetsInfo.length / columns)}
                            rowHeight={presetSize + ITEM_TEXT_HEIGHT + ROW_GAP}
                            overscanCount={2}
                            onResize={(size) => setGridWidth(size.width)}
                            style={{ width: '100%', height: '100%' }}
                        />
                    ) : (<div className='no-presets'>{window.stringTable['LabelNoPresets']}</div>)
                }
            </div>
        </div>
    );
}

export default PresetPanel;