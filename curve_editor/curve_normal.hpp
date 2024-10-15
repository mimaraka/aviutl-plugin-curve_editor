#pragma once

#include "constants.hpp"
#include "curve_graph.hpp"
#include "enum.hpp"
#include <cereal/types/vector.hpp>



namespace cved {
	// カーブ(標準)
	class NormalCurve : public GraphCurve {
		std::vector<std::unique_ptr<GraphCurve>> curve_segments_;

	public:
		// コンストラクタ
		NormalCurve(
			const mkaul::Point<double>& anchor_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& anchor_end = mkaul::Point{ 1., 1. }
		) noexcept;

		// コピーコンストラクタ
		NormalCurve(const NormalCurve& curve) noexcept;

		constexpr std::string get_type() const noexcept override { return global::CURVE_NAME_NORMAL; }

		// セグメント数を取得
		auto segment_n() const noexcept { return curve_segments_.size(); }
		// セグメントを取得
		const GraphCurve* get_segment(size_t idx) const noexcept {
			if (idx < curve_segments_.size()) {
				return curve_segments_[idx].get();
			}
			return nullptr;
		}

		// セグメントの型をチェック
		template <class CurveType>
		bool check_segment_type(size_t idx) const noexcept {
			auto segment = get_segment(idx);
			return segment != nullptr and typeid(*segment) == typeid(CurveType);
		}

		//bool adjust_segment_handle_angle(size_t idx, BezierHandle::Type handle_type, const GraphView& view) noexcept;

		double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		bool is_default() const noexcept override;
		void reverse(bool fix_pt = false) noexcept override;
		void reverse_segment(uint32_t segment_id) noexcept;

		bool load_v1_data(const byte* data, size_t pt_n) noexcept;

		bool add_curve(const mkaul::Point<double>& pt, double scale_x) noexcept;
		bool delete_curve(uint32_t segment_id) noexcept;
		bool replace_curve(uint32_t segment_id, CurveSegmentType segment_type) noexcept;

		nlohmann::json create_json() const noexcept override;
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
}

CEREAL_CLASS_VERSION(cved::NormalCurve, 0)
CEREAL_REGISTER_TYPE(cved::NormalCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::GraphCurve, cved::NormalCurve)