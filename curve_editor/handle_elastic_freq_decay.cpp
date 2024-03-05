#include "handle_elastic_freq_decay.hpp"
#include <algorithm>



namespace cved {
	bool ElasticFreqDecayHandle::is_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept {
		return point_.is_hovered(point, box_width, view);
	}

	bool ElasticFreqDecayHandle::check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept {
		return point_.check_hover(point, box_width, view);
	}

	auto ElasticFreqDecayHandle::get_dest_point(
		const mkaul::Point<double>& point,
		const ControlPoint& point_start,
		const ControlPoint& point_end
	) const noexcept {
		return mkaul::Point{
			mkaul::clamp(point.x, point_start.x(), point_end.x()),
			mkaul::clamp(point.y, point_end.y(), point_end.y() - std::abs(point_end.y() - point_start.y()))
		};
	}

	bool ElasticFreqDecayHandle::update(
		const mkaul::Point<double>& point,
		const ControlPoint& point_start,
		const ControlPoint& point_end
	) noexcept {
		return point_.update(get_dest_point(point, point_start, point_end));
	}

	void ElasticFreqDecayHandle::move(
		const mkaul::Point<double>& point,
		const ControlPoint& point_start,
		const ControlPoint& point_end
	) noexcept {
		point_.move(get_dest_point(point, point_start, point_end));
	}

	void ElasticFreqDecayHandle::end_control() noexcept {
		point_.end_control();
	}

	void ElasticFreqDecayHandle::set_is_reverse(bool is_reverse, const ControlPoint& point_start, const ControlPoint& point_end) noexcept {
		if (is_reverse_ != is_reverse) {
			reverse(point_start, point_end);
		}
	}

	void ElasticFreqDecayHandle::reverse(const ControlPoint& point_start, const ControlPoint& point_end) noexcept {
		is_reverse_ = !is_reverse_;
		move(
			mkaul::Point{ point_start.x() + point_end.x() - point_.x(), point_.y() },
			point_start, point_end
		);
	}

	double ElasticFreqDecayHandle::get_freq(const ControlPoint& point_start, const ControlPoint& point_end) const noexcept {
		double value;
		if (is_reverse_) {
			value = point_end.x() - point_.x();
		}
		else {
			value = point_.x() - point_start.x();
		}
		return std::max(0.5 / std::max(value / (point_end.x() - point_start.x()), 0.0001), 0.5);
	}

	double ElasticFreqDecayHandle::get_decay(const ControlPoint& point_start, const ControlPoint& point_end) const noexcept {
		const double point_delta = std::abs(point_start.y() - point_end.y());
		const double value = point_.y() - point_end.y() + point_delta;
		return -10. * std::log(-mkaul::clamp(value / point_delta, 0., 0.9999) + 1.) + 1.;
	}

	void ElasticFreqDecayHandle::from_param(double freq, double decay, const ControlPoint& point_start, const ControlPoint& point_end) noexcept {
		double value;
		if (is_reverse_) {
			value = (1. - 0.5 / freq);
		}
		else {
			value = 0.5 / freq;
		}
		const double point_delta_x = point_end.x() - point_start.x();
		const double point_delta_y = std::abs(point_start.y() - point_end.y());
		move(
			mkaul::Point{
				 point_start.x() + value * point_delta_x,
				 point_end.y() - std::exp(-(decay - 1.) * 0.1) * point_delta_y
			},
			point_start, point_end
		);
	}
}