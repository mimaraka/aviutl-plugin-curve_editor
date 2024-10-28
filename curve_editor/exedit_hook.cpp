#include "constants.hpp"
#include "curve_editor.hpp"
#include "exedit_hook.hpp"
#include "global.hpp"
#include "my_webview2_reference.hpp"
#include "window_select_curve.hpp"
#include <mkaul/exedit.hpp>
#include <mkaul/util_hook.hpp>
#include <sol/sol.hpp>



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
			constexpr size_t BUFFER_SIZE = 128;
			static SelectCurveWindow wnd_select_curve;
			static mkaul::ui::Window select_dialog;
			static int item_idx = 0;

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
						item_idx = i;
						auto mode_param = std::make_pair(EditMode::Normal, 1);
						char buffer[BUFFER_SIZE];
						::GetDlgItemTextA(hwnd, 170 + i, buffer, BUFFER_SIZE);
						::EnableWindow(hwnd, FALSE);
						sol::state lua;
						try {
							lua.script(std::format("t={}", buffer));
							auto mode_str = lua["t"]["mode"].get_or<std::string>(global::CURVE_NAME_NORMAL);
							auto param = lua["t"]["param"].get_or<int, int>(1);
							auto mode = global::editor.get_mode(mode_str);
							if (mode != EditMode::NumEditMode) {
								mode_param = std::make_pair(mode, param);
							}
						}
						catch (const sol::error&) {}
						wnd_select_curve.create(hwnd, &mode_param);
						break;
					}
				}
				switch (wparam) {
				case (WPARAM)WindowCommand::SelectCurveOk:
				{
					auto info = std::bit_cast<std::pair<std::string, int>*>(lparam);
					if (info and 0 < item_idx) {
						::SetDlgItemTextA(
							hwnd,
							170 + item_idx,
							std::format("{{mode=\"{}\",param={}}}", info->first, info->second).c_str()
						);
					}
					break;
				}

				case (WPARAM)WindowCommand::SelectCurveClose:
					::EnableWindow(hwnd, TRUE);
					::SetActiveWindow(hwnd);
					break;
				}
				break;
			}
			return anm_param_dialog_proc_orig(hwnd, message, wparam, lparam);
		}
	} // namespace global
} // namespace cved