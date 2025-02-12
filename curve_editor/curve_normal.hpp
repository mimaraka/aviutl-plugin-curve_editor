#pragma once

#include "constants.hpp"
#include "curve_bezier.hpp"
#include "curve_graph.hpp"
#include "enum.hpp"
#include "string_table.hpp"
#include <cereal/types/vector.hpp>



namespace curve_editor {
	// カーブ(標準)
	class NormalCurve : public GraphCurve {
		std::vector<std::unique_ptr<GraphCurve>> curve_segments_;

		void clone_segments(const NormalCurve& curve) noexcept;

	public:
		// コンストラクタ
		NormalCurve(
			const mkaul::Point<double>& anchor_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& anchor_end = mkaul::Point{ 1., 1. }
		) noexcept;

		// コピーコンストラクタ
		NormalCurve(const NormalCurve& curve) noexcept;

		// コピー代入演算子
		NormalCurve& operator=(const NormalCurve& curve) noexcept;

		[[nodiscard]] std::unique_ptr<GraphCurve> clone_graph() const noexcept override { return std::make_unique<NormalCurve>(*this); }
		[[nodiscard]] std::unique_ptr<Curve> clone() const noexcept override { return clone_graph(); }

		[[nodiscard]] constexpr std::string get_name() const noexcept override { return global::CURVE_NAME_NORMAL; }
		[[nodiscard]] std::string get_disp_name() const noexcept override { return global::string_table[global::StringTable::StringId::LabelEditModeNormal]; }

		void set_locked(bool locked) noexcept override;

		// セグメント数を取得
		[[nodiscard]] auto segment_n() const noexcept { return curve_segments_.size(); }
		// セグメントを取得
		[[nodiscard]] const GraphCurve* get_segment(size_t idx) const noexcept {
			if (idx < curve_segments_.size()) {
				return curve_segments_[idx].get();
			}
			return nullptr;
		}

		[[nodiscard]] uint32_t get_segment_id(size_t idx) const noexcept {
			if (idx < curve_segments_.size()) {
				return curve_segments_[idx]->get_id();
			}
			return 0;
		}

		bool adjust_segment_handle_angle(uint32_t segment_id, BezierCurve::HandleType handle_type, double scale_x, double scale_y) noexcept;

		[[nodiscard]] double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		[[nodiscard]] bool is_default() const noexcept override;
		void reverse(bool fix_pt = false) noexcept override;
		void reverse_segment(uint32_t segment_id) noexcept;

		bool load_v1_data(const byte* data, size_t pt_n) noexcept;

		bool add_curve(const mkaul::Point<double>& pt, double scale_x) noexcept;
		bool delete_curve(uint32_t segment_id) noexcept;
		bool replace_curve(uint32_t segment_id, CurveSegmentType segment_type) noexcept;

		[[nodiscard]] nlohmann::json create_json() const noexcept override;
		bool load_json(const nlohmann::json& data) noexcept override;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				cereal::base_class<GraphCurve>(this),
				curve_segments_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				cereal::base_class<GraphCurve>(this),
				curve_segments_
			);

			for (auto begin = curve_segments_.begin(), end = curve_segments_.end(), it = begin; it != end; ++it) {
				if (it != begin) {
					(*it)->set_prev((*std::prev(it)).get());
				}
				if (it != std::prev(end)) {
					(*it)->set_next((*std::next(it)).get());
				}
			}
		}
	};
} // namespace curve_editor

CEREAL_CLASS_VERSION(curve_editor::NormalCurve, 0)
CEREAL_REGISTER_TYPE(curve_editor::NormalCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(curve_editor::GraphCurve, curve_editor::NormalCurve)