#include "control_point.hpp"
#include <algorithm>



namespace cved {
	bool ControlPoint::is_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		constexpr float BOX_WIDTH = 20.f;
		
		bool in_range_x = mkaul::in_range(
			pt.x, pt_.x - BOX_WIDTH / view.scale_x() * 0.5,
			pt_.x + BOX_WIDTH / view.scale_x() * 0.5, true
		);
		bool in_range_y = mkaul::in_range(
			pt.y, pt_.y - BOX_WIDTH / view.scale_y() * 0.5,
			pt_.y + BOX_WIDTH / view.scale_y() * 0.5, true
		);
		return in_range_x and in_range_y;
	}

	bool ControlPoint::check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		if (is_hovered(pt, view)) {
			controlled_ = true;
			return true;
		}
		else return false;
	}

	bool ControlPoint::update(const mkaul::Point<double>& pt) noexcept {
		if (controlled_) {
			move(pt);
			return true;
		}
		else return false;
	}

	void ControlPoint::move(const mkaul::Point<double>& pt) noexcept {
		if (!fixed_) {
			pt_ = pt;
		}
	}
}