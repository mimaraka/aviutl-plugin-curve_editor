#include "config.hpp"
#include "curve_editor.hpp"
#include "dialog_curve_code.hpp"
#include "global.hpp"
#include "my_messagebox.hpp"
#include "resource.h"
#include "string_table.hpp"
#include <regex>



namespace cved {
	int CurveCodeDialog::resource_id() const noexcept { return IDD_CURVE_CODE; }


	void CurveCodeDialog::init_controls(HWND hwnd) noexcept {
		hwnd_edit_ = ::GetDlgItem(hwnd, IDC_EDIT_CURVE_CODE);
		::SendMessageA(hwnd_edit_, EM_SETLIMITTEXT, MAX_TEXT, NULL);
		::SetFocus(hwnd_edit_);
	}


	INT_PTR CurveCodeDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		using StringId = global::StringTable::StringId;
		const std::regex regex_code{ R"(^-?\d+$)" };

		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_KEYDOWN:
			if (wparam == VK_RETURN) {
				::SendMessageA(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
				return TRUE;
			}
			break;


		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
			{
				char buffer[MAX_TEXT + 1u];
				auto p_curve = global::editor.editor_graph().numeric_curve();
				::GetDlgItemTextA(hwnd, IDC_EDIT_CURVE_CODE, buffer, MAX_TEXT);
				if (p_curve) {
					if (std::regex_match(buffer, regex_code)) {
						try {
							if (p_curve->decode(std::stoi(buffer))) {
								::SendMessageA(global::fp->hwnd, WM_COMMAND, (WPARAM)WindowCommand::UpdateHandlePos, NULL);
								::EndDialog(hwnd, 1);
								break;
							}
						}
						catch (std::out_of_range&) {}
						// 入力値が範囲外の場合
						if (global::config.get_show_popup()) {
							my_messagebox(global::string_table[StringId::ErrorOutOfRange], hwnd, MessageBoxIcon::Error);
							return TRUE;
						}
					}
					// 入力値が不正の場合
					else if (global::config.get_show_popup()) {
						my_messagebox(global::string_table[StringId::ErrorInvalidInput], hwnd, MessageBoxIcon::Error);
						return TRUE;
					}
				}
				else {
					::EndDialog(hwnd, IDOK);
					return TRUE;
				}
				return TRUE;
			}

			case IDCANCEL:
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}