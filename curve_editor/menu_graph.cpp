#include "menu_graph.hpp"
#include "config.hpp"
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
	}

	bool GraphMenu::callback(WPARAM wparam, LPARAM lparam) noexcept {
		switch (LOWORD(wparam)) {
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

		default:
			return false;
		}
		return true;
	}
}