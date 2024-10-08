#include "host_object_grapheditor.hpp"
#include <strconv2.h>
#include "config.hpp"



namespace cved {
	std::vector<double> GraphEditorHostObject::get_curve_value_array(EditMode mode, double start_x, double start_y, double end_x, double end_y, size_t n) {
		std::vector<double> result;
		for (size_t i = 0u; i < n; i++) {
			double x = start_x + (end_x - start_x) * i / (n - 1);
			double y = global::editor.editor_graph().get_curve(mode)->get_value(x, start_y, end_y);
			result.emplace_back(y);
		}
		return result;
	}

	std::wstring GraphEditorHostObject::get_curve_type(uintptr_t curve_ptr) {
		auto curve = reinterpret_cast<GraphCurve*>(curve_ptr);
		if (!curve) {
			return std::wstring();
		}
		return ::sjis_to_wide(curve->get_type());
	}
} // namespace cved