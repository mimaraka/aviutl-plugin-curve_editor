#include "wndproc_library.hpp"


namespace cved {
	LRESULT CALLBACK wndproc_library(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
	{
		return ::DefWindowProc(hwnd, message, wparam, lparam);
	}
}