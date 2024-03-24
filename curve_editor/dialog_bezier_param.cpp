#include "dialog_bezier_param.hpp"
#include "resource.h"



namespace cved {
	int BezierParamDialog::i_resource() const noexcept { return IDD_BEZIER_PARAM; }


	void BezierParamDialog::init_controls(HWND hwnd) noexcept {
		hwnd_edit_ = ::GetDlgItem(hwnd, IDC_EDIT_BEZIER_PARAM);
	}


	INT_PTR BezierParamDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_COMMAND:
			break;
		}
		return FALSE;
	}
}