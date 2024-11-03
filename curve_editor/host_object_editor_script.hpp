#pragma once

#include "curve_editor.hpp"
#include <mkaul/host_object.hpp>



namespace cved {
	class ScriptEditorHostObject : public mkaul::ole::HostObject {
		static const std::string& get_script(uint32_t id);
		static void set_script(uint32_t id, std::string script);

	public:
		ScriptEditorHostObject() {
			register_member(L"getId", DispatchType::Method, +[](size_t idx) {
				return global::editor.editor_script().curve_script(idx)->get_id();
			});
			register_member(L"getScript", DispatchType::Method, get_script);
			register_member(L"setScript", DispatchType::Method, set_script);
			register_member(L"idx", DispatchType::PropertyGet, +[] { return global::editor.editor_script().idx(); });
			register_member(L"idx", DispatchType::PropertyPut, +[](int n) { return global::editor.editor_script().set_idx(n); });
			register_member(L"size", DispatchType::PropertyGet, +[] { return global::editor.editor_script().size(); });
			register_member(L"appendIdx", DispatchType::Method, +[] { global::editor.editor_script().append_idx(); });
			register_member(L"popIdx", DispatchType::Method, +[] { global::editor.editor_script().pop_idx(); });
			register_member(L"isIdxMax", DispatchType::PropertyGet, +[] { return global::editor.editor_script().is_idx_max(); });
		}
	};
} // namespace cved