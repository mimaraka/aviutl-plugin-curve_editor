#include "config.hpp"
#include "curve_editor.hpp"
#include "dialog_modifier.hpp"
#include "global.hpp"
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

	void GraphMenu::update_state() noexcept {
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_STATE;
		for (size_t i = 0; i < (size_t)ApplyMode::NumApplyMode; i++) {
			minfo_tmp.fState = (ApplyMode)i == global::config.get_apply_mode() ? MFS_CHECKED : MFS_UNCHECKED;
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
		minfo_tmp.fState = global::config.get_edit_mode() == EditMode::Normal ? MFS_ENABLED : MFS_DISABLED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_MODIFIER, FALSE, &minfo_tmp);
	}

	bool GraphMenu::callback(uint16_t id) noexcept {
		if (mkaul::in_range(
			id,
			(uint16_t)WindowCommand::ChangeApplyMode,
			(uint16_t)WindowCommand::ChangeApplyMode + (uint16_t)ApplyMode::NumApplyMode - 1u,
			true
		)) {
			ApplyMode new_mode = (ApplyMode)(id - (uint16_t)WindowCommand::ChangeApplyMode);
			if (new_mode != global::config.get_apply_mode()) {
				global::config.set_apply_mode(new_mode);
			}
			return true;
		}
		switch (id) {
		case ID_GRAPH_REVERSE:
		{
			auto curve = global::editor.editor_graph().current_curve();
			if (curve) {
				curve->reverse();
				::SendMessageA(global::fp->hwnd, WM_COMMAND, (WPARAM)WindowCommand::UpdateHandlePos, NULL);
			}
			break;
		}

		case ID_GRAPH_ALIGN:
			global::config.set_align_handle(!global::config.get_align_handle());
			break;

		case ID_GRAPH_SHOW_X_LABEL:
			global::config.set_show_x_label(!global::config.get_show_x_label());
			::SendMessageA(global::fp->hwnd, WM_COMMAND, (WPARAM)WindowCommand::UpdateAxisLabelVisibility, NULL);
			break;

		case ID_GRAPH_SHOW_Y_LABEL:
			global::config.set_show_y_label(!global::config.get_show_y_label());
			::SendMessageA(global::fp->hwnd, WM_COMMAND, (WPARAM)WindowCommand::UpdateAxisLabelVisibility, NULL);
			break;

		case ID_GRAPH_SHOWHANDLE:
			global::config.set_show_handle(!global::config.get_show_handle());
			::SendMessageA(global::fp->hwnd, WM_COMMAND, (WPARAM)WindowCommand::UpdateHandleVisibility, NULL);
			break;

		case ID_GRAPH_VELOCITY:
			global::config.set_show_velocity_graph(!global::config.get_show_velocity_graph());
			//global::window_grapheditor.send_command((WPARAM)WindowCommand::Update);
			break;

		case ID_GRAPH_MODIFIER:
		{
			ModifierDialog dialog;
			auto p_curve_graph = global::editor.editor_graph().current_curve();
			if (p_curve_graph) {
				dialog.show(global::fp->hwnd, reinterpret_cast<LPARAM>(p_curve_graph));
			}
			break;
		}

		default:
			return false;
		}
		return true;
	}
}