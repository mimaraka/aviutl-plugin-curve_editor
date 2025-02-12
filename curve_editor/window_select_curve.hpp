#pragma once

#include "enum.hpp"
#include <mkaul/window.hpp>
#include <utility>



namespace curve_editor {
	using ModeParamPair = std::pair<EditMode, int>;

	class SelectCurveWindow : public mkaul::ui::Window {
		HWND hwnd_parent_ = nullptr;
		static LRESULT CALLBACK wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	public:
		HWND create(HWND hwnd, const ModeParamPair* mode_param = nullptr) noexcept;
	};
} // namespace curve_editor