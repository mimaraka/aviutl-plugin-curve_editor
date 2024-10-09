#include "curve_graph.hpp"
#include "host_object_curve_graph.hpp"



namespace cved {
	double GraphCurveHostObject::get_anchor_start_x(uintptr_t curve_ptr) {
		auto curve = reinterpret_cast<GraphCurve*>(curve_ptr);
		if (!curve) {
			return 0.;
		}
		return curve->get_anchor_start_x();
	}

	double GraphCurveHostObject::get_anchor_start_y(uintptr_t curve_ptr) {
		auto curve = reinterpret_cast<GraphCurve*>(curve_ptr);
		if (!curve) {
			return 0.;
		}
		return curve->get_anchor_start_y();
	}

	void GraphCurveHostObject::set_anchor_start(uintptr_t curve_ptr, double x, double y) {
		auto curve = reinterpret_cast<GraphCurve*>(curve_ptr);
		if (!curve) {
			return;
		}
		curve->set_anchor_start(x, y);
	}

	double GraphCurveHostObject::get_anchor_end_x(uintptr_t curve_ptr) {
		auto curve = reinterpret_cast<GraphCurve*>(curve_ptr);
		if (!curve) {
			return 0.;
		}
		return curve->get_anchor_end_x();
	}

	double GraphCurveHostObject::get_anchor_end_y(uintptr_t curve_ptr) {
		auto curve = reinterpret_cast<GraphCurve*>(curve_ptr);
		if (!curve) {
			return 0.;
		}
		return curve->get_anchor_end_y();
	}

	void GraphCurveHostObject::set_anchor_end(uintptr_t curve_ptr, double x, double y) {
		auto curve = reinterpret_cast<GraphCurve*>(curve_ptr);
		if (!curve) {
			return;
		}
		curve->set_anchor_end(x, y);
	}

	uintptr_t GraphCurveHostObject::get_prev_curve_ptr(uintptr_t curve_ptr) {
		auto curve = reinterpret_cast<GraphCurve*>(curve_ptr);
		if (!curve) {
			return 0;
		}
		return reinterpret_cast<uintptr_t>(curve->prev());
	}

	uintptr_t GraphCurveHostObject::get_next_curve_ptr(uintptr_t curve_ptr) {
		auto curve = reinterpret_cast<GraphCurve*>(curve_ptr);
		if (!curve) {
			return 0;
		}
		return reinterpret_cast<uintptr_t>(curve->next());
	}
} // namespace cved