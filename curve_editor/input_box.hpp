#pragma once

#include <functional>
#include <string>
#include <Windows.h>



namespace curve_editor::util {
	void input_box(
		HWND hwnd,
		const std::wstring_view& prompt,
		const std::wstring_view& caption,
		std::function<bool(HWND, const std::wstring&)> on_submit,
		const std::wstring_view& default_text = L""
	);
} // namespace curve_editor::util