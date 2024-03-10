#define ISOLATION_AWARE_ENABLED 1

#include "filter_init.hpp"
#include <mkaul/include/aviutl.hpp>
#include "config.hpp"
#include "curve_editor.hpp"
#include "dlgproc_warning_autosaver.hpp"
#include "exedit_hook.hpp"
#include "global.hpp"
#include "string_table.hpp"
#include "my_messagebox.hpp"
#include "resource.h"



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
			my_messagebox(global::string_table[StringId::ErrorExeditNotFound], NULL, MessageBoxIcon::Error);
			return FALSE;
		}

		// オブジェクト設定ダイアログのフック処理
		if (!global::exedit_hook.hook_objdialog_proc()) {
			my_messagebox(global::string_table[StringId::ErrorExeditHookFailed], NULL, MessageBoxIcon::Error);
			return FALSE;
		}

		// コモンコントロールの初期化
		INITCOMMONCONTROLSEX icc{
			.dwSize = sizeof(INITCOMMONCONTROLSEX),
			// SysLinkの有効化
			.dwICC = ICC_LINK_CLASS
		};
		::InitCommonControlsEx(&icc);

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
			my_messagebox(global::string_table[StringId::ErrorGraphicsInitFailed], NULL, MessageBoxIcon::Error);
			return FALSE;
		}

		// アップデートチェック
		if (global::config.get_notify_update()) {
			DWORD thread_id;
			::CreateThread(NULL, 0, check_for_updates, NULL, 0, &thread_id);
		}

		// autosaverがインストールされていない場合の警告
		if (!mkaul::aviutl::get_fp_by_name(fp, "autosaver.auf") and !global::config.get_ignore_autosaver_warning()) {
			::MessageBeep(MB_ICONWARNING);
			::IsolationAwareDialogBoxParamA(
				fp->dll_hinst,
				MAKEINTRESOURCE(IDD_WARNING_AUTOSAVER),
				fp->hwnd_parent,
				dlgproc_warning_autosaver,
				NULL
			);
		}

		return TRUE;
	}
}