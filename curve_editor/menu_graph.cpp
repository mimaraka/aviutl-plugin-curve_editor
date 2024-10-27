#include "config.hpp"
#include "curve_editor.hpp"
#include "dialog_modifier.hpp"
#include "menu_graph.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace cved {
	GraphMenu::GraphMenu(HINSTANCE hinst) {
		using StringId = global::StringTable::StringId;

		menu_ = ::GetSubMenu(::LoadMenuA(hinst, MAKEINTRESOURCEA(IDR_MENU_GRAPH)), 0);
		
		// 適用モードのサブメニューの作成
		submenu_apply_mode_ = ::CreatePopupMenu();
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_TYPE | MIIM_ID;
		minfo_tmp.fType = MFT_RADIOCHECK;

		for (size_t i = 0; i < (size_t)ApplyMode::NumApplyMode; i++) {
			minfo_tmp.wID = (WORD)WindowCommand::ChangeApplyMode+ i;
			minfo_tmp.dwTypeData = const_cast<char*>(global::string_table[(StringId)((size_t)StringId::LabelApplyModeNormal + i)]);
			::InsertMenuItemA(submenu_apply_mode_, i, TRUE, &minfo_tmp);
		}

		// 適用モードのサブメニューの設定
		minfo_tmp.fMask = MIIM_SUBMENU;
		minfo_tmp.hSubMenu = submenu_apply_mode_;
		::SetMenuItemInfoA(menu_, ID_GRAPH_APPLYMODE, FALSE, &minfo_tmp);
	}

	void GraphMenu::update_state(EditMode mode) noexcept {
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_STATE;
		for (size_t i = 0; i < (size_t)ApplyMode::NumApplyMode; i++) {
			minfo_tmp.fState = (ApplyMode)i == global::config.get_apply_mode(mode) ? MFS_CHECKED : MFS_UNCHECKED;
			::SetMenuItemInfoA(submenu_apply_mode_, i, TRUE, &minfo_tmp);
		}
		minfo_tmp.fState = global::config.get_align_handle() ? MFS_CHECKED : MFS_UNCHECKED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_ALIGN, FALSE, &minfo_tmp);
		minfo_tmp.fState = global::config.get_show_x_label() ? MFS_CHECKED : MFS_UNCHECKED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_SHOW_X_LABEL, FALSE, &minfo_tmp);
		minfo_tmp.fState = global::config.get_show_y_label() ? MFS_CHECKED : MFS_UNCHECKED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_SHOW_Y_LABEL, FALSE, &minfo_tmp);
		minfo_tmp.fState = global::config.get_show_handle() ? MFS_CHECKED : MFS_UNCHECKED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_SHOWHANDLE, FALSE, &minfo_tmp);
		minfo_tmp.fState = global::config.get_show_velocity_graph() ? MFS_CHECKED : MFS_UNCHECKED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_VELOCITY, FALSE, &minfo_tmp);
		// 標準モード以外は離散化のメニューを無効にする
		// TODO: 値指定モードへの対応
		minfo_tmp.fState = mode == EditMode::Normal ? MFS_ENABLED : MFS_DISABLED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_MODIFIER, FALSE, &minfo_tmp);
	}

	HMENU GraphMenu::get_handle(EditMode mode) noexcept {
		update_state(mode);
		return menu_;
	}

	int GraphMenu::show(
		EditMode mode,
		uint32_t curve_id,
		const MyWebView2& webview,
		HWND hwnd,
		UINT flags,
		const mkaul::Point<LONG>* p_custom_pt_screen
	) noexcept {
		POINT tmp;
		::GetCursorPos(&tmp);
		if (p_custom_pt_screen) {
			tmp = p_custom_pt_screen->to<POINT>();
		}
		update_state(mode);
		auto ret = ::TrackPopupMenu(
			menu_,
			flags | TPM_RETURNCMD | TPM_NONOTIFY,
			tmp.x,
			tmp.y,
			0, hwnd, NULL
		);
		return callback(mode, curve_id, webview, hwnd, static_cast<uint16_t>(ret));
	}

	bool GraphMenu::callback(EditMode mode, uint32_t curve_id, const MyWebView2& webview, HWND hwnd, uint16_t id) noexcept {
		if (mkaul::in_range(
			id,
			(uint16_t)WindowCommand::ChangeApplyMode,
			(uint16_t)WindowCommand::ChangeApplyMode + (uint16_t)ApplyMode::NumApplyMode - 1u
		)) {
			ApplyMode new_mode = (ApplyMode)(id - (uint16_t)WindowCommand::ChangeApplyMode);
			if (new_mode != global::config.get_apply_mode(mode)) {
				global::config.set_apply_mode(mode, new_mode);
			}
			return true;
		}
		switch (id) {
		case ID_GRAPH_REVERSE:
		{
			auto curve = global::id_manager.get_curve<GraphCurve>(curve_id);
			if (curve) {
				curve->reverse();
				webview.post_message(L"updateHandlePos");
			}
			break;
		}

		case ID_GRAPH_MODIFIER:
		{
			ModifierDialog dialog;
			dialog.show(hwnd, static_cast<LPARAM>(curve_id));
			break;
		}

		case ID_GRAPH_ALIGN:
			global::config.set_align_handle(!global::config.get_align_handle());
			break;

		case ID_GRAPH_SHOW_X_LABEL:
			global::config.set_show_x_label(!global::config.get_show_x_label());
			webview.post_message(L"updateAxisLabelVisibility");
			break;

		case ID_GRAPH_SHOW_Y_LABEL:
			global::config.set_show_y_label(!global::config.get_show_y_label());
			webview.post_message(L"updateAxisLabelVisibility");
			break;

		case ID_GRAPH_SHOWHANDLE:
			global::config.set_show_handle(!global::config.get_show_handle());
			webview.post_message(L"updateHandleVisibility");
			break;

		case ID_GRAPH_VELOCITY:
			global::config.set_show_velocity_graph(!global::config.get_show_velocity_graph());
			webview.post_message(L"updateVelocityGraphVisibility");
			break;

		default:
			return false;
		}
		return true;
	}
}