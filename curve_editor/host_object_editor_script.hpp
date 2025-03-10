#pragma once

#include "curve_editor.hpp"
#include <mkaul/host_object.hpp>



namespace curve_editor {
	class ScriptEditorHostObject : public mkaul::ole::HostObject {
		static const std::wstring& get_script(uint32_t id);
		static void set_script(uint32_t id, std::wstring script);

	public:
		ScriptEditorHostObject() {
			register_member(L"getId", DispatchType::Method, +[](size_t idx) {
				auto curve = global::editor.editor_script().p_curve_script(idx);
				return curve ? curve->get_id() : 0;
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
} // namespace curve_editor