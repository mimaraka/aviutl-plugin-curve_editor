// 座標表示用の丸め桁数
const COORD_DECIMAL_DIGITS = 4;

// 数値を指定桁数で丸め、末尾のゼロを除去して文字列化
const roundCoord = (value: number) => {
    const factor = 10 ** COORD_DECIMAL_DIGITS;
    return (Math.round(value * factor) / factor).toString();
};

// 座標を (X, Y) 形式の文字列に整形
export const formatCoord = (x: number, y: number) => {
    return `(${roundCoord(x)}, ${roundCoord(y)})`;
};
