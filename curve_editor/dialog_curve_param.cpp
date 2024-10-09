#include "dialog_curve_param.hpp"
#include "resource.h"



namespace cved {
	int CurveParamDialog::resource_id() const noexcept { return IDD_PARAM; }


	void CurveParamDialog::init_controls(HWND hwnd) noexcept {
		hwnd_edit_ = ::GetDlgItem(hwnd, IDC_EDIT_BEZIER_PARAM);
		::SetFocus(hwnd_edit_);
	}


	INT_PTR CurveParamDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				// Get the text from the edit control
				::EndDialog(hwnd, IDOK);
				return TRUE;

			case IDCANCEL:
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}