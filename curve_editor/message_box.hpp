#pragma once

#include "constants.hpp"
#include <Windows.h>



namespace cved {
	namespace util {
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
			const char* text,
			HWND hwnd = NULL,
			MessageBoxIcon icon_type = MessageBoxIcon::Information,
			MessageBoxButton button_type = MessageBoxButton::Ok,
			const char* caption = global::PLUGIN_NAME
		) noexcept {
			return ::MessageBoxA(hwnd, text, caption, (UINT)icon_type | (UINT)button_type);
		}
	}
}