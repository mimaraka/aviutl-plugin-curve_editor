#pragma once

#include "control_point.hpp"



namespace cved {
	class ElasticAmpHandle {
	private:
		ControlPoint ctl_pt_left_;
		ControlPoint ctl_pt_right_;

	public:
		ElasticAmpHandle() :
			ctl_pt_left_{},
			ctl_pt_right_{}
		{}

		const auto& pt_left() const noexcept { return ctl_pt_left_.pt(); }
		const auto& pt_right() const noexcept { return ctl_pt_right_.pt(); }

		bool is_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept;
		bool check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept;
		bool update(
			const mkaul::Point<double>& pt,
			const ControlPoint& pt_start,
			const ControlPoint& pt_end
		) noexcept;
		void move(
			const mkaul::Point<double>& pt,
			const ControlPoint& pt_start,
			const ControlPoint& pt_end
		) noexcept;
		void end_control() noexcept;

		double get_amp(const ControlPoint& pt_start, const ControlPoint& pt_end) const noexcept;
		void from_param(double amp, const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept;
	};
}