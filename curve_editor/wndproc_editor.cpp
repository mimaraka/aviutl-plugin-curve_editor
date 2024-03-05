#include "wndproc_editor.hpp"
#include "curve_editor_graph.hpp"



namespace cved {
	LRESULT CALLBACK wndproc_editor(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		
		switch (message) {
		case WM_CREATE:
			return 0;
		}
		return ::DefWindowProc(hwnd, message, wparam, lparam);
	}
}