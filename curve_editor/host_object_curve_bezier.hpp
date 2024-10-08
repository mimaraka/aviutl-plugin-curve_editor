#pragma once

#include "host_object_curve_graph.hpp"



namespace cved {
	class BezierCurveHostObject : public GraphCurveHostObject {
		static double get_handle_left_x(uintptr_t curve_ptr);
		static double get_handle_left_y(uintptr_t curve_ptr);
		static void set_handle_left(uintptr_t curve_ptr, double x, double y, bool keep_angle);
		static double get_handle_right_x(uintptr_t curve_ptr);
		static double get_handle_right_y(uintptr_t curve_ptr);
		static void set_handle_right(uintptr_t curve_ptr, double x, double y, bool keep_angle);

	public:
		BezierCurveHostObject() {
			register_member(L"getHandleLeftX", DispatchType::Method, get_handle_left_x);
			register_member(L"getHandleLeftY", DispatchType::Method, get_handle_left_y);
			register_member(L"setHandleLeft", DispatchType::Method, set_handle_left);
			register_member(L"getHandleRightX", DispatchType::Method, get_handle_right_x);
			register_member(L"getHandleRightY", DispatchType::Method, get_handle_right_y);
			register_member(L"setHandleRight", DispatchType::Method, set_handle_right);
		}
	};
} // namespace cved