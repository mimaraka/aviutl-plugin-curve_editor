#include "dialog_control_position.hpp"

#include <cmath>
#include <cwctype>
#include <format>
#include <optional>

#include "message_box.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace curve_editor {
	using StringId = global::StringTable::StringId;

	int ControlPositionDialog::resource_id() const noexcept { return IDD_CONTROL_POSITION; }


	// 座標値を小数点以下4桁に丸めて表示用の文字列に変換する（末尾の不要な0は付与しない）
	static std::wstring format_coord(double value) noexcept {
		constexpr double scale = 1e4; // 小数点以下4桁
		double rounded = std::round(value * scale) / scale;
		// -0 を 0 に正規化
		if (rounded == 0.0) {
			rounded = 0.0;
		}
		return std::format(L"{}", rounded);
	}


	// エディットの内容をdoubleとして解釈する。数値として解釈できない場合はstd::nulloptを返す
	static std::optional<double> parse_double(HWND hwnd_edit) noexcept {
		wchar_t buffer[64];
		::GetWindowTextW(hwnd_edit, buffer, std::size(buffer));
		std::wstring text = buffer;
		try {
			size_t pos = 0;
			double value = std::stod(text, &pos);
			// 末尾の空白を許容しつつ、数値以外の文字が残っていないか確認
			while (pos < text.size() and std::iswspace(text[pos])) {
				pos++;
			}
			if (pos != text.size()) {
				return std::nullopt;
			}
			return value;
		}
		catch (const std::exception&) {
			return std::nullopt;
		}
	}


	INT_PTR ControlPositionDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		switch (message) {
		case WM_INITDIALOG:
		{
			hwnd_edit_x_ = ::GetDlgItem(hwnd, IDC_EDIT_X);
			hwnd_edit_y_ = ::GetDlgItem(hwnd, IDC_EDIT_Y);
			::SendMessageA(hwnd_edit_x_, EM_SETLIMITTEXT, MAX_TEXT, NULL);
			::SendMessageA(hwnd_edit_y_, EM_SETLIMITTEXT, MAX_TEXT, NULL);
			::SetWindowTextW(hwnd_edit_x_, format_coord(init_x_).c_str());
			::SetWindowTextW(hwnd_edit_y_, format_coord(init_y_).c_str());
			if (enable_x_) {
				::SetFocus(hwnd_edit_x_);
				::PostMessageA(hwnd_edit_x_, EM_SETSEL, 0, -1);
			}
			else {
				// X座標は編集不可（振幅ハンドル等）。Y座標にフォーカスを当てる
				::EnableWindow(hwnd_edit_x_, FALSE);
				::SetFocus(hwnd_edit_y_);
				::PostMessageA(hwnd_edit_y_, EM_SETSEL, 0, -1);
			}
			::SetWindowTextW(hwnd, caption_.c_str());
			::SetDlgItemTextW(hwnd, IDOK, global::string_table[StringId::WordOK]);
			::SetDlgItemTextW(hwnd, IDCANCEL, global::string_table[StringId::WordCancel]);
			return FALSE;
		}

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
			{
				auto x = parse_double(hwnd_edit_x_);
				auto y = parse_double(hwnd_edit_y_);
				if (!x.has_value() or !y.has_value()) {
					util::message_box(
						global::string_table[StringId::ErrorInvalidInput],
						hwnd,
						util::MessageBoxIcon::Error
					);
					break;
				}
				if (!on_submit_ or on_submit_(hwnd, x.value(), y.value())) {
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
