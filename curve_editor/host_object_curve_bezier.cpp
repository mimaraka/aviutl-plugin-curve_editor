#include "host_object_curve_bezier.hpp"
#include "curve_bezier.hpp"



namespace cved {
	double BezierCurveHostObject::get_handle_left_x(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<BezierCurve>(curve_ptr);
			return curve->get_handle_left_x();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	double BezierCurveHostObject::get_handle_left_y(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<BezierCurve>(curve_ptr);
			return curve->get_handle_left_y();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	void BezierCurveHostObject::set_handle_left(uintptr_t curve_ptr, double x, double y, bool keep_angle) {
		try {
			auto curve = try_get<BezierCurve>(curve_ptr);
			curve->set_handle_left(x, y, keep_angle);
		}
		catch (const std::runtime_error&) {
			return;
		}
	}

	double BezierCurveHostObject::get_handle_right_x(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<BezierCurve>(curve_ptr);
			return curve->get_handle_right_x();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	double BezierCurveHostObject::get_handle_right_y(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<BezierCurve>(curve_ptr);
			return curve->get_handle_right_y();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	void BezierCurveHostObject::set_handle_right(uintptr_t curve_ptr, double x, double y, bool keep_angle) {
		try {
			auto curve = try_get<BezierCurve>(curve_ptr);
			curve->set_handle_right(x, y, keep_angle);
		}
		catch (const std::runtime_error&) {
			return;
		}
	}
} // namespace cved