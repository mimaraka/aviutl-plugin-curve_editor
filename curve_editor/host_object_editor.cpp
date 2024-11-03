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
} // namespace cved