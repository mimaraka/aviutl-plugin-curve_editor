#include "curve_graph.hpp"
#include "modifier_discretization.hpp"
#include "string_table.hpp"



namespace curve_editor {
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

	nlohmann::json DiscretizationModifier::create_json() const noexcept {
		auto data = Modifier::create_json();
		data["sampling_resolution"] = sampling_resolution_;
		data["quantization_resolution"] = quantization_resolution_;
		return data;
	}

	bool DiscretizationModifier::load_json(const nlohmann::json& data) noexcept {
		try {
			sampling_resolution_ = data.at("sampling_resolution").get<uint32_t>();
			quantization_resolution_ = data.at("quantization_resolution").get<uint32_t>();
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}
} // namespace curve_editor