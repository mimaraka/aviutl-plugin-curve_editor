#pragma once

#include "host_object_curve_graph.hpp"



namespace cved {
	class NormalCurveHostObject : public GraphCurveHostObject {
		static std::vector<uintptr_t> get_ptr_array(uintptr_t curve_ptr);
		static void add_curve(uintptr_t curve_ptr, double x, double y, double scale_x);
		static void delete_curve(uintptr_t curve_ptr, uintptr_t segment_ptr);

	public:
		NormalCurveHostObject() {
			register_member(L"getPtrArray", DispatchType::Method, get_ptr_array);
			register_member(L"addCurve", DispatchType::Method, add_curve);
			register_member(L"deleteCurve", DispatchType::Method, delete_curve);
		}
	};
} // namespace cved