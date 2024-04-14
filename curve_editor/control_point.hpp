#pragma once

#include "view_graph.hpp"
#include <mkaul/include/point.hpp>



namespace cved {
	// 操作可能なポイント
	class ControlPoint {
	private:
		mkaul::Point<double> point_;
		bool controlled_;
		bool fixed_;

	public:
		constexpr ControlPoint(const mkaul::Point<double>& point = mkaul::Point<double>{}, bool fixed = false) :
			point_{point},
			controlled_{false},
			fixed_{fixed}
		{}

		auto operator = (const mkaul::Point<double>& point) noexcept {
			this->point_ = point;
			return *this;
		}

		// アクセッサ
		const auto& point() const noexcept { return point_; }
		auto x() const noexcept { return point_.x; }
		auto y() const noexcept { return point_.y; }

		// 引数のポイントがコントロールポイント付近にあるかどうか
		bool is_hovered(const mkaul::Point<double>& point, const GraphView& view) const noexcept;
		bool is_controlled() const noexcept { return controlled_; }
		bool check_hover(const mkaul::Point<double>& point, const GraphView& view) noexcept;
		
		bool update(const mkaul::Point<double>& point) noexcept;
		void move(const mkaul::Point<double>& point) noexcept;

		void end_control() noexcept { controlled_ = false; }
	};
}