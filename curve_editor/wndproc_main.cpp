#include <windowsx.h>
#include <mkaul/rectangle.hpp>
#include <mkaul/graphics.hpp>
#include "wndproc_main.hpp"
#include "wndproc_grapheditor.hpp"
#include "wndproc_toolbar.hpp"
#include "wndproc_library.hpp"
#include "global.hpp"
#include "config.hpp"
#include "constants.hpp"
#include "my_messagebox.hpp"
#include "string_table.hpp"



namespace cved {
	// メインウィンドウのウィンドウプロシージャ
	LRESULT CALLBACK wndproc_main(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
	{
		using StringId = global::StringTable::StringId;

		static bool is_separator_moving = false;
		POINT pt_client = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
		
		mkaul::WindowRectangle rect_sepr, rect_menu, rect_editor, rect_library;
		const bool is_vertical = global::config.get_layout_mode() == LayoutMode::Vertical;

		// Graphics
		using namespace mkaul::graphics;
		static std::unique_ptr<Graphics> p_graphics = Factory::create_graphics();

		RECT rect_wnd;
		::GetClientRect(hwnd, &rect_wnd);

		// セパレータの矩形を設定
		if (is_vertical) {
			rect_sepr.set(
				0,
				std::max((int)rect_wnd.bottom - global::config.get_separator() - global::SEPARATOR_WIDTH, global::MENU_HEIGHT),
				rect_wnd.right,
				std::max(
					(int)rect_wnd.bottom - global::config.get_separator() + global::SEPARATOR_WIDTH,
					(int)global::MENU_HEIGHT + 2 * global::SEPARATOR_WIDTH
				)
			);
		}
		else {
			rect_sepr.set(
				std::max((int)rect_wnd.right - global::config.get_separator() - global::SEPARATOR_WIDTH, global::MIN_WIDTH),
				0,
				std::max(
					(int)rect_wnd.right - global::config.get_separator() + global::SEPARATOR_WIDTH,
					global::MIN_WIDTH + 2 * global::SEPARATOR_WIDTH
				),
				rect_wnd.bottom
			);
		}

		// メニューの矩形を設定
		rect_menu.set(
			0, 0,
			is_vertical ? rect_wnd.right : rect_sepr.left,
			global::MENU_HEIGHT
		);

		// エディタウィンドウの矩形を設定
		rect_editor.set(
			0,
			global::MENU_HEIGHT,
			is_vertical ? rect_wnd.right : rect_sepr.left,
			is_vertical ? rect_sepr.top : rect_wnd.bottom
		);
		rect_editor.set_margin(global::MARGIN, 0, global::MARGIN, global::MARGIN);

		// ライブラリウィンドウの矩形を設定
		rect_library.set(
			is_vertical ? 0 : rect_sepr.right,
			is_vertical ? rect_sepr.bottom : 0,
			rect_wnd.right,
			rect_wnd.bottom
		);

		switch (message) {
		case WM_CREATE:
			if (!p_graphics->init(hwnd)) {
				my_messagebox(global::string_table[StringId::ErrorGraphicsInitFailed], hwnd, MessageBoxIcon::Error);
				::DestroyWindow(hwnd);
				return 0;
			}

			// グラフエディタウィンドウ
			global::window_grapheditor.create(
				global::fp->dll_hinst,
				hwnd,
				"WINDOW_EDITOR",
				"CVED_WND_EDITOR",
				wndproc_grapheditor,
				WS_CHILD,
				CS_DBLCLKS,
				rect_editor
			);

			// 上部メニュー
			global::window_toolbar.create(
				global::fp->dll_hinst,
				hwnd,
				"WINDOW_MENU",
				"CVED_WND_MENU",
				wndproc_toolbar,
				WS_CHILD,
				NULL,
				rect_menu
			);

			// プリセットパネル
			global::window_library.create(
				global::fp->dll_hinst,
				hwnd,
				"WINDOW_LIBRARY",
				"CVED_WND_LIBRARY",
				wndproc_library,
				WS_CHILD,
				NULL,
				rect_library
			);

			return 0;

		case WM_CLOSE:
		case WM_DESTROY:
			global::window_toolbar.close();
			global::window_grapheditor.close();
			global::window_library.close();
			p_graphics->release();
			return 0;

		// セパレータを描画
		case WM_PAINT:
		{
			const mkaul::Point<float> line_start = {
				is_vertical ?
					(rect_sepr.right + rect_sepr.left) * .5f - global::SEPARATOR_LINE_LENGTH :
					(float)(rect_sepr.left + global::SEPARATOR_WIDTH),
				is_vertical ?
					(float)(rect_sepr.top + global::SEPARATOR_WIDTH) :
					(rect_sepr.top + rect_sepr.bottom) * .5f - global::SEPARATOR_LINE_LENGTH
			};

			const mkaul::Point<float> line_end = {
				is_vertical ?
					(rect_sepr.right + rect_sepr.left) * .5f + global::SEPARATOR_LINE_LENGTH :
					(float)(rect_sepr.left + global::SEPARATOR_WIDTH),
				is_vertical ?
					(float)(rect_sepr.top + global::SEPARATOR_WIDTH) :
					(rect_sepr.top + rect_sepr.bottom) * .5f + global::SEPARATOR_LINE_LENGTH
			};

			const mkaul::graphics::Stroke stroke{global::SEPARATOR_LINE_WIDTH};

			p_graphics->begin_draw();
			p_graphics->fill_background(global::config.get_theme().bg);
			p_graphics->draw_line(
				line_start,
				line_end,
				global::config.get_theme().separator,
				stroke
			);

			p_graphics->end_draw();
			return 0;
		}

		case WM_SIZE:
			p_graphics->resize();
			global::window_toolbar.move(rect_menu);
			global::window_grapheditor.move(rect_editor);
			global::window_library.move(rect_library);
			return 0;

		// 左クリック時
		case WM_LBUTTONDOWN:
		{
			if (rect_sepr.pt_in_rect(pt_client)) {
				is_separator_moving = true;
				if (global::config.get_layout_mode() == LayoutMode::Vertical)
					::SetCursor(LoadCursor(NULL, IDC_SIZENS));
				else
					::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
				::SetCapture(hwnd);
			}
		}
			return 0;

		// 左クリック終了時
		case WM_LBUTTONUP:
			is_separator_moving = false;
			::ReleaseCapture();
			return 0;

		// マウスが動いたとき
		case WM_MOUSEMOVE:
			if (rect_sepr.pt_in_rect(pt_client)) {
				if (is_vertical)
					::SetCursor(LoadCursor(NULL, IDC_SIZENS));
				else
					::SetCursor(LoadCursor(NULL, IDC_SIZEWE));
			}
			if (is_separator_moving) {
				if (is_vertical) {
					global::config.set_separator(mkaul::clamp(
						(int)(rect_wnd.bottom - pt_client.y),
						global::SEPARATOR_WIDTH,
						(int)rect_wnd.bottom - global::SEPARATOR_WIDTH - global::MENU_HEIGHT
					));
				}
				else {
					global::config.set_separator(mkaul::clamp(
						(int)(rect_wnd.right - pt_client.x),
						global::SEPARATOR_WIDTH,
						(int)rect_wnd.right - global::MIN_WIDTH + global::SEPARATOR_WIDTH
					));
				}
				::SendMessageA(hwnd, WM_COMMAND, (WPARAM)WindowCommand::MoveWindow, NULL);
			}
			return 0;

		case WM_COMMAND:
			switch (wparam) {
			case (WPARAM)WindowCommand::Update:
				global::window_grapheditor.send_command((WPARAM)WindowCommand::Update);
				global::window_toolbar.send_command((WPARAM)WindowCommand::Update);
				global::window_main.redraw();
				break;

			case (WPARAM)WindowCommand::MoveWindow:
				global::window_toolbar.move(rect_menu);
				global::window_grapheditor.move(rect_editor);
				global::window_library.move(rect_library);
				break;
			}
			return 0;

		default:
			return ::DefWindowProc(hwnd, message, wparam, lparam);
		}
	}
}