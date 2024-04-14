#pragma once

#include "control_point.hpp"



namespace cved {
	class ElasticFreqDecayHandle {
	private:
		ControlPoint point_;
		bool is_reverse_;

		auto get_dest_point(
			const mkaul::Point<double>& point,
			const ControlPoint& point_start,
			const ControlPoint& point_end
		) const noexcept;

	public:
		ElasticFreqDecayHandle() :
			point_{},
			is_reverse_{false}
		{}

		const auto& point() const noexcept { return point_; }
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

		bool is_reverse() const noexcept { return is_reverse_; }
		void set_is_reverse(bool reverse, const ControlPoint& point_start, const ControlPoint& point_end) noexcept;
		void reverse(const ControlPoint& point_start, const ControlPoint& point_end) noexcept;

		double get_freq(const ControlPoint& point_start, const ControlPoint& point_end) const noexcept;
		double get_decay(const ControlPoint& point_start, const ControlPoint& point_end) const noexcept;

		void from_param(double freq, double decay, const ControlPoint& point_start, const ControlPoint& point_end) noexcept;
	};
}