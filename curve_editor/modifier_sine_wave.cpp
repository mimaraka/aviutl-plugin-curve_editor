#include "curve_graph.hpp"
#include "modifier_sine_wave.hpp"
#include "string_table.hpp"
#include <numbers>



namespace cved {
	SineWaveModifier::SineWaveModifier(
		double amplitude,
		double frequency,
		double phase
	) noexcept :
		Modifier{ global::string_table[global::StringTable::StringId::LabelModifierSineWave] },
		amplitude_{ amplitude },
		frequency_{ frequency },
		phase_{ phase }
	{}

	CurveFunction SineWaveModifier::apply(const CurveFunction& function) const noexcept {
		return [this, function](double progress, double start, double end) {
			const double prog_rel = (progress - p_curve_->anchor_start().x) / (p_curve_->anchor_end().x - p_curve_->anchor_start().x);
			const auto sine = amplitude_ * std::sin(2 * std::numbers::pi * frequency_ * (prog_rel - phase_));
			return function(progress, start, end) + sine * (end - start);
		};
	}
} // namespace cved