#pragma once

#include "curve_editor.hpp"
#include <mkaul/host_object.hpp>



namespace cved {
	class ScriptEditorHostObject : public mkaul::wv2::HostObject {
	public:
		ScriptEditorHostObject() {
			register_member(L"script", DispatchType::PropertyGet, +[] { return global::editor.editor_script().curve_script()->script(); });
			register_member(L"script", DispatchType::PropertyPut, +[](std::string script) { return global::editor.editor_script().curve_script()->set_script(script); });
		}
	};
} // namespace cved