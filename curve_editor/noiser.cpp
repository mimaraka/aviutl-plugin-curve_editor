#include "noiser.hpp"
#include <PerlinNoise.hpp>
#include "string_table.hpp"



namespace cved {
	Noiser::Noiser(
		uint32_t seed,
		double amplitude,
		double frequency,
		double phase,
		int32_t octaves,
		double decay_hardness
	) noexcept
		: Modifier{ global::string_table[global::StringTable::StringId::LabelModifierNoiser] },
		seed_{ seed },
		amplitude_{ amplitude },
		frequency_{ frequency },
		phase_{ phase },
		octaves_{ octaves },
		decay_sharpness_{ decay_hardness }
	{}

	CurveFunction Noiser::convert(const CurveFunction& function) const noexcept {
		return [this, function](double progress, double start, double end) {
			siv::PerlinNoise perlin{ seed_ };
			const auto noise = amplitude_ * perlin.octave1D(progress * frequency_ + phase_, octaves_);
			const auto coef = (1 - std::exp(-decay_sharpness_ * progress)) * (1 - std::exp(-decay_sharpness_ * (1 - progress)));
			return function(progress, start, end) + noise * coef * (end - start);
		};
	}
}