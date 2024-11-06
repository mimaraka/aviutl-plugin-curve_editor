#include "host_object_editor.hpp"
#include <strconv2.h>



namespace cved {
	std::wstring EditorHostObject::get_curve_name(uint32_t id) {
		auto curve = global::id_manager.get_curve<Curve>(id);
		if (!curve) {
			return std::wstring();
		}
		return ::sjis_to_wide(curve->get_name());
	}

	std::wstring EditorHostObject::get_curve_disp_name(uint32_t id) {
		auto curve = global::id_manager.get_curve<Curve>(id);
		if (!curve) {
			return std::wstring();
		}
		return ::sjis_to_wide(curve->get_disp_name());
	}

	bool EditorHostObject::is_curve_locked(uint32_t id) {
		auto curve = global::id_manager.get_curve<Curve>(id);
		if (!curve) {
			return false;
		}
		return curve->is_locked();
	}

	void EditorHostObject::set_is_curve_locked(uint32_t id, bool locked) {
		auto curve = global::id_manager.get_curve<Curve>(id);
		if (!curve) {
			return;
		}
		curve->set_locked(locked);
	}
} // namespace cved