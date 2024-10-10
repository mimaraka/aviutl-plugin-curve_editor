#pragma once

#include <aviutl.hpp>
#include <mkaul/exedit.hpp>



namespace cved {
	namespace global {
		inline class ExeditHook {
		private:
			static LRESULT CALLBACK objdialog_proc_hooked(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

		public:
			bool hook_objdialog_proc();
		} exedit_hook;
	}
}