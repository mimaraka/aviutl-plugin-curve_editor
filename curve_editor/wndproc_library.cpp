#include "wndproc_library.hpp"
#include <mkaul/graphics.hpp>
#include "config.hpp"


namespace cved {
	LRESULT CALLBACK wndproc_library(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
	{
		static auto p_graphics = mkaul::graphics::Factory::create_graphics();

		switch (message) {
		case WM_CREATE:
			p_graphics->init(hwnd);
			break;

		case WM_CLOSE:
		case WM_DESTROY:
			p_graphics->release();
			break;

		case WM_PAINT:
			p_graphics->begin_draw();
			p_graphics->fill_background(global::config.get_theme().bg);
			p_graphics->end_draw();
			break;
		}
		return 0;
	}
}