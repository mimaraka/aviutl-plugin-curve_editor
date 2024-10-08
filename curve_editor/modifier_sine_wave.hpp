#pragma once

#include "modifier_base.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>



namespace cved {
	class SineWaveModifier : public Modifier {
		double amplitude_;
		double frequency_;
		double phase_;

	public:
		SineWaveModifier(
			double amplitude = 0.2,
			double frequency = 10.,
			double phase = 0.
		) noexcept;

		SineWaveModifier(const SineWaveModifier& sinewave) noexcept :
			Modifier{ sinewave },
			amplitude_{ sinewave.amplitude_ },
			frequency_{ sinewave.frequency_ },
			phase_{ sinewave.phase_ }
		{}

		auto amplitude() const noexcept { return amplitude_; }
		void set_amplitude(double amplitude) noexcept { amplitude_ = amplitude; }

		auto frequency() const noexcept { return frequency_; }
		void set_frequency(double frequency) noexcept { frequency_ = frequency; }

		auto phase() const noexcept { return phase_; }
		void set_phase(double phase) noexcept { phase_ = phase; }

		CurveFunction apply(const CurveFunction& function) const noexcept override;

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
} // namespace cved

CEREAL_CLASS_VERSION(cved::SineWaveModifier, 0)
CEREAL_REGISTER_TYPE(cved::SineWaveModifier)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::Modifier, cved::SineWaveModifier)