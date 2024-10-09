#include "dialog_pref.hpp"
#include "config.hpp"
#include "enum.hpp"
#include "global.hpp"
#include "my_messagebox.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	PrefDialog::PrefDialog() {
		using StringId = global::StringTable::StringId;

		categories_.emplace_back(CategoryInfo{
			.id = ID_CATEGORY_GENERAL,
			.name =global::string_table[StringId::LabelPreferenceGeneral],
			.p_dialog = &dialog_general_ 
		});
		categories_.emplace_back(CategoryInfo{
			.id = ID_CATEGORY_APPEARANCE,
			.name = global::string_table[StringId::LabelPreferenceAppearance],
			.p_dialog = &dialog_appearance_
		});
		categories_.emplace_back(CategoryInfo{
			.id = ID_CATEGORY_BEHAVIOR,
			.name = global::string_table[StringId::LabelPreferenceBehavior],
			.p_dialog = &dialog_behavior_
		});
		categories_.emplace_back(CategoryInfo{
			.id = ID_CATEGORY_EDITING,
			.name = global::string_table[StringId::LabelPreferenceEditing],
			.p_dialog = &dialog_editing_
		});
	}


	int PrefDialog::resource_id() const noexcept { return IDD_PREF; }


	void PrefDialog::init_controls(HWND hwnd) noexcept {
		hwnd_list_categories_ = ::GetDlgItem(hwnd, IDC_LIST_CATEGORY);
		RECT rect_child = { 100, 7, 400, 207 };
		::MapDialogRect(hwnd, &rect_child);

		::SendMessageA(hwnd_list_categories_, WM_SETREDRAW, FALSE, NULL);
		for (auto& category : categories_) {
			// リストに項目を追加
			::SendMessageA(hwnd_list_categories_, LB_ADDSTRING, NULL, (LPARAM)category.name);
			// 項目へのIDの紐づけ
			::SendMessageA(
				hwnd_list_categories_,
				LB_SETITEMDATA,
				(WPARAM)(::SendMessageA(hwnd_list_categories_, LB_GETCOUNT, NULL, NULL) - 1),
				(LPARAM)category.id
			);
			category.hwnd = category.p_dialog->create(hwnd);
			// 子ウィンドウの移動
			::MoveWindow(
				category.hwnd,
				rect_child.left,
				rect_child.top,
				rect_child.right - rect_child.left,
				rect_child.bottom - rect_child.top,
				TRUE
			);
		}
		::SendMessageA(hwnd_list_categories_, WM_SETREDRAW, TRUE, NULL);
		::SendMessageA(hwnd_list_categories_, LB_SETCURSEL, 0, NULL);
	}


	void PrefDialog::update() noexcept {
		int idx = ::SendMessageA(hwnd_list_categories_, LB_GETCURSEL, NULL, NULL);
		for (const auto& category : categories_) {
			::ShowWindow(category.hwnd, SW_HIDE);
		}
		if (idx != LB_ERR) {
			int id = ::SendMessageA(hwnd_list_categories_, LB_GETITEMDATA, (WPARAM)idx, NULL);
			if (id != LB_ERR) {
				for (const auto& category : categories_) {
					if (category.id == id) {
						::ShowWindow(category.hwnd, SW_SHOW);
					}
				}
			}
		}
	}


	void PrefDialog::load_config() noexcept {
		for (const auto& category : categories_) {
			::SendMessageA(category.hwnd, WM_COMMAND, (WPARAM)WindowCommand::LoadConfig, NULL);
		}
	}


	void PrefDialog::save_config() noexcept {
		for (const auto& category : categories_) {
			::SendMessageA(category.hwnd, WM_COMMAND, (WPARAM)WindowCommand::SaveConfig, NULL);
		}
		global::config.save_json();
		::SendMessageA(global::fp->hwnd, WM_COMMAND, (WPARAM)WindowCommand::ApplyPreferences, NULL);
	}


	INT_PTR PrefDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		using StringId = global::StringTable::StringId;

		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			update();
			load_config();
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
				save_config();
				::EndDialog(hwnd, IDOK);
				return TRUE;

			case IDCANCEL:
				::EndDialog(hwnd, IDCANCEL);
				return TRUE;

			case IDC_BUTTON_APPLY:
				save_config();
				return TRUE;

			case IDC_BUTTON_RESET:
			{
				auto resp = my_messagebox(
					global::string_table[StringId::WarningResetPreferences],
					hwnd,
					MessageBoxIcon::Warning, MessageBoxButton::OkCancel
				);
				if (resp == IDOK) {
					global::config.reset_pref();
					load_config();
				}
				return TRUE;
			}
			}
			switch (HIWORD(wparam)) {
			case LBN_SELCHANGE:
				update();
				return TRUE;
			}
			break;
		}
		return FALSE;
	}
}