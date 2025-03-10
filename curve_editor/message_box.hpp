#pragma once

#include "constants.hpp"
#include <strconv2.h>
#include <Windows.h>



namespace curve_editor::util {
	enum class MessageBoxIcon : UINT {
		None = 0x0000L,
		Error = 0x0010L,
		Question = 0x0020L,
		Warning = 0x0030L,
		Information = 0x0040L
	};

	enum class MessageBoxButton : UINT {
		Ok = 0x0000L,
		OkCancel = 0x0001L,
		AbortRetryIgnore = 0x0002L,
		YesNoCancel = 0x0003L,
		YesNo = 0x0004L,
		RetryCancel = 0x0005L,
		CancelRetryContinue = 0x0006L,
		Help = 0x4000L
	};

	inline int message_box(
		const std::wstring_view& text,
		HWND hwnd = NULL,
		MessageBoxIcon icon_type = MessageBoxIcon::Information,
		MessageBoxButton button_type = MessageBoxButton::Ok,
		const std::wstring_view& caption = global::PLUGIN_DISPLAY_NAME
	) noexcept {
		return ::MessageBoxW(hwnd, text.data(), caption.data(), (UINT)icon_type | (UINT)button_type);
	}
} // namespace curve_editor::util