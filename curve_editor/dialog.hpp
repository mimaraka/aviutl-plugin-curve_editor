#pragma once

#include <Windows.h>



namespace curve_editor {
	class Dialog {
		LPARAM init_param_ = NULL;

		static INT_PTR CALLBACK message_router(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
		virtual INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) = 0;
		virtual void init_controls(HWND) noexcept {};
		virtual int resource_id() const noexcept = 0;

	public:
		HWND create(HWND hwnd, LPARAM init_param = NULL) noexcept;
		INT_PTR show(HWND hwnd, LPARAM init_param = NULL) noexcept;
	};
} // namespace curve_editor