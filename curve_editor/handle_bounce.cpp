#include "handle_bounce.hpp"



namespace cved {
	void BounceHandle::reverse(const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept {
		is_reverse_ = !is_reverse_;
		move(
			mkaul::Point{
				pt_start.x() + pt_end.x() - ctl_pt_.x(),
				pt_start.y() + pt_end.y() - ctl_pt_.y()
			}, pt_start, pt_end
		);
	}

	void BounceHandle::set_is_reverse(bool is_reverse, const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept {
		if (is_reverse_ != is_reverse) {
			reverse(pt_start, pt_end);
		}
	}

	bool BounceHandle::is_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		return ctl_pt_.is_hovered(pt, view);
	}

	bool BounceHandle::check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		return ctl_pt_.check_hover(pt, view);
	}

	bool BounceHandle::update(
		const mkaul::Point<double>& pt,
		const ControlPoint& pt_start,
		const ControlPoint& pt_end
	) noexcept {
		return ctl_pt_.update(
			mkaul::Point{
				mkaul::clamp(pt.x, pt_start.x(), pt_end.x()),
				mkaul::clamp(pt.y, pt_start.y(), pt_end.y())
			}
		);
	}

	void BounceHandle::move(
		const mkaul::Point<double>& pt,
		const ControlPoint& pt_start,
		const ControlPoint& pt_end
	) noexcept {
		ctl_pt_.move(
			mkaul::Point{
				mkaul::clamp(pt.x, pt_start.x(), pt_end.x()),
				mkaul::clamp(pt.y, pt_start.y(), pt_end.y())
			}
		);
	}

	double BounceHandle::get_cor(const ControlPoint& pt_start, const ControlPoint& pt_end) const noexcept {
		double y = (ctl_pt_.y() - pt_start.y()) / (pt_end.y() - pt_start.y());
		if (is_reverse_) {
			y = 1. - y;
		}
		return std::sqrt(1. - mkaul::clamp(y, 0.001, 1.));
	}

	double BounceHandle::get_period(const ControlPoint& pt_start, const ControlPoint& pt_end) const noexcept {
		double x = (ctl_pt_.x() - pt_start.x()) / (pt_end.x() - pt_start.x());
		if (is_reverse_) {
			x = 1. - x;
		}
		return 2. * std::clamp(x, 0.001, 1.) / (get_cor(pt_start, pt_end) + 1.);
	}

	void BounceHandle::from_param(double cor, double period, const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept {
		double x = mkaul::clamp(period * (cor + 1.) * 0.5, 0., 1.);
		double y = mkaul::clamp(1. - cor * cor, 0., 1.);
		if (is_reverse_) {
			x = 1. - x;
			y = 1. - y;
		}

		move(
			mkaul::Point{
				pt_start.x() + x * (pt_end.x() - pt_start.x()),
				pt_start.y() + y * (pt_end.y() - pt_start.y())
			}, pt_start, pt_end
		);
	}
	
	void BounceHandle::end_control() noexcept {
		ctl_pt_.end_control();
	}
}