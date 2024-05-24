#pragma once

#include "dialog.hpp"
#include "curve_graph.hpp"



namespace cved {
	class ModifierDialog : public Dialog {
		HWND hwnd_list_modifier_ = NULL;
		HWND hwnd_button_add_ = NULL;
		HWND hwnd_button_remove_ = NULL;
		HWND hwnd_button_edit_ = NULL;
		HWND hwnd_button_rename_ = NULL;
		HWND hwnd_button_up_ = NULL;
		HWND hwnd_button_down_ = NULL;
		HMENU menu_modifier_ = NULL;

		int i_resource() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd, const GraphCurve* p_curve) noexcept;
		void update(const GraphCurve* p_curve) noexcept;
	};
}