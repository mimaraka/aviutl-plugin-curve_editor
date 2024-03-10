#include "handle_bounce.hpp"



namespace cved {
	void BounceHandle::reverse(const ControlPoint& point_start, const ControlPoint& point_end) noexcept {
		is_reverse_ = !is_reverse_;
		move(
			mkaul::Point{
				point_start.x() + point_end.x() - point_.x(),
				point_start.y() + point_end.y() - point_.y()
			}, point_start, point_end
		);
	}

	void BounceHandle::set_is_reverse(bool is_reverse, const ControlPoint& point_start, const ControlPoint& point_end) noexcept {
		if (is_reverse_ != is_reverse) {
			reverse(point_start, point_end);
		}
	}

	bool BounceHandle::is_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept {
		return point_.is_hovered(point, box_width, view);
	}

	bool BounceHandle::check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept {
		return point_.check_hover(point, box_width, view);
	}

	bool BounceHandle::update(
		const mkaul::Point<double>& point,
		const ControlPoint& point_start,
		const ControlPoint& point_end
	) noexcept {
		return point_.update(
			mkaul::Point{
				mkaul::clamp(point.x, point_start.x(), point_end.x()),
				mkaul::clamp(point.y, point_start.y(), point_end.y())
			}
		);
	}

	void BounceHandle::move(
		const mkaul::Point<double>& point,
		const ControlPoint& point_start,
		const ControlPoint& point_end
	) noexcept {
		point_.move(
			mkaul::Point{
				mkaul::clamp(point.x, point_start.x(), point_end.x()),
				mkaul::clamp(point.y, point_start.y(), point_end.y())
			}
		);
	}

	double BounceHandle::get_cor(const ControlPoint& point_start, const ControlPoint& point_end) const noexcept {
		double y = (point_.y() - point_start.y()) / (point_end.y() - point_start.y());
		if (is_reverse_) {
			y = 1. - y;
		}
		return std::sqrt(1. - mkaul::clamp(y, 0.001, 1.));
	}

	double BounceHandle::get_period(const ControlPoint& point_start, const ControlPoint& point_end) const noexcept {
		double x = (point_.x() - point_start.x()) / (point_end.x() - point_start.x());
		if (is_reverse_) {
			x = 1. - x;
		}
		return 2. * std::clamp(x, 0.001, 1.) / (get_cor(point_start, point_end) + 1.);
	}

	void BounceHandle::from_param(double cor, double period, const ControlPoint& point_start, const ControlPoint& point_end) noexcept {
		double x = mkaul::clamp(period * (cor + 1.) * 0.5, 0., 1.);
		double y = mkaul::clamp(1. - cor * cor, 0., 1.);
		if (is_reverse_) {
			x = 1. - x;
			y = 1. - y;
		}

		move(
			mkaul::Point{
				point_start.x() + x * (point_end.x() - point_start.x()),
				point_start.y() + y * (point_end.y() - point_start.y())
			}, point_start, point_end
		);
	}
	
	void BounceHandle::end_control() noexcept {
		point_.end_control();
	}
}