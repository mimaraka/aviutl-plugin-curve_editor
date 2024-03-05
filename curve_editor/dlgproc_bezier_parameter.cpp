#include "dlgproc_bezier_parameter.hpp"



namespace cved {
	INT_PTR CALLBACK dlgproc_bezier_parameter(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		switch (message) {
		case WM_INITDIALOG:
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				
			}
			break;
		}
		return 0;
	}
}