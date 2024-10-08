#include "modifier_square_wave.hpp"
#include "string_table.hpp"
#include "curve_graph.hpp"



namespace cved {
	SquareWaveModifier::SquareWaveModifier(
		double amplitude,
		double frequency,
		double phase
	) noexcept :
		Modifier{ global::string_table[global::StringTable::StringId::LabelModifierSquareWave] },
		amplitude_{ amplitude },
		frequency_{ frequency },
		phase_{ phase }
	{}

	CurveFunction SquareWaveModifier::apply(const CurveFunction& function) const noexcept {
		return [this, function](double progress, double start, double end) {
			const double prog_rel = (progress - p_curve_->anchor_start().x) / (p_curve_->anchor_end().x - p_curve_->anchor_start().x);
			const auto square = std::copysign(amplitude_, std::sin(2 * std::numbers::pi * frequency_ * (prog_rel - phase_)));
			return function(progress, start, end) + square * (end - start);
		};
	}
} // namespace cved