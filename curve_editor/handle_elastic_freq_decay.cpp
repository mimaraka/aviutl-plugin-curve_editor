#include "handle_elastic_freq_decay.hpp"
#include <algorithm>



namespace cved {
	bool ElasticFreqDecayHandle::is_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		return ctl_pt_.is_hovered(pt, view);
	}

	bool ElasticFreqDecayHandle::check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		return ctl_pt_.check_hover(pt, view);
	}

	auto ElasticFreqDecayHandle::get_dest_pt(
		const mkaul::Point<double>& pt,
		const ControlPoint& pt_start,
		const ControlPoint& pt_end
	) const noexcept {
		return mkaul::Point{
			mkaul::clamp(pt.x, pt_start.x(), pt_end.x()),
			mkaul::clamp(pt.y, pt_end.y(), pt_end.y() - std::abs(pt_end.y() - pt_start.y()))
		};
	}

	bool ElasticFreqDecayHandle::update(
		const mkaul::Point<double>& pt,
		const ControlPoint& pt_start,
		const ControlPoint& pt_end
	) noexcept {
		return ctl_pt_.update(get_dest_pt(pt, pt_start, pt_end));
	}

	void ElasticFreqDecayHandle::move(
		const mkaul::Point<double>& pt,
		const ControlPoint& pt_start,
		const ControlPoint& pt_end
	) noexcept {
		ctl_pt_.move(get_dest_pt(pt, pt_start, pt_end));
	}

	void ElasticFreqDecayHandle::end_control() noexcept {
		ctl_pt_.end_control();
	}

	void ElasticFreqDecayHandle::set_is_reverse(bool is_reverse, const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept {
		if (is_reverse_ != is_reverse) {
			reverse(pt_start, pt_end);
		}
	}

	void ElasticFreqDecayHandle::reverse(const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept {
		is_reverse_ = !is_reverse_;
		move(
			mkaul::Point{ pt_start.x() + pt_end.x() - ctl_pt_.x(), ctl_pt_.y() },
			pt_start, pt_end
		);
	}

	double ElasticFreqDecayHandle::get_freq(const ControlPoint& pt_start, const ControlPoint& pt_end) const noexcept {
		double value;
		if (is_reverse_) {
			value = pt_end.x() - ctl_pt_.x();
		}
		else {
			value = ctl_pt_.x() - pt_start.x();
		}
		return std::max(0.5 / std::max(value / (pt_end.x() - pt_start.x()), 0.0001), 0.5);
	}

	double ElasticFreqDecayHandle::get_decay(const ControlPoint& pt_start, const ControlPoint& pt_end) const noexcept {
		const double pt_delta = std::abs(pt_start.y() - pt_end.y());
		const double value = ctl_pt_.y() - pt_end.y() + pt_delta;
		return -10. * std::log(-mkaul::clamp(value / pt_delta, 0., 0.9999) + 1.) + 1.;
	}

	void ElasticFreqDecayHandle::from_param(double freq, double decay, const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept {
		double value;
		if (is_reverse_) {
			value = (1. - 0.5 / freq);
		}
		else {
			value = 0.5 / freq;
		}
		const double pt_delta_x = pt_end.x() - pt_start.x();
		const double pt_delta_y = std::abs(pt_start.y() - pt_end.y());
		move(
			mkaul::Point{
				 pt_start.x() + value * pt_delta_x,
				 pt_end.y() - std::exp(-(decay - 1.) * 0.1) * pt_delta_y
			},
			pt_start, pt_end
		);
	}
}