#pragma once

#include <Windows.h>
#include <mkaul/include/point.hpp>
#include "constants.hpp"



namespace cved {
	namespace util {
		enum class PopupIcon : UINT {
			None = 0x0000L,
			Error = 0x0010L,
			Question = 0x0020L,
			Warning = 0x0030L,
			Information = 0x0040L
		};

		enum class PopupButton : UINT {
			Ok = 0x0000L,
			OkCancel = 0x0001L,
			AbortRetryIgnore = 0x0002L,
			YesNoCancel = 0x0003L,
			YesNo = 0x0004L,
			RetryCancel = 0x0005L,
			CancelRetryContinue = 0x0006L,
			Help = 0x4000L
		};


		inline int show_popup(
			const char* text,
			PopupIcon icon_type = PopupIcon::Information,
			PopupButton button_type = PopupButton::Ok,
			const char* caption = global::PLUGIN_NAME,
			HWND hwnd = NULL
		) {
			return ::MessageBoxA(hwnd, text, caption, (UINT)icon_type | (UINT)button_type);
		}

		void get_cutoff_line(mkaul::Point<float>* pt, const mkaul::Point<float>& start, const mkaul::Point<float>& end, float length);

		bool copy_to_clipboard(HWND hwnd, const char* str) noexcept;

		int16_t get_track_script_idx() noexcept;
	}
}