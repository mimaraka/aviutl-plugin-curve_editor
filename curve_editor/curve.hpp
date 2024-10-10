#pragma once

#include <mkaul/graphics.hpp>



namespace cved {
	// カーブ(抽象クラス)
	class Curve {
		bool locked_ = false;

	public:
		virtual ~Curve() noexcept = default;

		// カーブの値を取得
		virtual double curve_function(double progress, double start, double end) const noexcept = 0;
		virtual double get_value(double progress, double start, double end) const noexcept;
		double get_velocity(double progress, double start, double end) const noexcept;
		virtual void clear() noexcept = 0;
		virtual void lock() noexcept { locked_ = true; }
		virtual void unlock() noexcept { locked_ = false; }
		auto is_locked() const noexcept { return locked_; }
		virtual bool is_default() const noexcept = 0;
		constexpr virtual std::string get_type() const noexcept = 0;
	};
} // namespace cved