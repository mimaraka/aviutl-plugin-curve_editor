#pragma once

#include "control_point.hpp"



namespace cved {
	class ElasticFreqDecayHandle {
	private:
		ControlPoint ctl_pt_;
		bool is_reverse_;

		auto get_dest_pt(
			const mkaul::Point<double>& pt,
			const ControlPoint& pt_start,
			const ControlPoint& pt_end
		) const noexcept;

	public:
		ElasticFreqDecayHandle() :
			ctl_pt_{},
			is_reverse_{false}
		{}

		const auto& pt() const noexcept { return ctl_pt_.pt(); }
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

		bool is_reverse() const noexcept { return is_reverse_; }
		void set_is_reverse(bool reverse, const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept;
		void reverse(const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept;

		double get_freq(const ControlPoint& pt_start, const ControlPoint& pt_end) const noexcept;
		double get_decay(const ControlPoint& pt_start, const ControlPoint& pt_end) const noexcept;

		void from_param(double freq, double decay, const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept;
	};
}