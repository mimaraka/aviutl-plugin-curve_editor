#pragma once

#include <mkaul/include/graphics.hpp>



namespace cved {
	// カーブ(抽象クラス)
	class Curve {
	public:
		// カーブの値を取得
		virtual double get_value(double progress, double start, double end) const noexcept = 0;
		double get_velocity(double progress, double start, double end) const noexcept;
		virtual void clear() noexcept = 0;
	};
}