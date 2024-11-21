#pragma once

#include "host_object_curve_graph.hpp"



namespace curve_editor {
	class ElasticCurveHostObject : public GraphCurveHostObject {
		static std::vector<double> get_handle_amp_left(uint32_t id);
		static void set_handle_amp_left(uint32_t id, double y);
		static std::vector<double> get_handle_amp_right(uint32_t id);
		static void set_handle_amp_right(uint32_t id, double y);
		static std::vector<double> get_handle_freq_decay(uint32_t id);
		static double get_handle_freq_decay_root_y(uint32_t id);
		static void set_handle_freq_decay(uint32_t id, double x, double y);
		static std::string get_param(uint32_t id);
		static uint32_t get_id(bool is_select_dialog);

	public:
		ElasticCurveHostObject() {
			register_member(L"getId", DispatchType::Method, get_id);
			register_member(L"getHandleAmpLeft", DispatchType::Method, get_handle_amp_left);
			register_member(L"moveHandleAmpLeft", DispatchType::Method, set_handle_amp_left);
			register_member(L"getHandleAmpRight", DispatchType::Method, get_handle_amp_right);
			register_member(L"moveHandleAmpRight", DispatchType::Method, set_handle_amp_right);
			register_member(L"getHandleFreqDecay", DispatchType::Method, get_handle_freq_decay);
			register_member(L"getHandleFreqDecayRootY", DispatchType::Method, get_handle_freq_decay_root_y);
			register_member(L"moveHandleFreqDecay", DispatchType::Method, set_handle_freq_decay);
			register_member(L"getParam", DispatchType::Method, get_param);
		}
	};
} // namespace curve_editor