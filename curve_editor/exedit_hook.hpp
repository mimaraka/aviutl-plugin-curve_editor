#pragma once

#include <aviutl.hpp>
#include <mkaul/exedit.hpp>



namespace curve_editor::global {
	inline class ExeditHook {
	private:
		static constexpr uint32_t OFFSET_ANM_PARAM_DIALOG_PROC = 0x3455;

		static INT_PTR __stdcall anm_param_dialog_proc_hooked(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
		inline static WNDPROC setting_dialog_proc_orig = nullptr;
		inline static DLGPROC anm_param_dialog_proc_orig = nullptr;


	public:
		bool hook_anm_param_dialog_proc();
	} exedit_hook;
} // namespace curve_editor::global