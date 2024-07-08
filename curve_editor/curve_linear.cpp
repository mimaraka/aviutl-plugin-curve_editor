#include "curve_linear.hpp"
#include "enum.hpp"



namespace cved {
	double LinearCurve::curve_function(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - pt_start().x()) / (pt_end().x() - pt_start().x()), 0., 1.);
		return start + (end - start) * (pt_start().y() + (pt_end().y() - pt_start().y()) * progress);
	}

	LinearCurve::ActivePoint LinearCurve::pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		if (pt_start_.check_hover(pt, view)) return ActivePoint::Start;
		else if (pt_end_.check_hover(pt, view)) return ActivePoint::End;
		else return ActivePoint::Null;
	}

	LinearCurve::ActivePoint LinearCurve::pt_update(const mkaul::Point<double>& pt, const GraphView&) noexcept {
		if (pt_start_.update(mkaul::Point{ std::min(pt.x, pt_end().x()), pt.y })) return ActivePoint::Start;
		else if (pt_end_.update(mkaul::Point{ std::max(pt.x, pt_start().x()), pt.y })) return ActivePoint::End;
		else return ActivePoint::Null;
	}

	void LinearCurve::pt_end_control() noexcept {
		pt_start_.end_control();
		pt_end_.end_control();
	}
}