#pragma once

#include "curve_graph.hpp"
#include <mkaul/host_object.hpp>
#include <stdexcept>



namespace cved {
	class GraphCurveHostObject : public mkaul::wv2::HostObject {
		static double get_anchor_start_x(uint32_t id);
		static double get_anchor_start_y(uint32_t id);
		static void set_anchor_start(uint32_t id, double x, double y);
		static double get_anchor_end_x(uint32_t id);
		static double get_anchor_end_y(uint32_t id);
		static void set_anchor_end(uint32_t id, double x, double y);
		static uint32_t get_prev_curve_id(uint32_t id);
		static uint32_t get_next_curve_id(uint32_t id);

	public:
		GraphCurveHostObject() {
			register_member(L"getAnchorStartX", DispatchType::Method, get_anchor_start_x);
			register_member(L"getAnchorStartY", DispatchType::Method, get_anchor_start_y);
			register_member(L"setAnchorStart", DispatchType::Method, set_anchor_start);
			register_member(L"getAnchorEndX", DispatchType::Method, get_anchor_end_x);
			register_member(L"getAnchorEndY", DispatchType::Method, get_anchor_end_y);
			register_member(L"setAnchorEnd", DispatchType::Method, set_anchor_end);
			register_member(L"getPrevCurveId", DispatchType::Method, get_prev_curve_id);
			register_member(L"getNextCurveId", DispatchType::Method, get_next_curve_id);
			register_member(L"reverse", DispatchType::Method, +[](uint32_t id) {
				auto curve = global::id_manager.get_curve<GraphCurve>(id);
				if (curve) {
					curve->reverse();
				}
			});
		}
	};
} // namespace cved