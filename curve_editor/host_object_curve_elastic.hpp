#pragma once

#include "host_object_curve_graph.hpp"



namespace cved {
	class ElasticCurveHostObject : public GraphCurveHostObject {
		static double get_handle_amp_left_x(uint32_t id);
		static double get_handle_amp_left_y(uint32_t id);
		static void set_handle_amp_left(uint32_t id, double y);
		static double get_handle_amp_right_x(uint32_t id);
		static double get_handle_amp_right_y(uint32_t id);
		static void set_handle_amp_right(uint32_t id, double y);
		static double get_handle_freq_decay_x(uint32_t id);
		static double get_handle_freq_decay_y(uint32_t id);
		static double get_handle_freq_decay_root_y(uint32_t id);
		static void set_handle_freq_decay(uint32_t id, double x, double y);
		static std::wstring get_param(uint32_t id);
		static uint32_t get_id(bool is_select_dialog);

	public:
		ElasticCurveHostObject() {
			register_member(L"getId", DispatchType::Method, get_id);
			register_member(L"getHandleAmpLeftX", DispatchType::Method, get_handle_amp_left_x);
			register_member(L"getHandleAmpLeftY", DispatchType::Method, get_handle_amp_left_y);
			register_member(L"setHandleAmpLeft", DispatchType::Method, set_handle_amp_left);
			register_member(L"getHandleAmpRightX", DispatchType::Method, get_handle_amp_right_x);
			register_member(L"getHandleAmpRightY", DispatchType::Method, get_handle_amp_right_y);
			register_member(L"setHandleAmpRight", DispatchType::Method, set_handle_amp_right);
			register_member(L"getHandleFreqDecayX", DispatchType::Method, get_handle_freq_decay_x);
			register_member(L"getHandleFreqDecayY", DispatchType::Method, get_handle_freq_decay_y);
			register_member(L"getHandleFreqDecayRootY", DispatchType::Method, get_handle_freq_decay_root_y);
			register_member(L"setHandleFreqDecay", DispatchType::Method, set_handle_freq_decay);
			register_member(L"getParam", DispatchType::Method, get_param);
		}
	};
} // namespace cved