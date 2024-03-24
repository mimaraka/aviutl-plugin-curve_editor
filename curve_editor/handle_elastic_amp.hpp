#pragma once

#include "control_point.hpp"



namespace cved {
	class ElasticAmpHandle {
	private:
		ControlPoint point_left_;
		ControlPoint point_right_;

	public:
		ElasticAmpHandle() :
			point_left_{},
			point_right_{}
		{}

		const auto& point_left() const noexcept { return point_left_; }
		const auto& point_right() const noexcept { return point_right_; }

		bool is_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept;
		bool check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept;
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

		double get_amp(const ControlPoint& point_start, const ControlPoint& point_end) const noexcept;
		void from_param(double amp, const ControlPoint& point_start, const ControlPoint& point_end) noexcept;
	};
}