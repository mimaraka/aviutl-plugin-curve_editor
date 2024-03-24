#pragma once

#include "curve_graph.hpp"



namespace cved {
	class NumericGraphCurve : public GraphCurve {
	public:
		using GraphCurve::GraphCurve;

		// カーブから一意な整数値を生成
		virtual int32_t encode() const noexcept = 0;
		// 整数値からカーブに変換
		virtual bool decode(int32_t code) noexcept = 0;
	};
}