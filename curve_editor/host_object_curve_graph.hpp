#pragma once

#include <stdexcept>
#include <mkaul/host_object.hpp>



namespace cved {
	class GraphCurveHostObject : public mkaul::wv2::HostObject {
		static double get_anchor_start_x(uintptr_t curve_ptr);
		static double get_anchor_start_y(uintptr_t curve_ptr);
		static void set_anchor_start(uintptr_t curve_ptr, double x, double y);
		static double get_anchor_end_x(uintptr_t curve_ptr);
		static double get_anchor_end_y(uintptr_t curve_ptr);
		static void set_anchor_end(uintptr_t curve_ptr, double x, double y);
		static uintptr_t get_prev_curve_ptr(uintptr_t curve_ptr);
		static uintptr_t get_next_curve_ptr(uintptr_t curve_ptr);

	protected:
		template <typename CurveType>
		static CurveType* try_get(uintptr_t curve_ptr) {
			auto curve = reinterpret_cast<CurveType*>(curve_ptr);
			if (!curve or typeid(*curve) != typeid(CurveType)) {
				throw std::runtime_error("Invalid Pointer.");
			}
			return curve;
		}

	public:
		GraphCurveHostObject() {
			register_member(L"getAnchorStartX", DispatchType::Method, get_anchor_start_x);
			register_member(L"getAnchorStartY", DispatchType::Method, get_anchor_start_y);
			register_member(L"setAnchorStart", DispatchType::Method, set_anchor_start);
			register_member(L"getAnchorEndX", DispatchType::Method, get_anchor_end_x);
			register_member(L"getAnchorEndY", DispatchType::Method, get_anchor_end_y);
			register_member(L"setAnchorEnd", DispatchType::Method, set_anchor_end);
			register_member(L"getPrevCurvePtr", DispatchType::Method, get_prev_curve_ptr);
			register_member(L"getNextCurvePtr", DispatchType::Method, get_next_curve_ptr);
		}
	};
} // namespace cved