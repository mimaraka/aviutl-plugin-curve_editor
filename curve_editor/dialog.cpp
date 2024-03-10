#include "dialog.hpp"
#include "global.hpp"



namespace cved {
	HWND Dialog::create(HWND hwnd) const noexcept {
		return ::CreateDialogParamA(
			global::fp->dll_hinst,
			MAKEINTRESOURCEA(i_resource()),
			hwnd,
			message_router,
			(LPARAM)this
		);
	}

	INT_PTR Dialog::show(HWND hwnd) const noexcept {
		return ::DialogBoxParamA(
			global::fp->dll_hinst,
			MAKEINTRESOURCEA(i_resource()),
			hwnd,
			message_router,
			(LPARAM)this
		);
	}

	INT_PTR CALLBACK Dialog::message_router(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		if (message == WM_INITDIALOG) {
			if (!lparam) return FALSE;
			::SetWindowLongA(hwnd, GWL_USERDATA, (LONG)lparam);
		}
		Dialog* p_inst = reinterpret_cast<Dialog*>(::GetWindowLongA(hwnd, GWL_USERDATA));
		if (!p_inst) return FALSE;
		return p_inst->dialog_proc(hwnd, message, wparam, lparam);
	}
}