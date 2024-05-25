#pragma once

#include "modifier.hpp"
#include "control_point.hpp"



namespace cved {
	// 離散化モディファイア—
	class Discretizer : public Modifier {
		uint32_t sampling_resolution_;
		uint32_t quantization_resolution_;
		const ControlPoint* p_pt_start_ = nullptr;
		const ControlPoint* p_pt_end_ = nullptr;

	public:
		Discretizer(
			const ControlPoint* p_pt_start,
			const ControlPoint* p_pt_end,
			uint32_t sampling_resolution = 10u,
			uint32_t quantization_resolution = 10u
		) noexcept;

		Discretizer(const Discretizer& discretizer) noexcept :
			Modifier{ discretizer.name() },
			sampling_resolution_{ discretizer.sampling_resolution_ },
			quantization_resolution_{ discretizer.quantization_resolution_ },
			p_pt_start_{ discretizer.p_pt_start_ },
			p_pt_end_{ discretizer.p_pt_end_ }
		{}

		auto sampling_resolution() const noexcept { return sampling_resolution_; }
		void set_sampling_resolution(uint32_t sampling_resolution) noexcept { sampling_resolution_ = sampling_resolution; }

		auto quantization_resolution() const noexcept { return quantization_resolution_; }
		void set_quantization_resolution(uint32_t quantization_resolution) noexcept { quantization_resolution_ = quantization_resolution; }

		CurveFunction convert(const CurveFunction& function) const noexcept override;
	};
}