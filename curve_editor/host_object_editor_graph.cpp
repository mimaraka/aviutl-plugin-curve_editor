#include "config.hpp"
#include "host_object_editor_graph.hpp"
#include <strconv2.h>



namespace curve_editor {
	std::vector<double> GraphEditorHostObject::get_curve_value_array(uint32_t id, double start_x, double start_y, double end_x, double end_y, size_t n) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return std::vector<double>();
		}
		std::vector<double> result;
		for (size_t i = 0u; i < n; i++) {
			double x = start_x + (end_x - start_x) * i / (n - 1);
			double y = curve->get_value(x, start_y, end_y);
			result.emplace_back(y);
		}
		return result;
	}

	std::vector<double> GraphEditorHostObject::get_curve_velocity_array(uint32_t id, double start_x, double start_y, double end_x, double end_y, size_t n) {
		auto curve = global::id_manager.get_curve<GraphCurve>(id);
		if (!curve) {
			return std::vector<double>();
		}
		std::vector<double> result;
		for (size_t i = 0u; i < n; i++) {
			double x = start_x + (end_x - start_x) * i / (n - 1);
			double y = curve->get_velocity(x, start_y, end_y);
			result.emplace_back(y);
		}
		return result;
	}
} // namespace curve_editor