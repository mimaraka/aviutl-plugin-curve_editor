#include "host_object_curve_normal.hpp"
#include "curve_normal.hpp"



namespace cved {
	std::vector<uintptr_t> NormalCurveHostObject::get_ptr_array(uintptr_t curve_ptr) {
		try{
			auto curve = try_get<NormalCurve>(curve_ptr);
			std::vector<uintptr_t> ptr_array;
			for (size_t i = 0; i < curve->segment_n(); i++) {
				ptr_array.emplace_back(reinterpret_cast<uintptr_t>(curve->get_segment(i)));
			}
			return ptr_array;
		}
		catch (const std::runtime_error&) {
			return std::vector<uintptr_t>();
		}
	}

	void NormalCurveHostObject::add_curve(uintptr_t curve_ptr, double x, double y, double scale_x) {
		try {
			auto curve = try_get<NormalCurve>(curve_ptr);
			curve->add_curve(mkaul::Point{ x, y }, scale_x);
		}
		catch (const std::runtime_error&) {}
	}

	void NormalCurveHostObject::delete_curve(uintptr_t curve_ptr, uintptr_t segment_ptr) {
		try {
			auto curve = try_get<NormalCurve>(curve_ptr);
			auto segment = reinterpret_cast<GraphCurve*>(segment_ptr);
			curve->delete_curve(segment);
		}
		catch (const std::runtime_error&) {}
	}
} // namespace cved