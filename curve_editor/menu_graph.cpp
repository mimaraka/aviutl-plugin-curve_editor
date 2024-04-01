#include "menu_graph.hpp"
#include "config.hpp"
#include "curve_editor.hpp"
#include "dialog_curve_discretization.hpp"
#include "global.hpp"
#include "resource.h"



namespace cved {
	GraphMenu::GraphMenu(HINSTANCE hinst) {
		menu_ = ::GetSubMenu(::LoadMenuA(hinst, MAKEINTRESOURCEA(IDR_MENU_GRAPH)), 0);
	}

	void GraphMenu::update_state() noexcept {
		MENUITEMINFOA minfo_tmp;
		minfo_tmp.cbSize = sizeof(MENUITEMINFOA);
		minfo_tmp.fMask = MIIM_STATE;
		minfo_tmp.fState = global::config.get_align_handle() ? MFS_CHECKED : MFS_UNCHECKED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_ALIGN, FALSE, &minfo_tmp);
		minfo_tmp.fState = global::config.get_show_handle() ? MFS_CHECKED : MFS_UNCHECKED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_SHOWHANDLE, FALSE, &minfo_tmp);
		minfo_tmp.fState = global::config.get_show_velocity_graph() ? MFS_CHECKED : MFS_UNCHECKED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_VELOCITY, FALSE, &minfo_tmp);
		// 標準モード以外は離散化のメニューを無効にする
		// TODO: 値指定モードへの対応
		minfo_tmp.fState = global::config.get_edit_mode() == EditMode::Normal ? MFS_ENABLED : MFS_DISABLED;
		::SetMenuItemInfoA(menu_, ID_GRAPH_DESCRITIZATION, FALSE, &minfo_tmp);
	}

	bool GraphMenu::callback(uint16_t id) noexcept {
		switch (id) {
		case ID_GRAPH_REVERSE:
			global::window_grapheditor.send_command((WPARAM)WindowCommand::Reverse);
			break;

		case ID_GRAPH_ALIGN:
			global::config.set_align_handle(!global::config.get_align_handle());
			global::window_grapheditor.send_command((WPARAM)WindowCommand::Update);
			break;

		case ID_GRAPH_SHOWHANDLE:
			global::config.set_show_handle(!global::config.get_show_handle());
			global::window_grapheditor.send_command((WPARAM)WindowCommand::Update);
			break;

		case ID_GRAPH_VELOCITY:
			global::config.set_show_velocity_graph(!global::config.get_show_velocity_graph());
			global::window_grapheditor.send_command((WPARAM)WindowCommand::Update);
			break;

		case ID_GRAPH_DESCRITIZATION:
		{
			CurveDiscretizationDialog dialog;
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