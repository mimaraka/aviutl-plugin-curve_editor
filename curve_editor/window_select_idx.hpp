#pragma once

#include <mkaul/window.hpp>



namespace curve_editor {
	class SelectIdxWindow : public mkaul::ui::Window {
		HWND hwnd_parent_ = nullptr;
		static LRESULT CALLBACK wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	public:
		HWND create(HWND hwnd) noexcept;
	};
} // namespace curve_editor