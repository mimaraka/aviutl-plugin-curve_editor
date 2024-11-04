#include "curve_bezier.hpp"
#include "curve_editor.hpp"
#include "host_object_curve_bezier.hpp"



namespace cved {
	uint32_t BezierCurveHostObject::get_id(bool is_select_dialog) {
		static BezierCurve curve_tmp;
		uint32_t id;
		if (is_select_dialog) {
			id = curve_tmp.get_id();
		}
		else {
			id = global::editor.editor_graph().curve_bezier()->get_id();
		}
		return id;
	}

	double BezierCurveHostObject::get_handle_left_x(uint32_t id) {
		auto curve = global::id_manager.get_curve<BezierCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_left_x();
	}

	double BezierCurveHostObject::get_handle_left_y(uint32_t id) {
		auto curve = global::id_manager.get_curve<BezierCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_left_y();
	}

	void BezierCurveHostObject::begin_move_handle_left(uint32_t id, double scale_x, double scale_y) {
		auto curve = global::id_manager.get_curve<BezierCurve>(id);
		if (!curve) {
			return;
		}
		curve->begin_move_handle_left(scale_x, scale_y);
	}

	void BezierCurveHostObject::move_handle_left(uint32_t id, double x, double y, bool keep_angle) {
		auto curve = global::id_manager.get_curve<BezierCurve>(id);
		if (!curve) {
			return;
		}
		curve->move_handle_left(x, y, keep_angle);
	}

	double BezierCurveHostObject::get_handle_right_x(uint32_t id) {
		auto curve = global::id_manager.get_curve<BezierCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_right_x();
	}

	double BezierCurveHostObject::get_handle_right_y(uint32_t id) {
		auto curve = global::id_manager.get_curve<BezierCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_right_y();
	}

	void BezierCurveHostObject::begin_move_handle_right(uint32_t id, double scale_x, double scale_y) {
		auto curve = global::id_manager.get_curve<BezierCurve>(id);
		if (!curve) {
			return;
		}
		curve->begin_move_handle_right(scale_x, scale_y);
	}

	void BezierCurveHostObject::move_handle_right(uint32_t id, double x, double y, bool keep_angle) {
		auto curve = global::id_manager.get_curve<BezierCurve>(id);
		if (!curve) {
			return;
		}
		curve->move_handle_right(x, y, keep_angle);
	}

	std::string BezierCurveHostObject::get_param(uint32_t id) {
		auto curve = global::id_manager.get_curve<BezierCurve>(id);
		if (!curve) {
			return std::string("");
		}
		return curve->create_params_str();
	}
} // namespace cved