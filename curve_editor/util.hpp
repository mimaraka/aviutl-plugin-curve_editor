#pragma once

#include <Windows.h>
#include <mkaul/include/point.hpp>
#include "constants.hpp"



namespace cved {
	namespace util {
		void get_cutoff_line(mkaul::Point<float>* pt, const mkaul::Point<float>& start, const mkaul::Point<float>& end, float length);

		bool copy_to_clipboard(HWND hwnd, const char* str) noexcept;

		int16_t get_track_script_idx() noexcept;
	}
}