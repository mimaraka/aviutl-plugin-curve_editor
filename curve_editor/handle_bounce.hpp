#pragma once

#include "control_point.hpp"



namespace cved {
	class BounceHandle {
	private:
		ControlPoint point_;
		bool is_reverse_;

	public:
		BounceHandle() :
			point_{},
			is_reverse_{false}
		{}

		const auto& get_point() const noexcept { return point_; }

		bool is_reverse() const noexcept { return is_reverse_; }
		void reverse(const ControlPoint& point_start, const ControlPoint& point_end) noexcept;
		void set_is_reverse(bool reverse, const ControlPoint& point_start, const ControlPoint& point_end) noexcept;

		bool is_hovered(const mkaul::Point<double>& point, const GraphView& view) const noexcept;
		bool check_hover(const mkaul::Point<double>& point, const GraphView& view) noexcept;
		bool update(
			const mkaul::Point<double>& point,
			const ControlPoint& point_start,
			const ControlPoint& point_end
		) noexcept;
		void move(
			const mkaul::Point<double>& point,
			const ControlPoint& point_start,
			const ControlPoint& point_end
		) noexcept;
		void end_control() noexcept;

		double get_cor(const ControlPoint& point_start, const ControlPoint& point_end) const noexcept;
		double get_period(const ControlPoint& point_start, const ControlPoint& point_end) const noexcept;
		void from_param(double cor, double period, const ControlPoint& point_start, const ControlPoint& point_end) noexcept;
	};
}