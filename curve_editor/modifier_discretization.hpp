#pragma once

#include "constants.hpp"
#include "modifier_base.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>



namespace curve_editor {
	// 離散化モディファイア—
	class DiscretizationModifier : public Modifier {
		uint32_t sampling_resolution_;
		uint32_t quantization_resolution_;

	public:
		DiscretizationModifier(
			uint32_t sampling_resolution = 10u,
			uint32_t quantization_resolution = 10u
		) noexcept;

		DiscretizationModifier(const DiscretizationModifier& discretizer) noexcept :
			Modifier{ discretizer },
			sampling_resolution_{ discretizer.sampling_resolution_ },
			quantization_resolution_{ discretizer.quantization_resolution_ }
		{}

		std::unique_ptr<Modifier> clone() const noexcept override {
			return std::make_unique<DiscretizationModifier>(*this);
		}

		constexpr const std::string_view& get_type() const noexcept override { return global::MODIFIER_NAME_DISCRETIZATION; }

		auto sampling_resolution() const noexcept { return sampling_resolution_; }
		void set_sampling_resolution(uint32_t sampling_resolution) noexcept { sampling_resolution_ = sampling_resolution; }

		auto quantization_resolution() const noexcept { return quantization_resolution_; }
		void set_quantization_resolution(uint32_t quantization_resolution) noexcept { quantization_resolution_ = quantization_resolution; }

		CurveFunction apply(const CurveFunction& function) const noexcept override;

		nlohmann::json create_json() const noexcept override;
		bool load_json(const nlohmann::json& data) noexcept override;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				cereal::base_class<Modifier>(this),
				sampling_resolution_,
				quantization_resolution_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				cereal::base_class<Modifier>(this),
				sampling_resolution_,
				quantization_resolution_
			);
		}
	};
} // namespace curve_editor

CEREAL_CLASS_VERSION(curve_editor::DiscretizationModifier, 0)
CEREAL_REGISTER_TYPE(curve_editor::DiscretizationModifier)
CEREAL_REGISTER_POLYMORPHIC_RELATION(curve_editor::Modifier, curve_editor::DiscretizationModifier)