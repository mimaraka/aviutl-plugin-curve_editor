#pragma once

#include "host_object_curve_graph.hpp"



namespace cved {
	class BezierCurveHostObject : public GraphCurveHostObject {
		static double get_handle_left_x(uint32_t id);
		static double get_handle_left_y(uint32_t id);
		static void begin_move_handle_left(uint32_t id, double scale_x, double scale_y);
		static void move_handle_left(uint32_t id, double x, double y, bool keep_angle);
		static double get_handle_right_x(uint32_t id);
		static double get_handle_right_y(uint32_t id);
		static void begin_move_handle_right(uint32_t id, double scale_x, double scale_y);
		static void move_handle_right(uint32_t id, double x, double y, bool keep_angle);
		static std::string get_param(uint32_t id);
		static uint32_t get_id(bool is_select_dialog);

	public:
		BezierCurveHostObject() {
			register_member(L"getId", DispatchType::Method, get_id);
			register_member(L"getHandleLeftX", DispatchType::Method, get_handle_left_x);
			register_member(L"getHandleLeftY", DispatchType::Method, get_handle_left_y);
			register_member(L"beginMoveHandleLeft", DispatchType::Method, begin_move_handle_left);
			register_member(L"moveHandleLeft", DispatchType::Method, move_handle_left);
			register_member(L"getHandleRightX", DispatchType::Method, get_handle_right_x);
			register_member(L"getHandleRightY", DispatchType::Method, get_handle_right_y);
			register_member(L"beginMoveHandleRight", DispatchType::Method, begin_move_handle_right);
			register_member(L"moveHandleRight", DispatchType::Method, move_handle_right);
			register_member(L"getParam", DispatchType::Method, get_param);
		}
	};
} // namespace cved