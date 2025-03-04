import React from 'react';
import { IconDefinition } from '@fortawesome/fontawesome-svg-core';
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome';
import './style/button.scss';


interface ToolbarButtonProps {
    title?: string;
    onClick?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    onMouseDown?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    onMouseUp?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    onMouseLeave?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    disabled?: boolean;
    children: any;
    style?: React.CSSProperties;
    badge?: boolean;
}

export interface ToolbarButtonIconProps {
    title?: string;
    onClick?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    onMouseDown?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    onMouseUp?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    onMouseLeave?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    disabled?: boolean;
    icon: IconDefinition;
    style?: React.CSSProperties;
    badge?: boolean;
}

export interface ToolbarButtonTextProps {
    title?: string;
    onClick?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    onMouseDown?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    onMouseUp?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    onMouseLeave?: (event: React.MouseEvent<HTMLButtonElement>) => void;
    disabled?: boolean;
    label: string;
    style?: React.CSSProperties;
    badge?: boolean;
}

const ToolbarButton: React.FC<ToolbarButtonProps> = (props: ToolbarButtonProps) => {
    return (
        <button
            className='button-toolbar'
            style={props.style}
            title={props.title}
            onClick={props.onClick}
            onMouseDown={props.onMouseDown}
            onMouseUp={props.onMouseUp}
            onMouseLeave={props.onMouseLeave}
            disabled={props.disabled}
        >
            {props.badge ? <div className='badge'></div> : null}{props.children}
        </button>
    );
}

const ToolbarButtonIcon: React.FC<ToolbarButtonIconProps> = (props: ToolbarButtonIconProps) => {
    return (
        <ToolbarButton
            title={props.title}
            onClick={props.onClick}
            onMouseDown={props.onMouseDown}
            onMouseUp={props.onMouseUp}
            onMouseLeave={props.onMouseLeave}
            disabled={props.disabled}
            badge={props.badge}
        >
            <FontAwesomeIcon icon={props.icon} size='sm' />
        </ToolbarButton>
    );
}

const ToolbarButtonText: React.FC<ToolbarButtonTextProps> = (props: ToolbarButtonTextProps) => {
    return (
        <ToolbarButton
            title={props.title}
            onClick={props.onClick}
            onMouseDown={props.onMouseDown}
            onMouseUp={props.onMouseUp}
            onMouseLeave={props.onMouseLeave}
            disabled={props.disabled}
            badge={props.badge}
        >
            <p className='label'>{props.label}</p>
        </ToolbarButton>
    );
}

export { ToolbarButtonIcon, ToolbarButtonText };