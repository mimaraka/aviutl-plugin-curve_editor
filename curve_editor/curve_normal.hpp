#pragma once

#include "curve_graph.hpp"
#include <cereal/types/vector.hpp>
#include "handle_bezier.hpp"
#include "enum.hpp"



namespace cved {
	// カーブ(標準)
	class NormalCurve : public GraphCurve {
		std::vector<std::unique_ptr<GraphCurve>> curve_segments_;

	public:
		// コンストラクタ
		NormalCurve(
			const mkaul::Point<double>& pt_start = mkaul::Point{ 0., 0. },
			const mkaul::Point<double>& pt_end = mkaul::Point{ 1., 1. }
		) noexcept;

		// コピーコンストラクタ
		NormalCurve(const NormalCurve& curve) noexcept;

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

		bool adjust_segment_handle_angle(size_t idx, BezierHandle::Type handle_type, const GraphView& view) noexcept;

		double curve_function(double progress, double start, double end) const noexcept override;
		void clear() noexcept override;
		void reverse(bool fix_pt = false) noexcept override;
		void reverse_segment(size_t idx) noexcept;

		bool load_v1_data(const byte* data, size_t pt_n) noexcept;

		void draw_handle(
			mkaul::graphics::Graphics* p_graphics,
			const View& view,
			float thickness,
			float root_radius,
			float tip_radius,
			float tip_thickness,
			bool cutoff_line,
			const mkaul::ColorF& color = mkaul::ColorF{}
		) const noexcept override;

		bool add_curve(const mkaul::Point<double>& pt, const GraphView& view) noexcept;
		bool delete_curve(const mkaul::Point<double>& pt, const GraphView& view) noexcept;
		bool replace_curve(size_t idx, CurveSegmentType segment_type) noexcept;

		bool is_pt_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept override;
		bool is_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept override;

		bool handle_check_hover(
			const mkaul::Point<double>& pt,
			const GraphView& view
		) noexcept override;

		bool handle_update(
			const mkaul::Point<double>& pt,
			const GraphView& view
		) noexcept override;
		void handle_end_control() noexcept override;

		// カーソルがポイントにホバーしているかを判定し、ホバーしていれば移動を開始
		ActivePoint pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept override;
		// ポイントの移動を開始
		bool pt_begin_move(ActivePoint) noexcept override { return false; }
		// ポイントの位置をアップデート
		ActivePoint pt_update(const mkaul::Point<double>& pt, const GraphView& view) noexcept override;
		// ポイントを強制的に動かす
		bool pt_move(ActivePoint, const mkaul::Point<double>&) noexcept override { return true; }
		// ポイントの移動を終了
		void pt_end_move() noexcept override;
		void pt_end_control() noexcept override;

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