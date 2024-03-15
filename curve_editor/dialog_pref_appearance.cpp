#include "dialog_pref_appearance.hpp"
#include <Commctrl.h>
#include "config.hpp"
#include "enum.hpp"
#include "global.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	int AppearancePrefDialog::i_resource() const noexcept { return IDD_PREF_APPEARANCE; }

	void AppearancePrefDialog::init_controls(HWND hwnd) noexcept {
		using StringId = global::StringTable::StringId;

		hwnd_combo_theme_ = ::GetDlgItem(hwnd, IDC_COMBO_THEME);
		hwnd_button_curve_color_ = ::GetDlgItem(hwnd, IDC_BUTTON_CURVE_COLOR);
		hwnd_slider_curve_thickness_ = ::GetDlgItem(hwnd, IDC_SLIDER_CURVE_THICKNESS);
		hwnd_slider_curve_drawing_interval_ = ::GetDlgItem(hwnd, IDC_SLIDER_CURVE_DRAWING_INTERVAL);
		hwnd_static_curve_thickness_ = ::GetDlgItem(hwnd, IDC_STATIC_CURVE_THICKNESS);
		hwnd_static_curve_drawing_interval_ = ::GetDlgItem(hwnd, IDC_STATIC_CURVE_DRAWING_INTERVAL);
		hwnd_check_show_trace_ = ::GetDlgItem(hwnd, IDC_CHECK_SHOW_TRACE);

		for (uint32_t i = 0u; i < (uint32_t)ThemeId::NumThemeId; i++) {
			::SendMessageA(
				hwnd_combo_theme_,
				CB_ADDSTRING,
				NULL,
				(LPARAM)global::string_table[(StringId)((uint32_t)StringId::LabelThemeNameDark + i)]
			);
		}

		::SendMessageA(hwnd_slider_curve_thickness_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
		::SendMessageA(hwnd_slider_curve_drawing_interval_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
	}


	INT_PTR AppearancePrefDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using StringId = global::StringTable::StringId;
		static COLORREF custom_colors[16];
		static CHOOSECOLORA cc{
			.lStructSize = sizeof(CHOOSECOLORA),
			.lpCustColors = custom_colors,
			.Flags = CC_FULLOPEN | CC_RGBINIT
		};

		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);

			cc.hwndOwner = hwnd;
			cc.rgbResult = global::config.get_curve_color().colorref();
			return TRUE;

		case WM_HSCROLL:
			if (lparam == (LPARAM)hwnd_slider_curve_thickness_) {
				int value_int = ::SendMessageA(hwnd_slider_curve_thickness_, TBM_GETPOS, NULL, NULL);
				::SetWindowTextA(hwnd_static_curve_thickness_, std::format("{:.1f}", value_int * 0.1f).c_str());
			}
			else if (lparam == (LPARAM)hwnd_slider_curve_drawing_interval_) {
				int value_int = ::SendMessageA(hwnd_slider_curve_drawing_interval_, TBM_GETPOS, NULL, NULL);
				::SetWindowTextA(hwnd_static_curve_drawing_interval_, std::format("{:.1f}", value_int * 0.1f).c_str());
			}
			return TRUE;

		case WM_PAINT:
		{
			RECT rect_color = { 160, 23, 188, 36 };
			::MapDialogRect(hwnd, &rect_color);
			PAINTSTRUCT ps;
			HDC hdc = ::BeginPaint(hwnd, &ps);
			HBRUSH brush = ::CreateSolidBrush(cc.rgbResult);
			HPEN pen = ::CreatePen(PS_SOLID, 1, 0);
			::SelectObject(hdc, brush);
			::SelectObject(hdc, pen);
			::Rectangle(hdc, rect_color.left, rect_color.top, rect_color.right, rect_color.bottom);
			::DeleteObject(brush);
			::DeleteObject(pen);
			::EndPaint(hwnd, &ps);
			return TRUE;
		}

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case (UINT)WindowCommand::LoadConfig:
				::SendMessageA(hwnd_combo_theme_, CB_SETCURSEL, (WPARAM)global::config.get_theme_id(), NULL);
				::SendMessageA(
					hwnd_check_show_trace_,
					BM_SETCHECK,
					(WPARAM)global::config.get_show_trace(),
					NULL
				);
				::SendMessageA(hwnd_slider_curve_thickness_, TBM_SETPOS, TRUE, (LPARAM)(global::config.get_curve_thickness() * 10.f));
				::SendMessageA(hwnd_slider_curve_drawing_interval_, TBM_SETPOS, TRUE, (LPARAM)(global::config.get_curve_drawing_interval() * 10.f));
				::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_curve_thickness_);
				::SendMessageA(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_curve_drawing_interval_);
				return TRUE;

			case (UINT)WindowCommand::SaveConfig:
				global::config.set_theme_id((ThemeId)::SendMessageA(hwnd_combo_theme_, CB_GETCURSEL, NULL, NULL));
				global::config.set_show_trace((bool)::SendMessageA(hwnd_check_show_trace_, BM_GETCHECK, NULL, NULL));
				global::config.set_curve_color(cc.rgbResult);
				global::config.set_curve_thickness(
					(float)::SendMessageA(hwnd_slider_curve_thickness_, TBM_GETPOS, NULL, NULL) * 0.1f
				);
				global::config.set_curve_drawing_interval(
					(float)::SendMessageA(hwnd_slider_curve_drawing_interval_, TBM_GETPOS, NULL, NULL) * 0.1f
				);
				return TRUE;

			case IDC_BUTTON_CURVE_COLOR:
				if (::ChooseColorA(&cc)) {
					::InvalidateRect(hwnd, NULL, FALSE);
				}
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}