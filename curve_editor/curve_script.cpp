#include "curve_script.hpp"
#include <lua5_1_4_Win32_dll8_lib/include/lua.hpp>



namespace cved {
	void ScriptCurve::create_data(std::vector<byte>& data) const noexcept {
	}

	bool ScriptCurve::load_data(const byte* data, size_t size) noexcept {
		return false;
	}
}