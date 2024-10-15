#include "constants.hpp"
#include "dialog_modifier.hpp"
#include "dialog_modifier_discretization.hpp"
#include "dialog_modifier_noise.hpp"
#include "dialog_modifier_sine_wave.hpp"
#include "dialog_modifier_square_wave.hpp"
#include "enum.hpp"
#include "global.hpp"
#include "modifier.hpp"
#include "my_messagebox.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace cved {
	int ModifierDialog::resource_id() const noexcept { return IDD_MODIFIER; }


	void ModifierDialog::init_controls(HWND hwnd, const GraphCurve* curve) noexcept {
		using StringId = global::StringTable::StringId;

		hwnd_button_add_ = ::GetDlgItem(hwnd, IDC_BUTTON_ADD);
		hwnd_button_edit_ = ::GetDlgItem(hwnd, IDC_BUTTON_EDIT);
		hwnd_button_rename_ = ::GetDlgItem(hwnd, IDC_BUTTON_RENAME);
		hwnd_button_up_ = ::GetDlgItem(hwnd, IDC_BUTTON_UP);
		hwnd_button_down_ = ::GetDlgItem(hwnd, IDC_BUTTON_DOWN);
		hwnd_button_remove_ = ::GetDlgItem(hwnd, IDC_BUTTON_REMOVE);
		hwnd_check_bypass_ = ::GetDlgItem(hwnd, IDC_CHECK_BYPASS);
		hwnd_list_modifier_ = ::GetDlgItem(hwnd, IDC_LIST_MODIFIER);
		menu_modifier_ = ::CreatePopupMenu();

		for (size_t i = 0; i < (size_t)ModifierType::NumModifierType; i++) {
			::AppendMenuA(
				menu_modifier_,
				MF_STRING,
				i + 1u,
				global::string_table[(StringId)((size_t)StringId::LabelModifierDiscretization + i)]
			);
		}

		// リストの初期化
		HFONT font = ::CreateFontA(
			14, 0, 0, 0,
			FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			SHIFTJIS_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH,
			"MS Shell Dlg"
		);
		::SendMessageA(hwnd_list_modifier_, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));
		::SendMessageA(hwnd_list_modifier_, LB_SETITEMHEIGHT, 0, 16);
		for (const auto& modifier : curve->modifiers()) {
			::SendMessageA(hwnd_list_modifier_, LB_ADDSTRING, NULL, (LPARAM)modifier->name().c_str());
		}
	}


	void ModifierDialog::update_buttons(const GraphCurve* curve) noexcept {
		int idx = ::SendMessageA(hwnd_list_modifier_, LB_GETCURSEL, NULL, NULL);
		auto modifier = curve->get_modifier(idx);
		if (idx != LB_ERR) {
			::EnableWindow(hwnd_button_edit_, TRUE);
			::EnableWindow(hwnd_button_rename_, TRUE);
			::EnableWindow(hwnd_button_up_, idx != 0);
			::EnableWindow(hwnd_button_down_, idx != ::SendMessageA(hwnd_list_modifier_, LB_GETCOUNT, NULL, NULL) - 1);
			::EnableWindow(hwnd_button_remove_, TRUE);
			::EnableWindow(hwnd_check_bypass_, TRUE);
		}
		else {
			::EnableWindow(hwnd_button_edit_, FALSE);
			::EnableWindow(hwnd_button_rename_, FALSE);
			::EnableWindow(hwnd_button_up_, FALSE);
			::EnableWindow(hwnd_button_down_, FALSE);
			::EnableWindow(hwnd_button_remove_, FALSE);
			::EnableWindow(hwnd_check_bypass_, FALSE);
		}
		if (modifier) {
			::SendMessageA(hwnd_check_bypass_, BM_SETCHECK, !modifier->enabled() ? BST_CHECKED : BST_UNCHECKED, NULL);
		}
		else {
			::SendMessageA(hwnd_check_bypass_, BM_SETCHECK, BST_UNCHECKED, NULL);
		}
	}


	void ModifierDialog::update_list(const GraphCurve* curve) noexcept {
		::SendMessageA(hwnd_list_modifier_, LB_RESETCONTENT, NULL, NULL);
		for (const auto& modifier : curve->modifiers()) {
			::SendMessageA(hwnd_list_modifier_, LB_ADDSTRING, NULL, (LPARAM)modifier->name().c_str());
		}
	}


	INT_PTR ModifierDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		static GraphCurve* curve = nullptr;
		static std::vector<std::unique_ptr<Modifier>> modifiers_prev;

		switch (message) {
		case WM_INITDIALOG:
			curve = global::id_manager.get_curve<GraphCurve>(static_cast<uint32_t>(lparam));
			if (!curve) {
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;
			}

			// 編集前のモディファイアの状態を保存
			for (const auto& modifier : curve->modifiers()) {
				save_prev_modifier<
					DiscretizationModifier,
					NoiseModifier,
					SineWaveModifier,
					SquareWaveModifier
				>(modifier, modifiers_prev);
			}

			init_controls(hwnd, curve);
			update_list(curve);
			update_buttons(curve);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				modifiers_prev.clear();
				::EndDialog(hwnd, 1);
				return TRUE;

			case IDCANCEL:
				// モディファイアの状態を編集前に戻す
				curve->set_modifiers(modifiers_prev);
				global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
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
					std::unique_ptr<Modifier> new_modifier = create_modifier<0u,
						DiscretizationModifier,
						NoiseModifier,
						SineWaveModifier,
						SquareWaveModifier
					>(ret - 1);
					if (!new_modifier) {
						return TRUE;
					}
					new_modifier->set_curve(curve);
					curve->add_modifier(new_modifier);
					update_list(curve);
					::SendMessageA(hwnd_list_modifier_, LB_SETCURSEL, ::SendMessageA(hwnd_list_modifier_, LB_GETCOUNT, NULL, NULL) - 1, NULL);
					update_buttons(curve);
					global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
				}
				return TRUE;
			}

			case IDC_BUTTON_EDIT:
			{
				int idx = ::SendMessageA(hwnd_list_modifier_, LB_GETCURSEL, NULL, NULL);
				if (idx != LB_ERR) {
					auto p_modifier = curve->get_modifier(idx);
					show_editor_dialog<
						DiscretizationModifier,
						DiscretizationModifierDialog,
						NoiseModifier,
						NoiseModifierDialog,
						SineWaveModifier,
						SineWaveModifierDialog,
						SquareWaveModifier,
						SquareWaveModifierDialog
					>(hwnd, p_modifier);
				}
				return TRUE;
			}

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
					if (idx != LB_ERR) {
						curve->remove_modifier(idx);
						update_list(curve);
						update_buttons(curve);
						global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
					}
				}
				return TRUE;
			}

			case IDC_CHECK_BYPASS:
			{
				auto idx = ::SendMessageA(hwnd_list_modifier_, LB_GETCURSEL, NULL, NULL);
				if (idx != LB_ERR) {
					auto p_modifier = curve->get_modifier(idx);
					if (p_modifier) {
						p_modifier->set_enabled(::SendMessageA(hwnd_check_bypass_, BM_GETCHECK, NULL, NULL) == BST_UNCHECKED);
						global::webview_main.post_message(L"editor-graph", L"updateCurvePath");
					}
				}
				return TRUE;
			}
			}
			switch (HIWORD(wparam)) {
			case LBN_SELCHANGE:
				update_buttons(curve);
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}