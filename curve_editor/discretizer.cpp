#include "discretizer.hpp"
#include "string_table.hpp"



namespace cved {
	Discretizer::Discretizer(
		const ControlPoint* p_pt_start,
		const ControlPoint* p_pt_end,
		uint32_t sampling_resolution,
		uint32_t quantization_resolution
	) noexcept
		: Modifier{ global::string_table[global::StringTable::StringId::LabelModifierDiscretizer] },
		sampling_resolution_{ sampling_resolution },
		quantization_resolution_{ quantization_resolution },
		p_pt_start_{ p_pt_start },
		p_pt_end_{ p_pt_end }
	{}

	CurveFunction Discretizer::apply(const CurveFunction& function) const noexcept {
		return [this, function](double progress, double start, double end) {
			// 離散化の設定
			double prog = progress;
			// 標本化
			if (0u < sampling_resolution_) {
				double sampling_interval = (p_pt_end_->x() - p_pt_start_->x()) / (double)sampling_resolution_;
				prog = std::floor(prog / sampling_interval) * sampling_interval;
			}
			double ret = function(prog, start, end);
			// 量子化
			if (0u < quantization_resolution_) {
				double tmp1 = (ret - start) / (end - start);
				double tmp2 = (tmp1 - p_pt_start_->y()) / (p_pt_end_->y() - p_pt_start_->y());
				double tmp3 = std::floor(tmp2 * (double)quantization_resolution_) / (double)quantization_resolution_;
				double tmp4 = p_pt_start_->y() + tmp3 * (p_pt_end_->y() - p_pt_start_->y());
				ret = start + tmp4 * (end - start);
			}
			return ret;
		};
	}
}