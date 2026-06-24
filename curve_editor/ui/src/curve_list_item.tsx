import React from 'react';
import CurveThumbnail from './curve_thumbnail';
import './style/preset_item.scss';


interface CurveListItemProps {
    width: number;
    curveId: number;
    label: string;
    title?: string;
    isCurrent?: boolean;
    onClick?: (event: React.MouseEvent) => void;
    onDoubleClick?: (event: React.MouseEvent) => void;
    onMouseDown?: (event: React.MouseEvent) => void;
    onMouseUp?: (event: React.MouseEvent) => void;
    onMouseLeave?: (event: React.MouseEvent) => void;
}

// プリセットリストと ID 一覧で共通の「サムネイル + ラベル」表示部。
// マウス操作などの挙動は呼び出し側からハンドラとして受け取る。
const CurveListItem: React.FC<CurveListItemProps> = (props: CurveListItemProps) => {
    return (
        <div className='preset-item' style={{ width: props.width, height: props.width + 16 /*<- TODO: 直す*/ }}>
            <div
                className={'preset-item-thumbnail' + (props.isCurrent ? ' preset-item-thumbnail-current' : '')}
                title={props.title}
                onClick={props.onClick}
                onDoubleClick={props.onDoubleClick}
                onMouseDown={props.onMouseDown}
                onMouseUp={props.onMouseUp}
                onMouseLeave={props.onMouseLeave}
            >
                <CurveThumbnail curveId={props.curveId} width={props.width - 4}/>
            </div>
            <div className='preset-item-text'>
                {props.label}
            </div>
        </div>
    );
};

export default CurveListItem;
