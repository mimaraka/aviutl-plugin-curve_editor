#include "curve_script.hpp"
#include <FastNoiseLite.h>
#include <sol/sol.hpp>
#include <strconv2.h>



namespace curve_editor {
	double ScriptCurve::curve_function(double t, double st, double ed) const noexcept {
		auto my_panic = [](sol::optional<std::string> optional_message) {
			if (optional_message) {
				std::cout << std::format("\033[31m[{}] An error occurred: {}\033[m", ::wide_to_sjis(global::PLUGIN_DISPLAY_NAME.data()), optional_message.value()) << std::endl;
			}
			else {
				std::cout << std::format("\033[31m[{}] An unknown error occurred.\033[m", ::wide_to_sjis(global::PLUGIN_DISPLAY_NAME.data())) << std::endl;
			}
		};
		sol::state lua{sol::c_call<decltype(+my_panic), +my_panic>};
		lua.open_libraries(sol::lib::base, sol::lib::math);
		lua["t"] = t;
		lua["st"] = st;
		lua["ed"] = ed;
		auto noise = [t](
			double amp = 1.,
			float freq = 1.,
			double phase = 0.,
			int octaves = 1,
			int seed = 0
		) {
			FastNoiseLite n;
			n.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
			n.SetFractalType(FastNoiseLite::FractalType_FBm);
			n.SetSeed(seed);
			n.SetFrequency(freq);
			n.SetFractalOctaves(octaves);
			return amp * n.GetNoise(t - phase, 0.);
		};
		lua.set_function("noise", sol::overload(
			noise,
			[noise](double amp, float freq, double phase, int octaves) { return noise(amp, freq, phase, octaves); },
			[noise](double amp, float freq, double phase) { return noise(amp, freq, phase); },
			[noise](double amp, float freq) { return noise(amp, freq); },
			[noise](double amp) { return noise(amp); },
			[noise]() mutable { return noise(); }
		));
		try {
			auto ret = lua.script(::wide_to_utf8(script_)).get<double>();
			return ret;
		}
		catch (sol::error&) {
			return (ed - st) * t + st;
		}
	}

	nlohmann::json ScriptCurve::create_json() const noexcept {
		auto data = Curve::create_json();
		data["script"] = ::wide_to_utf8(script_);
		return data;
	}

	bool ScriptCurve::load_json(const nlohmann::json& data) noexcept {
		try {
			script_ = ::utf8_to_wide(data.at("script").get<std::string>());
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}
} // namespace curve_editor