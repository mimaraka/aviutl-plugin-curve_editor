#pragma once

#include "curve_editor.hpp"
#include <mkaul/host_object.hpp>



namespace cved {
	class ScriptEditorHostObject : public mkaul::wv2::HostObject {
	public:
		ScriptEditorHostObject() {
			register_member(L"getId", DispatchType::Method, +[](size_t idx) {
				return global::editor.editor_script().curve_script(idx)->get_id();
			});
			register_member(L"script", DispatchType::PropertyGet, +[] { return global::editor.editor_script().curve_script()->script(); });
			register_member(L"script", DispatchType::PropertyPut, +[](std::string script) { return global::editor.editor_script().curve_script()->set_script(script); });
			register_member(L"idx", DispatchType::PropertyGet, +[] { return global::editor.editor_script().idx(); });
			register_member(L"idx", DispatchType::PropertyPut, +[](int n) { return global::editor.editor_script().set_idx(n); });
			register_member(L"size", DispatchType::PropertyGet, +[] { return global::editor.editor_script().size(); });
			register_member(L"appendIdx", DispatchType::Method, +[] { global::editor.editor_script().append_idx(); });
			register_member(L"popIdx", DispatchType::Method, +[] { global::editor.editor_script().pop_idx(); });
			register_member(L"isIdxMax", DispatchType::PropertyGet, +[] { return global::editor.editor_script().is_idx_max(); });
		}
	};
} // namespace cved