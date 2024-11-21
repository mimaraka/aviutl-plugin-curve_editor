#pragma once

#include "host_object_editor_graph.hpp"
#include "host_object_editor_script.hpp"
#include <mkaul/host_object.hpp>



namespace curve_editor {
	class EditorHostObject : public mkaul::ole::HostObject {
		static std::wstring get_curve_name(uint32_t id);
		static std::wstring get_curve_disp_name(uint32_t id);
		static bool is_curve_locked(uint32_t id);
		static void set_is_curve_locked(uint32_t id, bool locked);

	public:
		EditorHostObject() {
			register_member(L"currentCurveId", DispatchType::PropertyGet, +[]() { return global::editor.current_curve()->get_id(); });
			register_member(L"getCurveName", DispatchType::Method, get_curve_name);
			register_member(L"getCurveDispName", DispatchType::Method, get_curve_disp_name);
			register_member(L"isCurveLocked", DispatchType::Method, is_curve_locked);
			register_member(L"setIsCurveLocked", DispatchType::Method, set_is_curve_locked);
			register_member(L"graph", DispatchType::PropertyGet, create_host_object<GraphEditorHostObject>);
			register_member(L"script", DispatchType::PropertyGet, create_host_object<ScriptEditorHostObject>);
		}
	};
} // namespace curve_editor