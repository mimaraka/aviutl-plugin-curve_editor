#include "constants.hpp"
#include "dialog.hpp"
#include <bit>



namespace curve_editor {
	HWND Dialog::create(HWND hwnd, LPARAM init_param) noexcept {
		init_param_ = init_param;
		return ::CreateDialogParamA(
			::GetModuleHandleA(global::PLUGIN_DLL_NAME),
			MAKEINTRESOURCEA(resource_id()),
			hwnd,
			message_router,
			std::bit_cast<LPARAM>(this)
		);
	}

	INT_PTR Dialog::show(HWND hwnd, LPARAM init_param) noexcept {
		init_param_ = init_param;
		return ::DialogBoxParamA(
			::GetModuleHandleA(global::PLUGIN_DLL_NAME),
			MAKEINTRESOURCEA(resource_id()),
			hwnd,
			message_router,
			std::bit_cast<LPARAM>(this)
		);
	}

	INT_PTR CALLBACK Dialog::message_router(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		if (message == WM_INITDIALOG) {
			if (!lparam) return FALSE;
			::SetWindowLongA(hwnd, GWL_USERDATA, (LONG)lparam);
			auto p_inst = std::bit_cast<Dialog*>(lparam);
			return p_inst->dialog_proc(hwnd, message, wparam, p_inst->init_param_);
		}
		else {
			auto p_inst = std::bit_cast<Dialog*>(::GetWindowLongA(hwnd, GWL_USERDATA));
			if (!p_inst) return FALSE;
			return p_inst->dialog_proc(hwnd, message, wparam, lparam);
		}
	}
} // namespace curve_editor