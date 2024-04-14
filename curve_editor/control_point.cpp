#include "control_point.hpp"
#include <algorithm>



namespace cved {
	bool ControlPoint::is_hovered(const mkaul::Point<double>& point, const GraphView& view) const noexcept {
		constexpr float BOX_WIDTH = 20.f;
		
		bool in_range_x = mkaul::in_range(
			point.x, point_.x - BOX_WIDTH / view.scale_x() * 0.5,
			point_.x + BOX_WIDTH / view.scale_x() * 0.5, true
		);
		bool in_range_y = mkaul::in_range(
			point.y, point_.y - BOX_WIDTH / view.scale_y() * 0.5,
			point_.y + BOX_WIDTH / view.scale_y() * 0.5, true
		);
		return in_range_x and in_range_y;
	}

	bool ControlPoint::check_hover(const mkaul::Point<double>& point, const GraphView& view) noexcept {
		if (is_hovered(point, view)) {
			controlled_ = true;
			return true;
		}
		else return false;
	}

	bool ControlPoint::update(const mkaul::Point<double>& point) noexcept {
		if (controlled_) {
			move(point);
			return true;
		}
		else return false;
	}

	void ControlPoint::move(const mkaul::Point<double>& point) noexcept {
		if (!fixed_) {
			point_ = point;
		}
	}
}