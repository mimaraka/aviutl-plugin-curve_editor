#pragma once

#include "curve_editor.hpp"
#include "host_object_curve_graph.hpp"



namespace cved {
	class NormalCurveHostObject : public GraphCurveHostObject {
		static std::vector<uint32_t> get_id_array(uint32_t id);
		static void add_curve(uint32_t id, double x, double y, double scale_x);
		static void delete_curve(uint32_t id, uint32_t segment_id);

	public:
		NormalCurveHostObject() {
			register_member(L"getId", DispatchType::Method, +[](size_t idx) {
				auto curve = global::editor.editor_graph().curve_normal(idx);
				return curve ? curve->get_id() : 0;
			});
			register_member(L"getIdArray", DispatchType::Method, get_id_array);
			register_member(L"addCurve", DispatchType::Method, add_curve);
			register_member(L"deleteCurve", DispatchType::Method, delete_curve);
			register_member(L"idx", DispatchType::PropertyGet, +[] { return global::editor.editor_graph().idx_normal(); });
			register_member(L"idx", DispatchType::PropertyPut, +[](int n) { return global::editor.editor_graph().set_idx_normal(n); });
			register_member(L"size", DispatchType::PropertyGet, +[] { return global::editor.editor_graph().size_normal(); });
			register_member(L"appendIdx", DispatchType::Method, +[] { global::editor.editor_graph().append_idx_normal(); });
			register_member(L"popIdx", DispatchType::Method, +[] { global::editor.editor_graph().pop_idx_normal(); });
			register_member(L"isIdxMax", DispatchType::PropertyGet, +[] { return global::editor.editor_graph().is_idx_max_normal(); });
		}
	};
} // namespace cved