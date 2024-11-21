#include "curve_graph.hpp"
#include "host_object_curve_graph.hpp"



namespace curve_editor {
	std::vector<double> GraphCurveHostObject::get_anchor_start(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) return {};
		return { curve->anchor_start().x, curve->anchor_start().y};
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

	void GraphCurveHostObject::end_move_anchor_start(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return;
		}
		curve->end_move_anchor_start();
	}

	std::vector<double> GraphCurveHostObject::get_anchor_end(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return {};
		}
		return { curve->anchor_end().x, curve->anchor_end().y };
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

	void GraphCurveHostObject::end_move_anchor_end(uint32_t id) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return;
		}
		curve->end_move_anchor_end();
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
} // namespace curve_editor