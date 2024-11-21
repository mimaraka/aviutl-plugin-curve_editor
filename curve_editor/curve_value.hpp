#pragma once

#include "constants.hpp"
#include "curve_graph.hpp"
#include "enum.hpp"
#include "string_table.hpp"



namespace curve_editor {
	// カーブ(数値指定)
	class ValueCurve : public GraphCurve {
		std::vector<std::unique_ptr<GraphCurve>> curve_segments_;

	public:
		ValueCurve(
			const mkaul::Point<double>& anchor_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& anchor_end = mkaul::Point{ 1., 1. }
		) noexcept;

		ValueCurve(const ValueCurve& curve) noexcept;

		[[nodiscard]] constexpr std::string get_name() const noexcept override { return global::CURVE_NAME_VALUE; }
		[[nodiscard]] std::string get_disp_name() const noexcept override { return global::string_table[global::StringTable::StringId::LabelEditModeValue]; }

		[[nodiscard]] double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		[[nodiscard]] bool is_default() const noexcept override;

		bool add_curve(const mkaul::Point<double>& pt, double scale_x) noexcept;
		bool delete_curve(GraphCurve* p_segment) noexcept;
		bool replace_curve(size_t idx, CurveSegmentType segment_type) noexcept;

		[[nodiscard]] nlohmann::json create_json() const noexcept override;
		bool load_json(const nlohmann::json& data) noexcept override;
	};
} // namespace curve_editor