#include "dialog_input.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace curve_editor {
	int InputDialog::resource_id() const noexcept { return IDD_INPUT; }

	INT_PTR InputDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		switch (message) {
		case WM_INITDIALOG:
		{
			using StringId = global::StringTable::StringId;
			hwnd_edit_ = ::GetDlgItem(hwnd, IDC_EDIT1);
			::SendMessageA(hwnd_edit_, EM_SETLIMITTEXT, MAX_TEXT, NULL);
			::SetFocus(hwnd_edit_);
			::SetWindowTextW(hwnd_edit_, default_text_.c_str());
			::PostMessageA(hwnd_edit_, EM_SETSEL, 0, default_text_.size());
			auto hwnd_static_prompt = GetDlgItem(hwnd, IDC_STATIC_PROMPT);
			::SetWindowTextW(hwnd_static_prompt, prompt_.c_str());
			::SetWindowTextW(hwnd, caption_.c_str());
			::SetDlgItemTextW(hwnd, IDOK, global::string_table[StringId::WordOK]);
			::SetDlgItemTextW(hwnd, IDCANCEL, global::string_table[StringId::WordCancel]);
			break;
		}

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
				wchar_t buffer[MAX_TEXT];
				::GetWindowTextW(hwnd_edit_, buffer, MAX_TEXT);
				if (on_submit_(hwnd, buffer)) {
					::EndDialog(hwnd, IDOK);
				}
				break;
			}

			case IDCANCEL:
				::EndDialog(hwnd, IDCANCEL);
				break;
			}
			break;
		}
		return FALSE;
	}
} // namespace curve_editor