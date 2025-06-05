#include "config.hpp"
#include "dialog_pref_appearance.hpp"
#include "enum.hpp"
#include "my_webview2_reference.hpp"
#include "resource.h"
#include "string_table.hpp"
#include <Commctrl.h>
#include <format>
#include <strconv2.h>
#include <string_view>



namespace curve_editor {
	int AppearancePrefDialog::resource_id() const noexcept { return IDD_PREF_APPEARANCE; }

	void AppearancePrefDialog::init_controls(HWND hwnd) noexcept {
		using StringId = global::StringTable::StringId;

		hwnd_combo_theme_ = ::GetDlgItem(hwnd, IDC_COMBO_THEME);
		hwnd_button_curve_color_ = ::GetDlgItem(hwnd, IDC_BUTTON_CURVE_COLOR);
		hwnd_slider_curve_thickness_ = ::GetDlgItem(hwnd, IDC_SLIDER_CURVE_THICKNESS);
		hwnd_slider_curve_quality_ = ::GetDlgItem(hwnd, IDC_SLIDER_CURVE_RESOLUTION);
		hwnd_static_curve_thickness_ = ::GetDlgItem(hwnd, IDC_STATIC_CURVE_THICKNESS);
		hwnd_static_curve_quality_ = ::GetDlgItem(hwnd, IDC_STATIC_CURVE_RESOLUTION);
		hwnd_check_set_bg_image_ = ::GetDlgItem(hwnd, IDC_CHECK_BACKGROUND_IMAGE);
		hwnd_static_bg_image_1_ = ::GetDlgItem(hwnd, IDC_STATIC_BACKGROUND_IMAGE);
		hwnd_static_bg_image_2_ = ::GetDlgItem(hwnd, IDC_STATIC_BACKGROUND_IMAGE2);
		hwnd_edit_bg_image_path_ = ::GetDlgItem(hwnd, IDC_EDIT_BACKGROUND_IMAGE);
		hwnd_button_bg_image_path_ = ::GetDlgItem(hwnd, IDC_BUTTON_BACKGROUND_IMAGE);
		hwnd_slider_bg_image_opacity_ = ::GetDlgItem(hwnd, IDC_SLIDER_IMAGE_OPACITY);
		hwnd_static_bg_image_opacity_ = ::GetDlgItem(hwnd, IDC_STATIC_IMAGE_OPACITY);
		hwnd_static_apply_button_height_ = ::GetDlgItem(hwnd, IDC_STATIC_APPLY_BUTTON_HEIGHT);
		hwnd_slider_apply_button_height_ = ::GetDlgItem(hwnd, IDC_SLIDER_APPLY_BUTTON_HEIGHT);
		hwnd_check_show_trace_ = ::GetDlgItem(hwnd, IDC_CHECK_SHOW_TRACE);
		hwnd_check_enable_animation_ = ::GetDlgItem(hwnd, IDC_CHECK_ENABLE_ANIMATION);

		for (uint32_t i = 0u; i < (uint32_t)ThemeId::NumThemeId; i++) {
			::SendMessage(
				hwnd_combo_theme_,
				CB_ADDSTRING,
				NULL,
				(LPARAM)global::string_table[(StringId)((uint32_t)StringId::ThemeSystem + i)]
			);
		}

		::SendMessage(hwnd_slider_curve_thickness_, TBM_SETRANGE, TRUE, MAKELPARAM(1, 100));
		::SendMessage(hwnd_slider_curve_quality_, TBM_SETRANGE, TRUE, MAKELPARAM(100, 1000));
		::SendMessage(hwnd_slider_bg_image_opacity_, TBM_SETRANGE, TRUE, MAKELPARAM(0, 100));
		::SendMessage(hwnd_slider_apply_button_height_, TBM_SETRANGE, TRUE, MAKELPARAM(30, 200));
	}


	INT_PTR AppearancePrefDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using StringId = global::StringTable::StringId;
		static COLORREF custom_colors[16];
		static CHOOSECOLOR cc{
			.lStructSize = sizeof(CHOOSECOLOR),
			.lpCustColors = custom_colors,
			.Flags = CC_FULLOPEN | CC_RGBINIT
		};
		static wchar_t image_path[MAX_PATH + 1];
		image_path[MAX_PATH] = L'\0';

		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);

			cc.hwndOwner = hwnd;
			return TRUE;

		case WM_HSCROLL:
			if (lparam == (LPARAM)hwnd_slider_curve_thickness_) {
				int value_int = ::SendMessage(hwnd_slider_curve_thickness_, TBM_GETPOS, NULL, NULL);
				::SetWindowText(hwnd_static_curve_thickness_, std::format(L"{:.1f}", value_int * 0.1f).c_str());
			}
			else if (lparam == (LPARAM)hwnd_slider_curve_quality_) {
				int value_int = ::SendMessage(hwnd_slider_curve_quality_, TBM_GETPOS, NULL, NULL);
				::SetWindowText(hwnd_static_curve_quality_, std::format(L"{}", value_int).c_str());
			}
			else if (lparam == (LPARAM)hwnd_slider_bg_image_opacity_) {
				int value_int = ::SendMessage(hwnd_slider_bg_image_opacity_, TBM_GETPOS, NULL, NULL);
				::SetWindowText(hwnd_static_bg_image_opacity_, std::format(L"{}%", value_int).c_str());
			}
			else if (lparam == (LPARAM)hwnd_slider_apply_button_height_) {
				int value_int = ::SendMessage(hwnd_slider_apply_button_height_, TBM_GETPOS, NULL, NULL);
				::SetWindowText(hwnd_static_apply_button_height_, std::format(L"{}", value_int).c_str());
			}
			return TRUE;

		case WM_PAINT:
		{
			RECT rect_color = { 150, 28, 195, 41 };
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
				cc.rgbResult = global::config.get_curve_color().colorref();
				::InvalidateRect(hwnd, NULL, FALSE);
				::SendMessage(hwnd_combo_theme_, CB_SETCURSEL, (WPARAM)global::config.get_theme(), NULL);
				::SendMessage(
					hwnd_check_show_trace_,
					BM_SETCHECK,
					(WPARAM)global::config.get_show_trace(),
					NULL
				);
				::SendMessage(
					hwnd_check_set_bg_image_,
					BM_SETCHECK,
					(WPARAM)global::config.get_show_bg_image(),
					NULL
				);
				::SendMessage(
					hwnd_check_enable_animation_,
					BM_SETCHECK,
					(WPARAM)global::config.get_enable_animation(),
					NULL
				);
				if (global::config.get_show_bg_image()) {
					::EnableWindow(hwnd_static_bg_image_1_, TRUE);
					::EnableWindow(hwnd_edit_bg_image_path_, TRUE);
					::EnableWindow(hwnd_button_bg_image_path_, TRUE);
					::EnableWindow(hwnd_static_bg_image_2_, TRUE);
					::EnableWindow(hwnd_slider_bg_image_opacity_, TRUE);
					::EnableWindow(hwnd_static_bg_image_opacity_, TRUE);
				}
				else {
					::EnableWindow(hwnd_static_bg_image_1_, FALSE);
					::EnableWindow(hwnd_edit_bg_image_path_, FALSE);
					::EnableWindow(hwnd_button_bg_image_path_, FALSE);
					::EnableWindow(hwnd_static_bg_image_2_, FALSE);
					::EnableWindow(hwnd_slider_bg_image_opacity_, FALSE);
					::EnableWindow(hwnd_static_bg_image_opacity_, FALSE);
				};
				::SetWindowText(hwnd_edit_bg_image_path_, global::config.get_bg_image_path().wstring().c_str());
				::SendMessage(hwnd_edit_bg_image_path_, EM_SETLIMITTEXT, MAX_PATH, NULL);

				::SendMessage(hwnd_slider_curve_thickness_, TBM_SETPOS, TRUE, (LPARAM)(global::config.get_curve_thickness() * 10.f));
				::SendMessage(hwnd_slider_curve_quality_, TBM_SETPOS, TRUE, (LPARAM)(global::config.get_curve_resolution()));
				::SendMessage(hwnd_slider_bg_image_opacity_, TBM_SETPOS, TRUE, (LPARAM)round(global::config.get_bg_image_opacity() * 100.f));
				::SendMessage(hwnd_slider_apply_button_height_, TBM_SETPOS, TRUE, (LPARAM)global::config.get_apply_button_height());

				::SendMessage(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_curve_thickness_);
				::SendMessage(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_curve_quality_);
				::SendMessage(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_bg_image_opacity_);
				::SendMessage(hwnd, WM_HSCROLL, NULL, (LPARAM)hwnd_slider_apply_button_height_);

				return TRUE;

			case (UINT)WindowCommand::SaveConfig:
				global::config.set_theme((ThemeId)::SendMessage(hwnd_combo_theme_, CB_GETCURSEL, NULL, NULL));
				global::config.set_show_trace((bool)::SendMessage(hwnd_check_show_trace_, BM_GETCHECK, NULL, NULL));
				global::config.set_show_bg_image((bool)::SendMessage(hwnd_check_set_bg_image_, BM_GETCHECK, NULL, NULL));
				global::config.set_enable_animation((bool)::SendMessage(hwnd_check_enable_animation_, BM_GETCHECK, NULL, NULL));
				global::config.set_curve_color(cc.rgbResult);
				global::config.set_curve_thickness(
					(float)::SendMessage(hwnd_slider_curve_thickness_, TBM_GETPOS, NULL, NULL) * 0.1f
				);
				global::config.set_curve_resolution(
					(uint32_t)::SendMessage(hwnd_slider_curve_quality_, TBM_GETPOS, NULL, NULL)
				);
				global::config.set_bg_image_opacity(
					(float)::SendMessage(hwnd_slider_bg_image_opacity_, TBM_GETPOS, NULL, NULL) * 0.01f
				);
				global::config.set_apply_button_height(
					(uint32_t)::SendMessage(hwnd_slider_apply_button_height_, TBM_GETPOS, NULL, NULL)
				);
				{
					wchar_t buffer[MAX_PATH];
					::GetWindowTextW(hwnd_edit_bg_image_path_, buffer, MAX_PATH);
					global::config.set_bg_image_path(std::filesystem::path(buffer));
				}
				if (global::webview) global::webview->update_color_scheme();
				return TRUE;

			case IDC_BUTTON_CURVE_COLOR:
				if (::ChooseColor(&cc)) {
					::InvalidateRect(hwnd, NULL, FALSE);
				}
				return TRUE;

			case IDC_CHECK_BACKGROUND_IMAGE:
				if (::SendMessageA(hwnd_check_set_bg_image_, BM_GETCHECK, NULL, NULL)) {
					::EnableWindow(hwnd_static_bg_image_1_, TRUE);
					::EnableWindow(hwnd_edit_bg_image_path_, TRUE);
					::EnableWindow(hwnd_button_bg_image_path_, TRUE);
					::EnableWindow(hwnd_static_bg_image_2_, TRUE);
					::EnableWindow(hwnd_slider_bg_image_opacity_, TRUE);
					::EnableWindow(hwnd_static_bg_image_opacity_, TRUE);
				}
				else {
					::EnableWindow(hwnd_static_bg_image_1_, FALSE);
					::EnableWindow(hwnd_edit_bg_image_path_, FALSE);
					::EnableWindow(hwnd_button_bg_image_path_, FALSE);
					::EnableWindow(hwnd_static_bg_image_2_, FALSE);
					::EnableWindow(hwnd_slider_bg_image_opacity_, FALSE);
					::EnableWindow(hwnd_static_bg_image_opacity_, FALSE);
				}
				return TRUE;

			case IDC_BUTTON_BACKGROUND_IMAGE:
			{
				using namespace std::literals::string_view_literals;

				constexpr auto TEMPLATE_IMAGE = L"*.bmp;*.jpg;*.jpeg;*.png;*.webp;*.jfif;*.gif";
				auto str_filter = std::format(
					L"{0} ({1})\0{1}\0"sv,
					global::string_table[StringId::WordImageFile],
					TEMPLATE_IMAGE
				);
				OPENFILENAME ofn{
					.lStructSize = sizeof(OPENFILENAME),
					.hwndOwner = hwnd,
					.lpstrFilter = str_filter.c_str(),
					.lpstrFile = image_path,
					.nMaxFile = MAX_PATH + 1,
					.lpstrTitle = global::string_table[StringId::CaptionSelectBackgroundImage],
					.Flags = OFN_FILEMUSTEXIST
				};
				if (::GetOpenFileName(&ofn)) {
					::SetWindowText(hwnd_edit_bg_image_path_, ofn.lpstrFile);
				}
				return TRUE;
			}
			}
			break;
		}
		return FALSE;
	}
} // namespace curve_editor