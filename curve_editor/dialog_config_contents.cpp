#include "dialog_config_contents.hpp"
#include "dlgproc_config_general.hpp"
#include "dlgproc_config_appearance.hpp"
#include "dlgproc_config_behavior.hpp"
#include "dlgproc_config_editing.hpp"
#include "global.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	ConfigDialogContents::ConfigDialogContents() noexcept {
		using StringId = global::StringTable::StringId;

		// カテゴリの情報を配列に追加
		categories_.emplace_back(
			CategoryInfo{
				ID_CATEGORY_GENERAL,
				global::string_table[StringId::LabelPreferenceGeneral],
				IDD_CONFIG_GENERAL,
				dlgproc_config_general
			}
		);
		categories_.emplace_back(
			CategoryInfo{
				ID_CATEGORY_APPEARANCE,
				global::string_table[StringId::LabelPreferenceAppearance],
				IDD_CONFIG_APPEARANCE,
				dlgproc_config_appearance
			}
		);
		categories_.emplace_back(
			CategoryInfo{
				ID_CATEGORY_BEHAVIOR,
				global::string_table[StringId::LabelPreferenceBehavior],
				IDD_CONFIG_BEHAVIOR,
				dlgproc_config_behavior
			}
		);
		categories_.emplace_back(
			CategoryInfo{
				ID_CATEGORY_EDITING,
				global::string_table[StringId::LabelPreferenceEditing],
				IDD_CONFIG_EDITING,
				dlgproc_config_editing
			}
		);
	}

	bool ConfigDialogContents::init(HWND hwnd) noexcept {
		hwnd_list_ = ::GetDlgItem(hwnd, IDC_LIST_CONFIG);
		RECT rect_child = { 100, 7, 400, 217 };
		::MapDialogRect(hwnd, &rect_child);

		for (auto& category : categories_) {
			// 項目の追加
			::SendMessageA(
				hwnd_list_,
				LB_ADDSTRING,
				NULL,
				(LPARAM)category.name
			);
			// 項目へのIDの紐づけ
			::SendMessageA(
				hwnd_list_,
				LB_SETITEMDATA,
				(WPARAM)(::SendMessageA(hwnd_list_, LB_GETCOUNT, NULL, NULL) - 1),
				(LPARAM)category.id
			);
			// 子ウィンドウの生成
			category.hwnd = ::CreateDialogA(
				global::fp->dll_hinst,
				MAKEINTRESOURCEA(category.resource),
				hwnd,
				category.proc
			);
			if (!category.hwnd) return false;
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
		::SendMessageA(hwnd_list_, LB_SETCURSEL, 0, NULL);
		update();
		return true;
	}

	void ConfigDialogContents::update() noexcept {
		int index = ::SendMessageA(hwnd_list_, LB_GETCURSEL, NULL, NULL);
		for (const auto& category : categories_) {
			::ShowWindow(category.hwnd, SW_HIDE);
		}
		if (index != LB_ERR) {
			int id = ::SendMessageA(hwnd_list_, LB_GETITEMDATA, (WPARAM)index, NULL);
			if (id != LB_ERR) {
				for (const auto& category : categories_) {
					if (category.id == id) {
						::ShowWindow(category.hwnd, SW_SHOW);
					}
				}
			}
		}
	}
}