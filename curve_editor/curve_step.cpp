#include "curve_step.hpp"



namespace cved {
	double StepCurve::get_value(double progress, double start, double end) const noexcept {
		return 0.f;
	}

	void StepCurve::clear() noexcept {

	}

	void StepCurve::reverse() noexcept {

	}

	int StepCurve::encode() const noexcept {
		return 0;
	}

	bool StepCurve::decode(int number) noexcept {
		return true;
	}

	void StepCurve::draw_handle(
		mkaul::graphics::Graphics* p_graphics,
		const View& view,
		float thickness,
		float root_radius,
		float tip_radius,
		float tip_thickness,
		bool cutoff_line,
		const mkaul::ColorF& color
	) const noexcept {

	}

	bool StepCurve::is_handle_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept {
		return false;
	}

	bool StepCurve::handle_check_hover(
		const mkaul::Point<double>& point,
		float box_width,
		const GraphView& view
	) noexcept {
		return false;
	}

	bool StepCurve::handle_update(
		const mkaul::Point<double>& point,
		const GraphView& view
	) noexcept {
		return false;
	}

	void StepCurve::handle_end_control() noexcept {

	}

	StepCurve::ActivePoint StepCurve::point_check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept {
		return ActivePoint::Null;
	}

	bool StepCurve::point_begin_move(ActivePoint active_point, const GraphView& view) noexcept {
		return true;
	}

	StepCurve::ActivePoint StepCurve::point_update(const mkaul::Point<double>& point, const GraphView& view) noexcept {
		return ActivePoint::Null;
	}

	bool StepCurve::point_move(ActivePoint active_point, const mkaul::Point<double>& point, const GraphView& view) noexcept {
		return true;
	}

	void StepCurve::point_end_move() noexcept {

	}

	void StepCurve::point_end_control() noexcept {

	}
}