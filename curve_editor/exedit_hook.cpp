#include "exedit_hook.hpp"
#include "global.hpp"
#include "window_select_curve.hpp"
#include <mkaul/exedit.hpp>
#include <mkaul/util_hook.hpp>



namespace cved {
	namespace global {
		// アニメーションパラメータダイアログのウィンドウプロシージャのフック
		bool ExeditHook::hook_anm_param_dialog_proc() {
			if (global::exedit_internal.base_address()) {
				anm_param_dialog_proc_orig = *global::exedit_internal.get<DLGPROC>(OFFSET_ANM_PARAM_DIALOG_PROC);
				return mkaul::replace_var(global::exedit_internal.base_address() + OFFSET_ANM_PARAM_DIALOG_PROC, &anm_param_dialog_proc_hooked);
			}
			return false;
		}

		INT_PTR CALLBACK ExeditHook::anm_param_dialog_proc_hooked(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
			static SelectCurveWindow wnd_select_curve;
			static mkaul::ui::Window select_dialog;

			switch (message) {
			case WM_INITDIALOG:
			{
				const HWND* p_hwnd_target = global::exedit_internal.get<HWND>(0x177a44);
				if (p_hwnd_target) {
					::PostMessageA(*p_hwnd_target, 0x401, 0, 0);
				}
				break;
			}

			case WM_DESTROY:
			case WM_CLOSE:
				wnd_select_curve.close();
				break;

			case WM_COMMAND:
				for (int i = 1; i < 16; i++) {
					if (wparam == MAKEWPARAM(i + 400, BN_CLICKED)) {
						wnd_select_curve.create(hwnd, &i);
						break;
					}
				}
				break;
			}
			return anm_param_dialog_proc_orig(hwnd, message, wparam, lparam);
		}
	} // namespace global
} // namespace cved