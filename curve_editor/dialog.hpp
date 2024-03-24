#pragma once

#include <Windows.h>



namespace cved {
	class Dialog {
		static INT_PTR CALLBACK message_router(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
		virtual INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) = 0;
		virtual void init_controls(HWND hwnd) noexcept = 0;
		virtual int i_resource() const noexcept = 0;

	public:
		HWND create(HWND hwnd) const noexcept;
		INT_PTR show(HWND hwnd) const noexcept;
	};
}