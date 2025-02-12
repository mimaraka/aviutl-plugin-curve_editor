#pragma once

#include "curve_graph.hpp"
#include <mkaul/host_object.hpp>
#include <stdexcept>



namespace curve_editor {
	class GraphCurveHostObject : public mkaul::ole::HostObject {
		static std::vector<double> get_anchor_start(uint32_t id);
		static void begin_move_anchor_start(uint32_t id);
		static void move_anchor_start(uint32_t id, double x, double y);
		static void end_move_anchor_start(uint32_t id);
		static std::vector<double> get_anchor_end(uint32_t id);
		static void begin_move_anchor_end(uint32_t id);
		static void move_anchor_end(uint32_t id, double x, double y);
		static void end_move_anchor_end(uint32_t id);
		static uint32_t get_prev_curve_id(uint32_t id);
		static uint32_t get_next_curve_id(uint32_t id);

	public:
		GraphCurveHostObject() {
			register_member(L"getAnchorStart", DispatchType::Method, get_anchor_start);
			register_member(L"beginMoveAnchorStart", DispatchType::Method, begin_move_anchor_start);
			register_member(L"moveAnchorStart", DispatchType::Method, move_anchor_start);
			register_member(L"endMoveAnchorStart", DispatchType::Method, end_move_anchor_start);
			register_member(L"getAnchorEnd", DispatchType::Method, get_anchor_end);
			register_member(L"beginMoveAnchorEnd", DispatchType::Method, begin_move_anchor_end);
			register_member(L"moveAnchorEnd", DispatchType::Method, move_anchor_end);
			register_member(L"endMoveAnchorEnd", DispatchType::Method, end_move_anchor_end);
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
} // namespace curve_editor