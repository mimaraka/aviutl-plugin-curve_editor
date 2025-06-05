#pragma once

#include "constants.hpp"
#include <format>
#include <mkaul/point.hpp>
#include <Windows.h>



namespace curve_editor::util {
	bool copy_to_clipboard(HWND hwnd, const std::wstring& str) noexcept;
	inline HINSTANCE get_hinst() noexcept {
		return ::GetModuleHandleA(std::format("{}.{}", global::PLUGIN_NAME, global::PLUGIN_EXT).c_str());
	}
} // namespace curve_editor::util