#include "filter_wndproc.hpp"
#include "actctx_manager.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "global.hpp"
#include "wndproc_main.hpp"



namespace cved {
	// キー押下時の処理
	LRESULT on_keydown(WPARAM wparam) {
		switch (wparam) {
			// [R]
		case 82:
			if (global::window_editor.get_hwnd())
				::SendMessage(global::window_editor.get_hwnd(), WM_COMMAND, (WPARAM)WindowCommand::Reverse, 0);
			return 0;

			// [C]
		case 67:
			if (::GetAsyncKeyState(VK_CONTROL) < 0 and global::config.get_edit_mode() == EditMode::Bezier)
				::SendMessage(global::window_editor.get_hwnd(), WM_COMMAND, (WPARAM)WindowCommand::Copy, 0);
			return 0;

			// [S]
		case 83:
			if (::GetAsyncKeyState(VK_CONTROL) < 0)
				::SendMessage(global::window_editor.get_hwnd(), WM_COMMAND, (WPARAM)WindowCommand::Save, 0);
			return 0;

			// [<]
		case VK_LEFT:
			if (global::config.get_edit_mode() == EditMode::Normal and global::window_menu.get_hwnd())
				::SendMessage(global::window_menu.get_hwnd(), WM_COMMAND, (WPARAM)WindowCommand::IdBack, 0);
			return 0;

			// [>]
		case VK_RIGHT:
			if (global::config.get_edit_mode() == EditMode::Normal and global::window_menu.get_hwnd())
				::SendMessage(global::window_menu.get_hwnd(), WM_COMMAND, (WPARAM)WindowCommand::IdNext, 0);
			return 0;

			// [Home]
		case VK_HOME:
			if (global::window_editor.get_hwnd())
				::SendMessage(global::window_editor.get_hwnd(), WM_COMMAND, (WPARAM)WindowCommand::Fit, 0);
			return 0;

			// [A]
		case 65:
			global::config.set_align_handle(!global::config.get_align_handle());
			return 0;

			// [Delete]
		case VK_DELETE:
			if (global::window_editor.get_hwnd())
				::SendMessage(global::window_editor.get_hwnd(), WM_COMMAND, (WPARAM)WindowCommand::Clear, 0);
			return 0;
		}
		return 0;
	}


	// ウィンドウプロシージャ
	BOOL filter_wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
	{
		using WindowMessage = AviUtl::FilterPlugin::WindowMessage;
		static ActCtxManager actctx_manager;
		RECT rect_wnd;
		::GetClientRect(hwnd, &rect_wnd);

		switch (message) {
			// ウィンドウ作成時
		case WindowMessage::Init:
			global::editp = editp;

			actctx_manager.init(fp->dll_hinst);

			// WS_CLIPCHILDRENを追加
			::SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) | WS_CLIPCHILDREN);

			// Alpha版・Beta版の場合はキャプションを変更
			if (global::PLUGIN_VERSION.is_preview()) {
				::SetWindowTextA(
					hwnd,
					std::format("{} ({})", global::PLUGIN_NAME, global::PLUGIN_VERSION.preview_type().str(true)).c_str()
				);
			}

			global::window_main.create(
				global::fp->dll_hinst,
				hwnd,
				"WINDOW_MAIN",
				"WINDOWCLS_MAIN",
				wndproc_main,
				WS_CHILD,
				NULL,
				rect_wnd
			);
			return 0;

			// ウィンドウ破棄時
		case WM_CLOSE:
		case WM_DESTROY:
			global::window_main.close();
			actctx_manager.exit();
			return 0;

		case WM_SIZE:
		{
			RECT rect_wnd_main = rect_wnd;
			rect_wnd_main.right = std::max((int)rect_wnd_main.right, global::MIN_WIDTH);
			rect_wnd_main.bottom = std::max((int)rect_wnd_main.bottom, global::SEPARATOR_WIDTH * 2 + global::MENU_HEIGHT);
			global::window_main.move(rect_wnd_main);
			return 0;
		}

		case WM_KEYDOWN:
			return on_keydown(wparam);

		case WM_COMMAND:
			switch (wparam) {
			case (UINT)WindowCommand::RedrawAviutl:
				return TRUE;

			case (UINT)WindowCommand::Update:
				global::window_main.send_command((WPARAM)WindowCommand::Update);
				break;
			}
			return 0;
		}
		return 0;
	}
}