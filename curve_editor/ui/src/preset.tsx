import React from 'react';
import { faMagnifyingGlass, faFilter, faArrowDownWideShort, faPlus, faFileImport, faFileExport } from '@fortawesome/free-solid-svg-icons';
import { ToolbarButtonIcon } from './button';
import './scss/preset.scss';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';


interface PresetProps {
    style: React.CSSProperties;
}

const Preset: React.FC<PresetProps> = ({ style }) => {
    return (
        <div className='preset-container' style={style}>
            <div className='menu-row'>
                <div className='searchbar-container'>
                    <div className='searchbar-icon'>
                        <FontAwesomeIcon icon={faMagnifyingGlass} size='sm'/>
                    </div>
                    <input type='text' id='searchbar' name='searchbar' autoComplete='off' placeholder='プリセットを検索...'></input>
                </div>
                <div className='index-buttons'>
                    <ToolbarButtonIcon icon={faFilter} title='フィルター'/>
                    <ToolbarButtonIcon icon={faArrowDownWideShort} title='並び替え'/>
                </div>
            </div>
            <div className='menu-row'>
                <div className='menu-button dropdown-container' id='collection-container'>
                    <select className='dropdown' name='collection' id='collection' title='コレクション'>
                        <option value='0'>すべて</option>
                        <option value='1'>デフォルト</option>
                        <option value='2'>ユーザー</option>
                    </select>
                </div>
                <div className='collection-buttons'>
                    <ToolbarButtonIcon icon={faPlus} title='新規コレクションを作成'/>
                    <ToolbarButtonIcon icon={faFileImport} title='コレクションをインポート'/>
                    <ToolbarButtonIcon icon={faFileExport} title='コレクションをエクスポート'/>
                </div>
            </div>
        </div>
    );
}

export default Preset;