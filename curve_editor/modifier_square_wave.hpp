#pragma once

#include "constants.hpp"
#include "modifier_base.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>



namespace curve_editor {
	class SquareWaveModifier : public Modifier {
		double amplitude_;
		double frequency_;
		double phase_;

	public:
		SquareWaveModifier(
			double amplitude = 0.2,
			double frequency = 10.,
			double phase = 0.
		) noexcept;

		SquareWaveModifier(const SquareWaveModifier& squarewave) noexcept :
			Modifier{ squarewave },
			amplitude_{ squarewave.amplitude_ },
			frequency_{ squarewave.frequency_ },
			phase_{ squarewave.phase_ }
		{}

		std::unique_ptr<Modifier> clone() const noexcept override {
			return std::make_unique<SquareWaveModifier>(*this);
		}

		constexpr const std::string_view& get_type() const noexcept override { return global::MODIFIER_NAME_SQUARE_WAVE; }

		auto amplitude() const noexcept { return amplitude_; }
		void set_amplitude(double amplitude) noexcept { amplitude_ = amplitude; }

		auto frequency() const noexcept { return frequency_; }
		void set_frequency(double frequency) noexcept { frequency_ = frequency; }

		auto phase() const noexcept { return phase_; }
		void set_phase(double phase) noexcept { phase_ = phase; }

		CurveFunction apply(const CurveFunction& function) const noexcept override;

		nlohmann::json create_json() const noexcept override;
		bool load_json(const nlohmann::json& data) noexcept override;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				cereal::base_class<Modifier>(this),
				amplitude_,
				frequency_,
				phase_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				cereal::base_class<Modifier>(this),
				amplitude_,
				frequency_,
				phase_
			);
		}
	};
} // namespace curve_editor

CEREAL_REGISTER_TYPE(curve_editor::SquareWaveModifier)
CEREAL_CLASS_VERSION(curve_editor::SquareWaveModifier, 0)
CEREAL_REGISTER_POLYMORPHIC_RELATION(curve_editor::Modifier, curve_editor::SquareWaveModifier)