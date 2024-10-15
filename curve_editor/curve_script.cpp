#include "curve_script.hpp"
#include <FastNoiseLite.h>
#include <sol/sol.hpp>



namespace cved {
	double ScriptCurve::curve_function(double t, double st, double ed) const noexcept {
		sol::state lua;
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
			auto ret = lua.script(script_).get<double>();
			return ret;
		}
		catch (sol::error&) {
			return (ed - st) * t + st;
		}
	}

	nlohmann::json ScriptCurve::create_json() const noexcept {
		auto data = Curve::create_json();
		data["script"] = script_;
		return data;
	}

	bool ScriptCurve::load_json(const nlohmann::json& data) noexcept {
		try {
			script_ = data.at("script").get<std::string>();
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}
} // namespace cved