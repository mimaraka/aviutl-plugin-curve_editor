#include "curve_graph.hpp"
#include "modifier_discretization.hpp"
#include "string_table.hpp"



namespace cved {
	DiscretizationModifier::DiscretizationModifier(
		uint32_t sampling_resolution,
		uint32_t quantization_resolution
	) noexcept
		: Modifier{ global::string_table[global::StringTable::StringId::LabelModifierDiscretization] },
		sampling_resolution_{ sampling_resolution },
		quantization_resolution_{ quantization_resolution }
	{}

	CurveFunction DiscretizationModifier::apply(const CurveFunction& function) const noexcept {
		return [this, function](double progress, double start, double end) {
			// 離散化の設定
			double prog = progress;
			// 標本化
			if (0u < sampling_resolution_) {
				double sampling_interval = (p_curve_->anchor_end().x - p_curve_->anchor_start().x) / (double)sampling_resolution_;
				prog = std::floor(prog / sampling_interval) * sampling_interval;
			}
			double ret = function(prog, start, end);
			// 量子化
			if (0u < quantization_resolution_) {
				double tmp1 = (ret - start) / (end - start);
				double tmp2 = (tmp1 - p_curve_->anchor_start().y) / (p_curve_->anchor_end().y - p_curve_->anchor_start().y);
				double tmp3 = std::floor(tmp2 * (double)quantization_resolution_) / (double)quantization_resolution_;
				double tmp4 = p_curve_->anchor_start().y + tmp3 * (p_curve_->anchor_end().y - p_curve_->anchor_start().y);
				ret = start + tmp4 * (end - start);
			}
			return ret;
		};
	}
}