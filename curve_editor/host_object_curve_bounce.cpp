#include "curve_bounce.hpp"
#include "host_object_curve_bounce.hpp"



namespace cved {
	double BounceCurveHostObject::get_handle_x(uint32_t curve_ptr) {
		try {
			auto curve = try_get<BounceCurve>(curve_ptr);
			return curve->get_handle_x();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	double BounceCurveHostObject::get_handle_y(uint32_t curve_ptr) {
		try {
			auto curve = try_get<BounceCurve>(curve_ptr);
			return curve->get_handle_y();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	void BounceCurveHostObject::set_handle(uint32_t curve_ptr, double x, double y) {
		try {
			auto curve = try_get<BounceCurve>(curve_ptr);
			curve->set_handle(x, y);
		}
		catch (const std::runtime_error&) {
			return;
		}
	}

	std::wstring BounceCurveHostObject::get_param(uint32_t curve_ptr) {
		try {
			auto curve = try_get<BounceCurve>(curve_ptr);
			return std::format(L"{:.2f}, {:.2f}", curve->get_cor(), curve->get_period());
		}
		catch (const std::runtime_error&) {
			return L"";
		}
	}
} // namespace cved