#include "curve_graph.hpp"
#include "modifier_square_wave.hpp"
#include "string_table.hpp"



namespace curve_editor {
	SquareWaveModifier::SquareWaveModifier(
		double amplitude,
		double frequency,
		double phase
	) noexcept :
		Modifier{ global::string_table[global::StringTable::StringId::ModifierTypeSquareWave] },
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

	nlohmann::json SquareWaveModifier::create_json() const noexcept {
		auto data = Modifier::create_json();
		data["amplitude"] = amplitude_;
		data["frequency"] = frequency_;
		data["phase"] = phase_;
		return data;
	}

	bool SquareWaveModifier::load_json(const nlohmann::json& data) noexcept {
		try {
			amplitude_ = data.at("amplitude").get<double>();
			frequency_ = data.at("frequency").get<double>();
			phase_ = data.at("phase").get<double>();
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}
} // namespace curve_editor