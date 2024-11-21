#include "curve_id_manager.hpp"
#include "host_object_editor_script.hpp"



namespace curve_editor {
	const std::string& ScriptEditorHostObject::get_script(uint32_t id) {
		auto curve = global::id_manager.get_curve<ScriptCurve>(id);
		if (!curve) {
			static std::string empty;
			return empty;
		}
		return curve->script();
	}

	void ScriptEditorHostObject::set_script(uint32_t id, std::string script) {
		auto curve = global::id_manager.get_curve<ScriptCurve>(id);
		if (!curve) {
			return;
		}
		curve->set_script(script);
	}
} // namespace curve_editor