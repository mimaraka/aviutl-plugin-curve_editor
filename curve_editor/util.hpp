#pragma once

#include <Windows.h>
#include <mkaul/point.hpp>
#include "constants.hpp"



namespace cved {
	namespace util {
		bool copy_to_clipboard(HWND hwnd, const char* str) noexcept;

		int16_t get_track_script_idx() noexcept;
	}
}