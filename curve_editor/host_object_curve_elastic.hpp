#pragma once

#include "host_object_curve_graph.hpp"



namespace cved {
	class ElasticCurveHostObject : public GraphCurveHostObject {
		static double get_handle_amp_left_x(uintptr_t curve_ptr);
		static double get_handle_amp_left_y(uintptr_t curve_ptr);
		static void set_handle_amp_left(uintptr_t curve_ptr, double y);
		static double get_handle_amp_right_x(uintptr_t curve_ptr);
		static double get_handle_amp_right_y(uintptr_t curve_ptr);
		static void set_handle_amp_right(uintptr_t curve_ptr, double y);
		static double get_handle_freq_decay_x(uintptr_t curve_ptr);
		static double get_handle_freq_decay_y(uintptr_t curve_ptr);
		static double get_handle_freq_decay_root_y(uintptr_t curve_ptr);
		static void set_handle_freq_decay(uintptr_t curve_ptr, double x, double y);

	public:
		ElasticCurveHostObject() {
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
		}
	};
} // namespace cved