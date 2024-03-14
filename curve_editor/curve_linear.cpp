#include "curve_linear.hpp"



namespace cved {
	double LinearCurve::get_value(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - point_start_.x()) / (point_end_.x() - point_start_.x()), 0., 1.);
		return start + (end - start) * (point_start_.y() + (point_end_.y() - point_start_.y()) * progress);
	}

	LinearCurve::ActivePoint LinearCurve::point_check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept {
		if (point_start_.check_hover(point, box_width, view)) return ActivePoint::Start;
		else if (point_end_.check_hover(point, box_width, view)) return ActivePoint::End;
		else return ActivePoint::Null;
	}

	bool LinearCurve::point_begin_move(ActivePoint active_point, const GraphView&) noexcept {
		return true;
	}

	LinearCurve::ActivePoint LinearCurve::point_update(const mkaul::Point<double>& point, const GraphView&) noexcept {
		if (point_start_.update(mkaul::Point{ std::min(point.x, point_end_.x()), point.y })) return ActivePoint::Start;
		else if (point_end_.update(mkaul::Point{ std::max(point.x, point_start_.x()), point.y })) return ActivePoint::End;
		else return ActivePoint::Null;
	}

	void LinearCurve::point_end_control() noexcept {
		point_start_.end_control();
		point_end_.end_control();
	}
}