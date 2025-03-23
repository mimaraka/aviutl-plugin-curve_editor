import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faList, faMagnifyingGlass, faFolderPlus, faPenToSquare } from '@fortawesome/free-solid-svg-icons';
import { ToolbarButtonIcon } from './button';
import PresetItem from './preset_item';
import { config, editor, preset } from './interface';
import './style/preset.scss';


interface PresetProps {
    style: React.CSSProperties;
}

const PresetPanel: React.FC<PresetProps> = ({ style }) => {
    interface PresetInfo {
        id: number;
        collectionId: number;
        name: string;
        date: number | null;
    }

    const [presetSize, setPresetSize] = React.useState(config.presetSize);
    const [collectionInfo, setCollectionInfo] = React.useState(JSON.parse(preset.getCollectionsAsJson()));
    const originalPresetsInfo = React.useRef(JSON.parse(preset.getPresetsAsJson()));
    const [presetsInfo, setPresetsInfo] = React.useState(originalPresetsInfo.current);
    const [simpleView, setSimpleView] = React.useState(config.presetSimpleView);
    const presetContainer = React.useRef<HTMLDivElement>(null);

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
                        presetsInfo.map((info: PresetInfo) => (
                            <PresetItem key={info.id} curveId={info.id} collectionId={info.collectionId} name={info.name} date={info.date} width={presetSize} />
                        ))
                    ) : (<div className='no-presets'>{window.stringTable['LabelNoPresets']}</div>)
                }
            </div>
        </div>
    );
}

export default PresetPanel;