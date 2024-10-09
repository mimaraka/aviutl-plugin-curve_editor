#pragma once

#include "constants.hpp"
#include <mkaul/point.hpp>
#include <Windows.h>



namespace cved {
	namespace util {
		bool copy_to_clipboard(HWND hwnd, const char* str) noexcept;
		int16_t get_track_script_idx() noexcept;
	}
}