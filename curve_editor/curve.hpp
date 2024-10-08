#pragma once

#include <mkaul/graphics.hpp>



namespace cved {
	// カーブ(抽象クラス)
	class Curve {
	public:
		virtual ~Curve() noexcept = default;

		// カーブの値を取得
		virtual double curve_function(double progress, double start, double end) const noexcept = 0;
		virtual double get_value(double progress, double start, double end) const noexcept;
		double get_velocity(double progress, double start, double end) const noexcept;
		virtual void clear() noexcept = 0;
		virtual bool is_default() const noexcept = 0;
		virtual std::string get_type() const noexcept = 0;
	};
} // namespace cved