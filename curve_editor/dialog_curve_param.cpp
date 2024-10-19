#include "dialog_curve_param.hpp"
#include "resource.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>



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
			{
					char buf[256];
					::GetWindowTextA(hwnd_edit_, buf, sizeof(buf));
					std::string str{ buf };
					std::vector<std::string> vec_param;
					boost::algorithm::split(vec_param, str, boost::is_any_of(","));
			}
				[[fallthrough]];

			case IDCANCEL:
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}