#include "dialog_preset_list_setting.hpp"
#include "preset_manager.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace curve_editor {
	int PresetListSettingDialog::resource_id() const noexcept { return IDD_PRESET_LIST_SETTING; }

	void PresetListSettingDialog::init_controls(HWND hwnd) noexcept {
		using StringId = global::StringTable::StringId;

		hwnd_combo_sort_by_ = ::GetDlgItem(hwnd, IDC_COMBO_SORT_BY);
		hwnd_radio_sort_asc_ = ::GetDlgItem(hwnd, IDC_RADIO_SORT_ASC);
		hwnd_radio_sort_desc_ = ::GetDlgItem(hwnd, IDC_RADIO_SORT_DESC);
		hwnd_check_filter_type_normal_ = ::GetDlgItem(hwnd, IDC_CHECK_FILTER_TYPE_NORMAL);
		hwnd_check_filter_type_value_ = ::GetDlgItem(hwnd, IDC_CHECK_FILTER_TYPE_VALUE);
		hwnd_check_filter_type_bezier_ = ::GetDlgItem(hwnd, IDC_CHECK_FILTER_TYPE_BEZIER);
		hwnd_check_filter_type_elastic_ = ::GetDlgItem(hwnd, IDC_CHECK_FILTER_TYPE_ELASTIC);
		hwnd_check_filter_type_bounce_ = ::GetDlgItem(hwnd, IDC_CHECK_FILTER_TYPE_BOUNCE);
		hwnd_check_filter_type_script_ = ::GetDlgItem(hwnd, IDC_CHECK_FILTER_TYPE_SCRIPT);

		::SendMessageA(hwnd_combo_sort_by_, CB_ADDSTRING, NULL, (LPARAM)global::string_table[StringId::LabelSortByNull]);
		::SendMessageA(hwnd_combo_sort_by_, CB_ADDSTRING, NULL, (LPARAM)global::string_table[StringId::LabelSortByName]);
		::SendMessageA(hwnd_combo_sort_by_, CB_ADDSTRING, NULL, (LPARAM)global::string_table[StringId::LabelSortByDate]);

		::SendMessageA(hwnd_combo_sort_by_, CB_SETCURSEL, (WPARAM)global::preset_manager.get_sort_by(), NULL);

		if (global::preset_manager.get_sort_order() == global::PresetManager::SortOrder::Asc) {
			::SendMessageA(hwnd_radio_sort_asc_, BM_SETCHECK, BST_CHECKED, NULL);
		}
		else {
			::SendMessageA(hwnd_radio_sort_desc_, BM_SETCHECK, BST_CHECKED, NULL);
		}

		const auto& filter_info = global::preset_manager.get_filter_info();
		::SendMessageA(hwnd_check_filter_type_normal_, BM_SETCHECK, filter_info.type_normal ? BST_CHECKED : BST_UNCHECKED, NULL);
		::SendMessageA(hwnd_check_filter_type_value_, BM_SETCHECK, filter_info.type_value ? BST_CHECKED : BST_UNCHECKED, NULL);
		::SendMessageA(hwnd_check_filter_type_bezier_, BM_SETCHECK, filter_info.type_bezier ? BST_CHECKED : BST_UNCHECKED, NULL);
		::SendMessageA(hwnd_check_filter_type_elastic_, BM_SETCHECK, filter_info.type_elastic ? BST_CHECKED : BST_UNCHECKED, NULL);
		::SendMessageA(hwnd_check_filter_type_bounce_, BM_SETCHECK, filter_info.type_bounce ? BST_CHECKED : BST_UNCHECKED, NULL);
		::SendMessageA(hwnd_check_filter_type_script_, BM_SETCHECK, filter_info.type_script ? BST_CHECKED : BST_UNCHECKED, NULL);
	}

	INT_PTR PresetListSettingDialog::dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM) {
		switch (message) {
		case WM_INITDIALOG:
			init_controls(hwnd);
			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
			case IDOK:
			{
				auto sort_by = ::SendMessageA(hwnd_combo_sort_by_, CB_GETCURSEL, NULL, NULL);
				auto sort_order = ::SendMessageA(hwnd_radio_sort_asc_, BM_GETCHECK, NULL, NULL) == BST_CHECKED ?
					global::PresetManager::SortOrder::Asc : global::PresetManager::SortOrder::Desc;
				global::preset_manager.set_sort_by(static_cast<global::PresetManager::SortBy>(sort_by));
				global::preset_manager.set_sort_order(sort_order);
				global::PresetManager::FilterInfo filter_info;
				filter_info.type_normal = ::SendMessageA(hwnd_check_filter_type_normal_, BM_GETCHECK, NULL, NULL) == BST_CHECKED;
				filter_info.type_value = ::SendMessageA(hwnd_check_filter_type_value_, BM_GETCHECK, NULL, NULL) == BST_CHECKED;
				filter_info.type_bezier = ::SendMessageA(hwnd_check_filter_type_bezier_, BM_GETCHECK, NULL, NULL) == BST_CHECKED;
				filter_info.type_elastic = ::SendMessageA(hwnd_check_filter_type_elastic_, BM_GETCHECK, NULL, NULL) == BST_CHECKED;
				filter_info.type_bounce = ::SendMessageA(hwnd_check_filter_type_bounce_, BM_GETCHECK, NULL, NULL) == BST_CHECKED;
				filter_info.type_script = ::SendMessageA(hwnd_check_filter_type_script_, BM_GETCHECK, NULL, NULL) == BST_CHECKED;
				global::preset_manager.set_filter_info(filter_info);
				::EndDialog(hwnd, 0);
				return TRUE;
			}

			case IDCANCEL:
				::EndDialog(hwnd, 1);
				return TRUE;
			}
		}
		return FALSE;
	}
} // namespace curve_editor