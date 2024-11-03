import React from 'react';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import { faMagnifyingGlass, faFilter, faArrowDownWideShort, faFileCirclePlus, faEllipsisVertical } from '@fortawesome/free-solid-svg-icons';
import { ToolbarButtonIcon } from './button';
import PresetItem from './preset_item';
import { config, preset } from './host_object';
import './scss/preset.scss';


interface PresetProps {
    style: React.CSSProperties;
}

const PresetPanel: React.FC<PresetProps> = ({ style }) => {
    interface PresetInfo {
        id: number;
        collectionId: number;
        name: string;
    }

    const [presetSize, setPresetSize] = React.useState(config.presetSize);
    const [collectionInfo, setCollectionInfo] = React.useState(JSON.parse(preset.getCollectionsAsJson()));
    const originalPresetsInfo = React.useRef(JSON.parse(preset.getPresetsAsJson()));
    const [presetsInfo, setPresetsInfo] = React.useState(originalPresetsInfo.current);
    const presetContainer = React.useRef<HTMLDivElement>(null);

    const onWheel = (event: WheelEvent) => {
        if (event.ctrlKey) {
            event.preventDefault();
            event.stopPropagation();
            config.presetSize = Math.max(30, Math.min(500, config.presetSize - event.deltaY / 100));
            setPresetSize(config.presetSize);
        }
    }

    const filterPresets = () => {
        const searchText = (document.getElementById('searchbar')! as HTMLInputElement).value;
        setPresetsInfo(
            originalPresetsInfo.current.filter(
                (info: PresetInfo) => {
                    const includesSearchText = info.name.toLowerCase().includes(searchText.toLowerCase());
                    const isCollectionIdSame = info.collectionId === preset.currentCollectionId;
                    return includesSearchText && (preset.currentCollectionId == 0 ? true : isCollectionIdSame);
                }
            )
        );
    }

    const onSearchbarChange = (event: React.ChangeEvent<HTMLInputElement>) => {
        filterPresets();
    }

    const onSelectChange = (event: React.ChangeEvent<HTMLSelectElement>) => {
        preset.currentCollectionId = parseInt(event.target.value);
        filterPresets();
    }

    const onMessageFromHost = (event: MessageEvent) => {
        switch (event.data.command) {
            case 'UpdatePresets':
                setCollectionInfo(JSON.parse(preset.getCollectionsAsJson()));
                originalPresetsInfo.current = JSON.parse(preset.getPresetsAsJson());
                filterPresets();
                break;
        }
    }

    React.useEffect(() => {
        window.chrome.webview.addEventListener('message', onMessageFromHost);
        filterPresets();

        presetContainer.current?.addEventListener('wheel', onWheel, { passive: false });

        return () => {
            window.chrome.webview.removeEventListener('message', onMessageFromHost);
            presetContainer.current?.removeEventListener('wheel', onWheel);
        }
    }, []);

    return (
        <div className='container-panel-preset' style={style}>
            <div className='menu-row'>
                <div className='searchbar-container'>
                    <div className='searchbar-icon'>
                        <FontAwesomeIcon icon={faMagnifyingGlass} size='sm'/>
                    </div>
                    <input type='text' id='searchbar' name='searchbar' autoComplete='off' placeholder='プリセットを検索...' onChange={onSearchbarChange}></input>
                </div>
                <div className='index-buttons'>
                    <ToolbarButtonIcon icon={faFilter} title='フィルター' onClick={() => {
                        window.chrome.webview.postMessage({
                            command: 'ButtonPresetFilter'
                        });
                    }}/>
                    <ToolbarButtonIcon icon={faArrowDownWideShort} title='並び替え' onClick={() => {
                        window.chrome.webview.postMessage({
                            command: 'ButtonPresetSort'
                        });
                    }}/>
                </div>
            </div>
            <div className='menu-row'>
                <div className='menu-button dropdown-container' id='collection-container'>
                    <div className='presets-count'>{`(${presetsInfo.length})`}</div>
                    <select
                        className='dropdown'
                        name='collection'
                        id='collection'
                        value={preset.currentCollectionId}
                        title={`コレクション(${preset.getCollectionName(preset.currentCollectionId)})`}
                        onChange={onSelectChange}
                    >
                        {collectionInfo.map(({ id, name }: {id: number, name: string}, index: number) => {
                            return <option key={index} value={id}>{name}</option>
                        })}
                    </select>
                </div>
                <div className='collection-buttons'>
                    <ToolbarButtonIcon icon={faFileCirclePlus} title='コレクションの作成・追加' onClick={() => {
                        window.chrome.webview.postMessage({
                            command: 'ButtonCollectionAdd'
                        });
                    }}/>
                    <ToolbarButtonIcon icon={faEllipsisVertical} title='コレクションの操作' onClick={() => {
                        window.chrome.webview.postMessage({
                            command: 'ButtonCollection'
                        });
                    }}/>
                </div>
            </div>
            <div className='container-preset' ref={presetContainer}>
                {presetsInfo.map((info: PresetInfo) => {
                    return <PresetItem key={info.id} curveId={info.id} collectionId={info.collectionId} name={info.name} width={presetSize}/>
                })}
            </div>
        </div>
    );
}

export default PresetPanel;