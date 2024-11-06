#pragma once

#include "host_object_curve_graph.hpp"



namespace cved {
	class BounceCurveHostObject : public GraphCurveHostObject {
		static std::vector<double> get_handle(uint32_t id);
		static void set_handle(uint32_t id, double x, double y);
		static std::string get_param(uint32_t id);
		static uint32_t get_id(bool is_select_dialog);

	public:
		BounceCurveHostObject() {
			register_member(L"getId", DispatchType::Method, get_id);
			register_member(L"getHandle", DispatchType::Method, get_handle);
			register_member(L"moveHandle", DispatchType::Method, set_handle);
			register_member(L"getParam", DispatchType::Method, get_param);
		}
	};
} // namespace cved