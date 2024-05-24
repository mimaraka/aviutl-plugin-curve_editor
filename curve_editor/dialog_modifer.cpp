#include "dialog_modifier.hpp"
#include "discretizer.hpp"
#include "noiser.hpp"
#include "constants.hpp"
#include "enum.hpp"
#include "global.hpp"
#include "my_messagebox.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	int ModifierDialog::i_resource() const noexcept { return IDD_MODIFIER; }


	void ModifierDialog::init_controls(HWND hwnd, const GraphCurve* p_curve) noexcept {
		using StringId = global::StringTable::StringId;

		hwnd_button_add_ = ::GetDlgItem(hwnd, IDC_BUTTON_ADD);
		hwnd_button_edit_ = ::GetDlgItem(hwnd, IDC_BUTTON_EDIT);
		hwnd_button_rename_ = ::GetDlgItem(hwnd, IDC_BUTTON_RENAME);
		hwnd_button_up_ = ::GetDlgItem(hwnd, IDC_BUTTON_UP);
		hwnd_button_down_ = ::GetDlgItem(hwnd, IDC_BUTTON_DOWN);
		hwnd_button_remove_ = ::GetDlgItem(hwnd, IDC_BUTTON_REMOVE);
		hwnd_list_modifier_ = ::GetDlgItem(hwnd, IDC_LIST_MODIFIER);
		menu_modifier_ = ::CreatePopupMenu();

		for (size_t i = 0; i < (size_t)ModifierType::NumModifierType; i++) {
			::AppendMenuA(
				menu_modifier_,
				MF_STRING,
				i + 1u,
				global::string_table[(StringId)((size_t)StringId::LabelModifierDiscretizer + i)]
			);
		}

		// リストの初期化
		HFONT font = ::CreateFontA(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "MS Shell Dlg");
		::SendMessageA(hwnd_list_modifier_, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));
		::SendMessageA(hwnd_list_modifier_, LB_SETITEMHEIGHT, 0, 16);
		for (const auto& modifier : p_curve->modifiers()) {
			::SendMessageA(hwnd_list_modifier_, LB_ADDSTRING, NULL, (LPARAM)modifier->name().c_str());
		}
	}


	void ModifierDialog::update(const GraphCurve* p_curve) noexcept {
		int idx = ::SendMessageA(hwnd_list_modifier_, LB_GETCURSEL, NULL, NULL);
		if (idx != LB_ERR) {
			::EnableWindow(hwnd_button_edit_, TRUE);
			::EnableWindow(hwnd_button_rename_, TRUE);
			::EnableWindow(hwnd_button_up_, idx != 0);
			::EnableWindow(hwnd_button_down_, idx != ::SendMessageA(hwnd_list_modifier_, LB_GETCOUNT, NULL, NULL) - 1);
			::EnableWindow(hwnd_button_remove_, TRUE);
		}
		else {
			::EnableWindow(hwnd_button_edit_, FALSE);
			::EnableWindow(hwnd_button_rename_, FALSE);
			::EnableWindow(hwnd_button_up_, FALSE);
			::EnableWindow(hwnd_button_down_, FALSE);
			::EnableWindow(hwnd_button_remove_, FALSE);
		}

		::SendMessageA(hwnd_list_modifier_, LB_RESETCONTENT, NULL, NULL);
		for (const auto& modifier : p_curve->modifiers()) {
			::SendMessageA(hwnd_list_modifier_, LB_ADDSTRING, NULL, (LPARAM)modifier->name().c_str());
		}
	}


	INT_PTR ModifierDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		static GraphCurve* p_curve = nullptr;
		static std::vector<std::unique_ptr<Modifier>> modifiers_prev;

		switch (message) {
		case WM_INITDIALOG:
			p_curve = reinterpret_cast<GraphCurve*>(lparam);
			if (!p_curve) return FALSE;

			// 編集前のモディファイアの状態を保存
			for (const auto& modifier : p_curve->modifiers()) {
				if (typeid(*modifier) == typeid(Discretizer)) {
					modifiers_prev.push_back(std::make_unique<Discretizer>(dynamic_cast<Discretizer&>(*modifier)));
				}
				else if (typeid(*modifier) == typeid(Noiser)) {
					modifiers_prev.push_back(std::make_unique<Noiser>(dynamic_cast<Noiser&>(*modifier)));
				}
			}

			init_controls(hwnd, p_curve);
			update(p_curve);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				modifiers_prev.clear();
				::EndDialog(hwnd, 1);
				return TRUE;

			case IDCANCEL:
				// モディファイアの状態を編集前に戻す
				p_curve->set_modifiers(modifiers_prev);
				global::window_grapheditor.redraw();
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;

			case IDC_BUTTON_ADD:
			{
				POINT tmp;
				::GetCursorPos(&tmp);
				auto ret = ::TrackPopupMenu(
					menu_modifier_,
					TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
					tmp.x,
					tmp.y,
					0,
					hwnd,
					NULL
				);
				if (0 < ret) {
					// モディファイアの追加
					std::unique_ptr<Modifier> new_modifier;
					switch ((ModifierType)(ret - 1)) {
					case ModifierType::Discretizer:
						new_modifier = std::make_unique<Discretizer>(&p_curve->pt_start(), &p_curve->pt_end());
						break;

					case ModifierType::Noiser:
						new_modifier = std::make_unique<Noiser>();
						break;

					default:
						break;
					}
					p_curve->add_modifier(new_modifier);
					update(p_curve);
					global::window_grapheditor.redraw();
				}
				return TRUE;
			}

			case IDC_BUTTON_EDIT:
				return TRUE;

			case IDC_BUTTON_RENAME:
				return TRUE;

			case IDC_BUTTON_UP:
				return TRUE;

			case IDC_BUTTON_DOWN:
				return TRUE;

			case IDC_BUTTON_REMOVE:
			{
				auto ret = my_messagebox(
					global::string_table[global::StringTable::StringId::WarningRemoveModifier],
					hwnd,
					MessageBoxIcon::Warning,
					MessageBoxButton::OkCancel
				);
				if (ret == IDOK) {
					auto idx = ::SendMessageA(hwnd_list_modifier_, LB_GETCURSEL, NULL, NULL);
					p_curve->remove_modifier(idx);
					update(p_curve);
					global::window_grapheditor.redraw();
				}
				return TRUE;
			}
			}
			switch (HIWORD(wparam)) {
			case LBN_SELCHANGE:
			{
				auto idx = ::SendMessageA(hwnd_list_modifier_, LB_GETCURSEL, NULL, NULL);
				update(p_curve);
				::SendMessageA(hwnd_list_modifier_, LB_SETCURSEL, (WPARAM)idx, NULL);
				return TRUE;
			}
			}
			break;
		}
		return FALSE;
	}
}