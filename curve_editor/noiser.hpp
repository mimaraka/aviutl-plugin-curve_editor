#pragma once

#include "modifier_base.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <FastNoiseLite.h>



namespace cved {
	// ノイズモディファイア—
	class Noiser : public Modifier {
		int32_t seed_;
		double amplitude_;
		double frequency_;
		double phase_;
		int32_t octaves_;
		double decay_sharpness_;
		FastNoiseLite noise_;

	public:
		Noiser(
			int32_t seed = 0u,
			double amplitude = 0.2,
			double frequency = 20.,
			double phase = 0.,
			int32_t octaves = 1,
			double decay_hardness = 50.
		) noexcept;

		Noiser(const Noiser& noiser) noexcept :
			Modifier{ noiser },
			seed_{ noiser.seed_ },
			amplitude_{ noiser.amplitude_ },
			frequency_{ noiser.frequency_ },
			phase_{ noiser.phase_ },
			octaves_{ noiser.octaves_ },
			decay_sharpness_{ noiser.decay_sharpness_ },
			noise_{ noiser.noise_ }
		{}

		auto seed() const noexcept { return seed_; }
		void set_seed(int32_t seed) noexcept { seed_ = seed; }

		auto amplitude() const noexcept { return amplitude_; }
		void set_amplitude(double amplitude) noexcept { amplitude_ = amplitude; }

		auto frequency() const noexcept { return frequency_; }
		void set_frequency(double frequency) noexcept { frequency_ = frequency; }

		auto phase() const noexcept { return phase_; }
		void set_phase(double phase) noexcept { phase_ = phase; }

		auto octaves() const noexcept { return octaves_; }
		void set_octaves(int32_t octaves) noexcept { octaves_ = octaves; }

		auto decay_sharpness() const noexcept { return decay_sharpness_; }
		void set_decay_sharpness(double decay_sharpness) noexcept { decay_sharpness_ = decay_sharpness; }

		void update() noexcept;

		CurveFunction convert(const CurveFunction& function) const noexcept override;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				cereal::base_class<Modifier>(this),
				seed_,
				amplitude_,
				frequency_,
				phase_,
				octaves_,
				decay_sharpness_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				cereal::base_class<Modifier>(this),
				seed_,
				amplitude_,
				frequency_,
				phase_,
				octaves_,
				decay_sharpness_
			);
			update();
		}
	};
}

CEREAL_CLASS_VERSION(cved::Noiser, 0)
CEREAL_REGISTER_TYPE(cved::Noiser)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::Modifier, cved::Noiser)