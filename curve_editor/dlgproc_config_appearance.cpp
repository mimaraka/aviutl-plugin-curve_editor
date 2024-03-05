#include "dlgproc_config_appearance.hpp"
#include "config.hpp"
#include "enum.hpp"
#include "string_table.hpp"
#include "resource.h"



namespace cved {
	INT_PTR CALLBACK dlgproc_config_appearance(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		using StringId = global::StringTable::StringId;
		static HWND combo_theme, edit_curve_thickness, edit_curve_drawing_interval;
		
		switch (message) {
		case WM_INITDIALOG:
			combo_theme = ::GetDlgItem(hwnd, IDC_COMBO_THEME);
			edit_curve_thickness = ::GetDlgItem(hwnd, IDC_EDIT_CURVE_THICKNESS);
			edit_curve_drawing_interval = ::GetDlgItem(hwnd, IDC_EDIT_CURVE_DRAWING_INTERVAL);

			for (uint32_t i = 0u; i < (uint32_t)ThemeId::NumThemeId; i++) {
				::SendMessageA(
					combo_theme,
					CB_ADDSTRING,
					NULL,
					(LPARAM)global::string_table[(StringId)((uint32_t)StringId::LabelThemeNameDark + i)]
				);
			}
			::SendMessageA(combo_theme, CB_SETCURSEL, (WPARAM)global::config.get_theme_id(), NULL);
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {

			}
			break;
		}
		return 0;
	}
}