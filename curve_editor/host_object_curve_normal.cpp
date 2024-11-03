#include "curve_normal.hpp"
#include "host_object_curve_normal.hpp"



namespace cved {
	std::vector<uint32_t> NormalCurveHostObject::get_id_array(uint32_t id) {
		auto curve = global::id_manager.get_curve<NormalCurve>(id);
		if (!curve) {
			return std::vector<uint32_t>();
		}
		std::vector<uint32_t> id_array;
		for (size_t i = 0; i < curve->segment_n(); i++) {
			id_array.emplace_back(curve->get_segment_id(i));
		}
		return id_array;
	}

	void NormalCurveHostObject::add_curve(uint32_t id, double x, double y, double scale_x) {
		auto curve = global::id_manager.get_curve<NormalCurve>(id);
		if (!curve) {
			return;
		}
		curve->add_curve(mkaul::Point{ x, y }, scale_x);
	}

	void NormalCurveHostObject::delete_curve(uint32_t id, uint32_t segment_id) {
		auto curve = global::id_manager.get_curve<NormalCurve>(id);
		if (!curve) {
			return;
		}
		curve->delete_curve(segment_id);
	}
} // namespace cved