#include "exedit_hook.hpp"
#include "global.hpp"
#include <mkaul/exedit.hpp>
#include <mkaul/util_hook.hpp>



namespace cved {
	namespace global {
		// 設定ダイアログのウィンドウプロシージャのフック
		bool ExeditHook::hook_objdialog_proc()
		{
			if (global::exedit_internal.base_address()) {
				return mkaul::replace_var(global::exedit_internal.p_wndproc_objdialog(), &objdialog_proc_hooked);
			}
			return false;
		}


		// 設定ダイアログのウィンドウプロシージャ(フック後)
		LRESULT CALLBACK ExeditHook::objdialog_proc_hooked(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
		{
			// TODO: 処理を書く
			return (global::exedit_internal.get_wndproc_objdialog())(hwnd, message, wparam, lparam);
		}
	}
}