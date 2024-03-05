#pragma once

#include "curve.hpp"



namespace cved {
	// カーブ(スクリプト)
	class ScriptCurve : public Curve {
	public:
		double get_value(double progress, double start, double end) const noexcept override { return start; }
		void clear() noexcept override {}
	};
}