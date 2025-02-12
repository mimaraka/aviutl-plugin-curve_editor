import React from 'react';


interface CurveIdxSelectorProps {
}

const CurveIdxSelector: React.FC<CurveIdxSelectorProps> = (props: CurveIdxSelectorProps) => {
    //const [idx, setIdx] = React.useState(props.idx);

    const changeIdx = (idx: number) => {
        //setIdx(idx);
        //props.onChange(idx);
    }

    return (
        <input type="number" onChange={(e) => changeIdx(parseInt(e.target.value))} />
    );
}

export default CurveIdxSelector;