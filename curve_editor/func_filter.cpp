#define NOMINMAX
#include "global.hpp"
#include "constants.hpp"
#include "config.hpp"
#include "curve_editor.hpp"
#include "version.hpp"
#include "string_table.hpp"
#include "exedit_hook.hpp"
#include "util.hpp"
#include "func_filter.hpp"
#include "wndproc_main.hpp"
#include "dlgproc_warning_autosaver.hpp"
#include "enum.hpp"
#include "resource.h"
#include <mkaul/include/aviutl.hpp>
#include <mkaul/include/exedit.hpp>
#include <aviutl/FilterPlugin.hpp>
#include <format>



namespace cved {
	// AviUtl起動時の処理
	BOOL filter_init(AviUtl::FilterPlugin* fp) {
		using StringId = global::StringTable::StringId;

		global::fp = fp;

		// jsonから設定の読み込み
		global::config.init(fp->dll_hinst);
		global::config.load_json();

		// 保存されたカーブの読み込み
		global::editor.editor_graph().load(
			global::config.get_curve_code_bezier(),
			global::config.get_curve_code_elastic(),
			global::config.get_curve_code_bounce()
		);

		// TODO: ロケールの設定
		// ここに書く

		// 文字列テーブルの読み込み
		global::string_table.load(fp->dll_hinst);

		// exedit.auf(ver.0.92)存在確認
		if (!global::exedit_internal.init(fp)) {
			// 拡張編集が存在しませんのエラー
			util::show_popup(global::string_table[StringId::ErrorExeditNotFound], util::PopupIcon::Error);
			return FALSE;
		}

		// オブジェクト設定ダイアログのフック処理
		if (!global::exedit_hook.hook_objdialog_proc()) {
			util::show_popup(global::string_table[StringId::ErrorExeditHookFailed], util::PopupIcon::Error);
			return FALSE;
		}

		//// スクリプトファイルの存在確認
		//const char* script_name_lua = "curve_editor.lua";
		//const char* script_name_tra = "@Curve editor.tra";
		//if (
		//	!std::filesystem::exists(global::config.get_aviutl_directory() / script_name_lua)
		//	and !std::filesystem::exists(global::config.get_aviutl_directory() / )
		//	) {

		//}

		// 描画環境の初期化
		if (!mkaul::graphics::Factory::startup(global::config.get_graphic_engine())) {
			util::show_popup(global::string_table[StringId::ErrorGraphicsInitFailed], util::PopupIcon::Error);
			return FALSE;
		}

		// アップデートチェック
		if (global::config.get_notify_update()) {
			DWORD thread_id;
			::CreateThread(NULL, 0, check_for_updates, NULL, 0, &thread_id);
		}

		// autosaverがインストールされていない場合の警告
		if (!mkaul::aviutl::get_fp_by_name(fp, "autosaver.auf") and !global::config.get_ignore_autosaver_warning()) {
			::DialogBoxA(fp->dll_hinst, MAKEINTRESOURCE(IDD_WARNING_AUTOSAVER), fp->hwnd_parent, dlgproc_warning_autosaver);
		}

		return TRUE;
	}


	BOOL filter_exit(AviUtl::FilterPlugin* fp) {
		// 設定をJSONに保存
		global::config.save_json();
		mkaul::graphics::Factory::shutdown();

		return TRUE;
	}


	BOOL filter_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size) {
		return TRUE;
	}


	BOOL filter_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size) {
		return TRUE;
	}

	
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
		RECT rect_wnd;
		::GetClientRect(hwnd, &rect_wnd);

		switch (message) {
			// ウィンドウ作成時
		case WindowMessage::Init:
			global::editp = editp;

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