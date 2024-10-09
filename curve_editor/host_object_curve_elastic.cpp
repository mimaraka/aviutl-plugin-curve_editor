#include "curve_elastic.hpp"
#include "host_object_curve_elastic.hpp"



namespace cved {
	double ElasticCurveHostObject::get_handle_amp_left_x(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			return curve->get_handle_amp_left_x();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	double ElasticCurveHostObject::get_handle_amp_left_y(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			return curve->get_handle_amp_left_y();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	void ElasticCurveHostObject::set_handle_amp_left(uintptr_t curve_ptr, double y) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			curve->set_handle_amp_left(y);
		}
		catch (const std::runtime_error&) {
			return;
		}
	}

	double ElasticCurveHostObject::get_handle_amp_right_x(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			return curve->get_handle_amp_right_x();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	double ElasticCurveHostObject::get_handle_amp_right_y(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			return curve->get_handle_amp_right_y();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	void ElasticCurveHostObject::set_handle_amp_right(uintptr_t curve_ptr, double y) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			curve->set_handle_amp_right(y);
		}
		catch (const std::runtime_error&) {
			return;
		}
	}

	double ElasticCurveHostObject::get_handle_freq_decay_x(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			return curve->get_handle_freq_decay_x();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	double ElasticCurveHostObject::get_handle_freq_decay_y(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			return curve->get_handle_freq_decay_y();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	double ElasticCurveHostObject::get_handle_freq_decay_root_y(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			return curve->get_handle_freq_decay_root_y();
		}
		catch (const std::runtime_error&) {
			return 0.;
		}
	}

	void ElasticCurveHostObject::set_handle_freq_decay(uintptr_t curve_ptr, double x, double y) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			curve->set_handle_freq_decay(x, y);
		}
		catch (const std::runtime_error&) {
			return;
		}
	}

	std::wstring ElasticCurveHostObject::get_param(uintptr_t curve_ptr) {
		try {
			auto curve = try_get<ElasticCurve>(curve_ptr);
			return std::format(L"{:.2f}, {:.2f}, {:.2f}", curve->get_amp(), curve->get_freq(), curve->get_decay());
		}
		catch (const std::runtime_error&) {
			return L"";
		}
	}
} // namespace cved