#pragma once

#include <functional>
#include <string>
#include <Windows.h>



namespace curve_editor::util {
	void input_box(
		HWND hwnd,
		const char* prompt,
		const char* caption,
		std::function<bool(HWND, const std::string&)> on_submit,
		const char* default_text = ""
	);
} // namespace curve_editor::util