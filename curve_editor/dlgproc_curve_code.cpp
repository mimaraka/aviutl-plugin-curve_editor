#include "dlgproc_curve_code.hpp"
#include "curve_editor.hpp"
#include "util.hpp"
#include "global.hpp"
#include "string_table.hpp"
#include "resource.h"
#include <regex>



namespace cved {
	INT_PTR CALLBACK dlgproc_curve_code(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using StringId = global::StringTable::StringId;
		constexpr size_t MAX_TEXT = 12;
		const std::regex regex_code{ R"(^-?\d+$)" };
		
		switch (message) {
		case WM_INITDIALOG:
		{
			HWND hwnd_edit = ::GetDlgItem(hwnd, IDC_EDIT_CURVE_CODE);
			::SendMessageA(hwnd_edit, EM_SETLIMITTEXT, MAX_TEXT, NULL);
			::SetFocus(hwnd_edit);
		}
			break;

		case WM_CLOSE:
			::EndDialog(hwnd, 1);
			break;

		case WM_KEYDOWN:
			if (wparam == VK_RETURN)
				::SendMessageA(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
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
								global::window_main.send_command((WPARAM)WindowCommand::Update);
								::EndDialog(hwnd, 1);
								break;
							}
						}
						catch (std::out_of_range&) {}
						// 入力値が範囲外の場合
						util::show_popup(global::string_table[StringId::ErrorOutOfRange], util::PopupIcon::Error);
					}
					// 入力値が不正の場合
					else {
						util::show_popup(global::string_table[StringId::ErrorInvalidInput], util::PopupIcon::Error);
					}
				}
				else {
					::EndDialog(hwnd, 1);
					break;
				}
				break;
			}

			case IDCANCEL:
				::EndDialog(hwnd, 1);
				break;
			}
			break;
		}
		return 0;
	}
}