#include "discretizer.hpp"
#include "string_table.hpp"
#include "curve_graph.hpp"



namespace cved {
	Discretizer::Discretizer(
		uint32_t sampling_resolution,
		uint32_t quantization_resolution
	) noexcept
		: Modifier{ global::string_table[global::StringTable::StringId::LabelModifierDiscretizer] },
		sampling_resolution_{ sampling_resolution },
		quantization_resolution_{ quantization_resolution }
	{}

	CurveFunction Discretizer::convert(const CurveFunction& function) const noexcept {
		return [this, function](double progress, double start, double end) {
			// 離散化の設定
			double prog = progress;
			// 標本化
			if (0u < sampling_resolution_) {
				double sampling_interval = (p_curve_->pt_end().x() - p_curve_->pt_start().x()) / (double)sampling_resolution_;
				prog = std::floor(prog / sampling_interval) * sampling_interval;
			}
			double ret = function(prog, start, end);
			// 量子化
			if (0u < quantization_resolution_) {
				double tmp1 = (ret - start) / (end - start);
				double tmp2 = (tmp1 - p_curve_->pt_start().y()) / (p_curve_->pt_end().y() - p_curve_->pt_start().y());
				double tmp3 = std::floor(tmp2 * (double)quantization_resolution_) / (double)quantization_resolution_;
				double tmp4 = p_curve_->pt_start().y() + tmp3 * (p_curve_->pt_end().y() - p_curve_->pt_start().y());
				ret = start + tmp4 * (end - start);
			}
			return ret;
		};
	}
}