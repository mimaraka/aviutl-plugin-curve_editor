#pragma once

#include "host_object_curve_graph.hpp"



namespace cved {
	class BounceCurveHostObject : public GraphCurveHostObject {
		static double get_handle_x(uint32_t curve_ptr);
		static double get_handle_y(uint32_t curve_ptr);
		static void set_handle(uint32_t curve_ptr, double x, double y);
		static std::wstring get_param(uint32_t curve_ptr);

	public:
		BounceCurveHostObject() {
			register_member(L"getHandleX", DispatchType::Method, get_handle_x);
			register_member(L"getHandleY", DispatchType::Method, get_handle_y);
			register_member(L"setHandle", DispatchType::Method, set_handle);
			register_member(L"getParam", DispatchType::Method, get_param);
		}
	};
} // namespace cved