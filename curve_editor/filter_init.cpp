#include "actctx_helper.hpp"
#include "config.hpp"
#include "curve_editor.hpp"
#include "dialog_warning_autosaver.hpp"
#include "exedit_hook.hpp"
#include "filter_init.hpp"
#include "global.hpp"
#include "message_box.hpp"
#include "preset_manager.hpp"
#include "resource.h"
#include "string_table.hpp"
#include "update_checker.hpp"
#include <CommCtrl.h>
#include <mkaul/aviutl.hpp>
#include <strconv2.h>



namespace cved {
	// AviUtl起動時の処理
	BOOL filter_init(AviUtl::FilterPlugin* fp) {
		using StringId = global::StringTable::StringId;

		// jsonから設定の読み込み
		global::config.load_json();

		// アップデートのチェック
		if (global::config.get_notify_update()) {
			global::update_checker.check_for_update();
		}

		// 保存されたカーブの読み込み
		global::editor.editor_graph().load_codes(
			global::config.get_curve_code_bezier(),
			global::config.get_curve_code_elastic(),
			global::config.get_curve_code_bounce()
		);

		// プリセットの読み込み
		global::preset_manager.load_json();

		// アクティベーションコンテキスト(開始)
		ActCtxHelper actctx_helper;
		actctx_helper.init();

		// exedit.auf(ver.0.92)存在確認
		if (!global::exedit_internal.init(fp)) {
			// 拡張編集が存在しませんのエラー
			util::message_box(global::string_table[StringId::ErrorExeditNotFound], NULL, util::MessageBoxIcon::Error);
			return FALSE;
		}

		// アニメーション効果パラメータ設定ダイアログのフック処理
		if (!global::exedit_hook.hook_anm_param_dialog_proc()) {
			util::message_box(global::string_table[StringId::ErrorExeditHookFailed], NULL, util::MessageBoxIcon::Error);
			return FALSE;
		}

		// コモンコントロールの初期化
		INITCOMMONCONTROLSEX icc{
			.dwSize = sizeof(INITCOMMONCONTROLSEX),
			// SysLinkの有効化
			.dwICC = ICC_LINK_CLASS
		};
		if (!::InitCommonControlsEx(&icc)) {
			util::message_box(global::string_table[StringId::ErrorCommCtrlInitFailed], NULL, util::MessageBoxIcon::Error);
			return FALSE;
		}

		// スクリプトファイルの存在確認
		//char path_str[MAX_PATH];
		//::GetModuleFileNameA(global::exedit_internal.fp()->dll_hinst, path_str, MAX_PATH);
		//std::filesystem::path path_exedit{ path_str };
		//auto dir_exedit = path_exedit.parent_path();
		//WIN32_FIND_DATA find_data;
		//auto find_lua = ::FindFirstFileA((dir_exedit / "curve_editor.lua").string().c_str(), &find_data);
		//auto find_tra = ::FindFirstFileA((dir_exedit / "script" / "@Curve Editor.tra").string().c_str(), &find_data);
		//auto find_tra_sub = ::FindFirstFileA((dir_exedit / "script" / "mimaraka" / "*.tra").string().c_str(), &find_data);
		//if (find_lua == INVALID_HANDLE_VALUE or (find_tra == INVALID_HANDLE_VALUE and find_tra_sub == INVALID_HANDLE_VALUE)) {
		//	// スクリプトファイルが見つかりませんの警告
		//	util::message_box(global::string_table[StringId::WarningScriptFileNotFound], NULL, util::MessageBoxIcon::Warning);
		//}

		// autosaverがインストールされていない場合の警告
		if (!mkaul::aviutl::get_fp_by_name(fp, "autosaver.auf") and !global::config.get_ignore_autosaver_warning()) {
			AutosaverWarningDialog dialog;
			::MessageBeep(MB_ICONWARNING);
			dialog.show(fp->hwnd_parent);
		}

		// アクティベーションコンテキスト(終了)
		actctx_helper.exit();

		return TRUE;
	}
} // namespace cved