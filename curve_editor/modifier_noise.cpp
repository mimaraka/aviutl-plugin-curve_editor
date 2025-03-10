#include "curve_graph.hpp"
#include "modifier_noise.hpp"
#include "string_table.hpp"



namespace curve_editor {
	NoiseModifier::NoiseModifier(
		int32_t seed,
		double amplitude,
		double frequency,
		double phase,
		int32_t octaves,
		double decay_hardness
	) noexcept
		: Modifier{ global::string_table[global::StringTable::StringId::ModifierTypeNoise] },
		seed_{ seed },
		amplitude_{ amplitude },
		frequency_{ frequency },
		phase_{ phase },
		octaves_{ octaves },
		decay_sharpness_{ decay_hardness },
		noise_{}
	{
		noise_.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		noise_.SetFractalType(FastNoiseLite::FractalType_FBm);
		update();
	}

	void NoiseModifier::update() noexcept {
		noise_.SetSeed(seed_);
		noise_.SetFrequency((float)frequency_);
		noise_.SetFractalOctaves(octaves_);
	}

	CurveFunction NoiseModifier::apply(const CurveFunction& function) const noexcept {
		return [this, function](double progress, double start, double end) {
			const double prog_rel = (progress - p_curve_->anchor_start().x) / (p_curve_->anchor_end().x - p_curve_->anchor_start().x);
			const auto noise = amplitude_ * noise_.GetNoise(prog_rel - phase_, 0.);
			const auto coef = (1 - std::exp(-decay_sharpness_ * prog_rel)) * (1 - std::exp(-decay_sharpness_ * (1 - prog_rel)));
			return function(progress, start, end) + noise * coef * (end - start);
		};
	}

	nlohmann::json NoiseModifier::create_json() const noexcept {
		auto data = Modifier::create_json();
		data["seed"] = seed_;
		data["amplitude"] = amplitude_;
		data["frequency"] = frequency_;
		data["phase"] = phase_;
		data["octaves"] = octaves_;
		data["decay_sharpness"] = decay_sharpness_;
		return data;
	}

	bool NoiseModifier::load_json(const nlohmann::json& data) noexcept {
		try {
			seed_ = data.at("seed").get<int32_t>();
			amplitude_ = data.at("amplitude").get<double>();
			frequency_ = data.at("frequency").get<double>();
			phase_ = data.at("phase").get<double>();
			octaves_ = data.at("octaves").get<int32_t>();
			decay_sharpness_ = data.at("decay_sharpness").get<double>();
			update();
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}
} // namespace curve_editor