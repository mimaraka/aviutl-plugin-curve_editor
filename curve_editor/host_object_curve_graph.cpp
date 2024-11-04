#include "curve_graph.hpp"
#include "host_object_curve_graph.hpp"



namespace cved {
	double GraphCurveHostObject::get_anchor_start_x(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) return 0.;
		return curve->get_anchor_start_x();
	}

	double GraphCurveHostObject::get_anchor_start_y(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_anchor_start_y();
	}

	void GraphCurveHostObject::begin_move_anchor_start(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return;
		}
		curve->begin_move_anchor_start();
	}

	void GraphCurveHostObject::move_anchor_start(uint32_t id, double x, double y) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return;
		}
		curve->move_anchor_start(x, y);
	}

	double GraphCurveHostObject::get_anchor_end_x(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_anchor_end_x();
	}

	double GraphCurveHostObject::get_anchor_end_y(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_anchor_end_y();
	}

	void GraphCurveHostObject::begin_move_anchor_end(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return;
		}
		curve->begin_move_anchor_end();
	}

	void GraphCurveHostObject::move_anchor_end(uint32_t id, double x, double y) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return;
		}
		curve->move_anchor_end(x, y);
	}

	uint32_t GraphCurveHostObject::get_prev_curve_id(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve or !curve->prev()) {
			return 0;
		}
		return curve->prev()->get_id();
	}

	uint32_t GraphCurveHostObject::get_next_curve_id(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve or !curve->next()) {
			return 0;
		}
		return curve->next()->get_id();
	}
} // namespace cved