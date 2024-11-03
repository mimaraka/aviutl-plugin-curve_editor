#include "curve_bounce.hpp"
#include "curve_editor.hpp"
#include "host_object_curve_bounce.hpp"



namespace cved {
	uint32_t BounceCurveHostObject::get_id(bool is_select_dialog) {
		static BounceCurve curve_tmp;
		uint32_t id;
		if (is_select_dialog) {
			id = curve_tmp.get_id();
		}
		else {
			id = global::editor.editor_graph().curve_bounce()->get_id();
		}
		return id;
	}

	double BounceCurveHostObject::get_handle_x(uint32_t id) {
		auto curve = global::id_manager.get_curve<BounceCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_x();
	}

	double BounceCurveHostObject::get_handle_y(uint32_t id) {
		auto curve = global::id_manager.get_curve<BounceCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_y();
	}

	void BounceCurveHostObject::set_handle(uint32_t id, double x, double y) {
		auto curve = global::id_manager.get_curve<BounceCurve>(id);
		if (!curve) {
			return;
		}
		curve->set_handle(x, y);
	}

	std::string BounceCurveHostObject::get_param(uint32_t id) {
		auto curve = global::id_manager.get_curve<BounceCurve>(id);
		if (!curve) {
			return "";
		}
		return curve->create_params_str();
	}
} // namespace cved