#pragma once

#include <mkaul/window.hpp>



namespace cved {
	class SelectCurveWindow : public mkaul::ui::Window {
		static LRESULT CALLBACK wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	public:
		HWND create(HWND hwnd, void* init_param = nullptr) noexcept;
	};
}