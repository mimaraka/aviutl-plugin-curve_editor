#include "config.hpp"
#include "curve_bezier.hpp"
#include "curve_bounce.hpp"
#include "curve_elastic.hpp"
#include "curve_linear.hpp"
#include "curve_normal.hpp"



namespace cved {
	NormalCurve::NormalCurve(
		const mkaul::Point<double>& anchor_start,
		const mkaul::Point<double>& anchor_end
	) noexcept :
		GraphCurve{ anchor_start, anchor_end, true, nullptr, nullptr },
		curve_segments_{}
	{
		clear();
	}

	// コピーコンストラクタ
	NormalCurve::NormalCurve(const NormalCurve& curve) noexcept :
		GraphCurve{ curve },
		curve_segments_{}
	{
		// TODO: 分岐処理を消したい
		for (const auto& p_curve : curve.curve_segments_) {
			std::unique_ptr<GraphCurve> new_curve;
			if (typeid(*p_curve.get()) == typeid(BezierCurve)) {
				new_curve = std::make_unique<BezierCurve>(*dynamic_cast<BezierCurve*>(p_curve.get()));
			}
			else if (typeid(*p_curve.get()) == typeid(LinearCurve)) {
				new_curve = std::make_unique<LinearCurve>(*dynamic_cast<LinearCurve*>(p_curve.get()));
			}
			else if (typeid(*p_curve.get()) == typeid(ElasticCurve)) {
				new_curve = std::make_unique<ElasticCurve>(*dynamic_cast<ElasticCurve*>(p_curve.get()));
			}
			else if (typeid(*p_curve.get()) == typeid(BounceCurve)) {
				new_curve = std::make_unique<BounceCurve>(*dynamic_cast<BounceCurve*>(p_curve.get()));
			}
			else continue;
			if (!curve_segments_.empty()) {
				new_curve->set_prev(curve_segments_.back().get());
				curve_segments_.back()->set_next(new_curve.get());
			}
			curve_segments_.emplace_back(std::move(new_curve));
		}
	}

	//bool NormalCurve::adjust_segment_handle_angle(size_t idx, BezierHandle::Type handle_type, const GraphView& view) noexcept {
	//	if (check_segment_type<BezierCurve>(idx)) {
	//		auto curve_bezier = dynamic_cast<BezierCurve*>(curve_segments_[idx].get());
	//		//curve_bezier->adjust_handle_angle(handle_type, view);
	//		return true;
	//	}
	//	return false;
	//}

	// カーブの値を取得
	double NormalCurve::curve_function(double progress, double start, double end) const noexcept {
		for (auto it = curve_segments_.begin(), ed = curve_segments_.end(); it != ed; it++) {
			if (*it == curve_segments_.front() and progress < (*it)->anchor_start().x) {
				return start;
			}
			else if (*it == curve_segments_.back() and (*it)->anchor_end().x < progress) {
				return end;
			}
			else {
				if (*it != curve_segments_.back() and mkaul::in_range(progress, (*it)->anchor_end().x, (*std::next(it))->anchor_start().x)) {
					// 線形補間
					double tmp = (progress - (*it)->anchor_end().x) / ((*std::next(it))->anchor_start().x - (*it)->anchor_end().x);
					return ((*std::next(it))->anchor_start().y - (*it)->anchor_end().y) * tmp + (*it)->anchor_end().y;
				}
				else if (mkaul::in_range(progress, (*it)->anchor_start().x, (*it)->anchor_end().x, true)) {
					return (*it)->get_value(progress, start, end);
				}
			}
		}
		return start;
	}

	void NormalCurve::clear() noexcept {
		curve_segments_.clear();
		std::unique_ptr<GraphCurve> new_curve{new BezierCurve{}};
		new_curve->clear();
		curve_segments_.emplace_back(std::move(new_curve));
	}

	bool NormalCurve::is_default() const noexcept {
		bool is_size_1 = curve_segments_.size() == 1;
		bool is_segment_bezier = typeid(*curve_segments_.front()) == typeid(BezierCurve);
		bool is_segment_default = curve_segments_.front()->is_default();

		return is_size_1 and is_segment_bezier and is_segment_default;
	}

	// カーブを反転
	void NormalCurve::reverse(bool) noexcept {
		std::reverse(curve_segments_.begin(), curve_segments_.end());

		for (auto& p_curve : curve_segments_) {
			p_curve->reverse();
			// prev, nextも反転
			auto tmp = p_curve->prev();
			p_curve->set_prev(p_curve->next());
			p_curve->set_next(tmp);
		}
	}

	void NormalCurve::reverse_segment(size_t idx) noexcept {
		if (idx < curve_segments_.size()) {
			curve_segments_[idx]->reverse(true);
		}
	}

	bool NormalCurve::load_v1_data(const byte* data, size_t pt_n) noexcept {
		constexpr int GRAPH_RESOLUTION = 10000;
		
		struct CurvePoint {
			struct {
				int32_t x, y;
			} pt_center, pt_left, pt_right;
			enum class Type : int32_t {
				DefaultLeft,
				DefaultRight,
				Extended
			} type;
		};

		auto pts = reinterpret_cast<const CurvePoint*>(data);
		std::vector<std::unique_ptr<GraphCurve>> vec_tmp;

		for (size_t i = 0u; i < pt_n; i++) {
			// Typeとポイントの座標の整合性チェック
			// 最初の要素
			if (i == 0u) {
				if (
					pts[i].type != CurvePoint::Type::DefaultLeft
					or pts[i].pt_center.x != 0
					or pts[i].pt_center.y != 0
					) {
					return false;
				}
			}
			// 最後の要素
			else if (i == pt_n - 1u) {
				if (
					pts[i].type != CurvePoint::Type::DefaultRight
					or pts[i].pt_center.x != GRAPH_RESOLUTION
					or pts[i].pt_center.y != GRAPH_RESOLUTION
					) {
					return false;
				}
			}
			// 最初と最後の要素以外
			else {
				if (
					pts[i].type != CurvePoint::Type::Extended
					or !mkaul::in_range(pts[i].pt_center.x, 0, GRAPH_RESOLUTION)
					) {
					return false;
				}
			}
			// 
			// 最後の要素以外
			if (i < pt_n - 1u) {
				// カーブの整合性チェック
				if (
					pts[i + 1u].pt_center.x < pts[i].pt_center.x
					or !mkaul::in_range(pts[i].pt_right.x, pts[i].pt_center.x, pts[i + 1u].pt_center.x, true)
					or !mkaul::in_range(pts[i + 1u].pt_left.x, pts[i].pt_center.x, pts[i + 1u].pt_center.x, true)
					) {
					return false;
				}

				std::unique_ptr<GraphCurve> new_curve = std::make_unique<BezierCurve>();
				// ポイントの移動
				new_curve->set_anchor_start(
					pts[i].pt_center.x / (double)GRAPH_RESOLUTION,
					pts[i].pt_center.y / (double)GRAPH_RESOLUTION
				);
				new_curve->set_anchor_end(
					pts[i + 1u].pt_center.x / (double)GRAPH_RESOLUTION,
					pts[i + 1u].pt_center.y / (double)GRAPH_RESOLUTION
				);

				// ハンドルの移動
				auto p_curve_bezier = dynamic_cast<BezierCurve*>(new_curve.get());
				p_curve_bezier->set_handle_left(
					(pts[i].pt_right.x - pts[i].pt_center.x) / (double)GRAPH_RESOLUTION,
					(pts[i].pt_right.y - pts[i].pt_center.y) / (double)GRAPH_RESOLUTION
				);
				p_curve_bezier->set_handle_right(
					(pts[i + 1u].pt_left.x - pts[i + 1u].pt_center.x) / (double)GRAPH_RESOLUTION,
					(pts[i + 1u].pt_left.y - pts[i + 1u].pt_center.y) / (double)GRAPH_RESOLUTION
				);

				if (!vec_tmp.empty()) {
					new_curve->set_prev(vec_tmp.back().get());
					vec_tmp.back()->set_next(new_curve.get());
				}
				vec_tmp.emplace_back(std::move(new_curve));
			}
		}
		curve_segments_ = std::move(vec_tmp);
		return true;
	}

	// ポイントを追加
	bool NormalCurve::add_curve(const mkaul::Point<double>& pt, double scale_x) noexcept {
		constexpr double HANDLE_DEFAULT_LENGTH = 50;

		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			// X座標がカーブの始点と終点の範囲内であった場合
			if (mkaul::in_range(pt.x, (*it)->anchor_start().x, (*it)->anchor_end().x)) {
				std::unique_ptr<GraphCurve> new_curve{
					new BezierCurve{
						pt,
						(*it)->anchor_end(),
						false,
						(*it).get(),
						(*it)->next(),
						mkaul::Point{
							((*it)->anchor_end().x - pt.x) * BezierCurve::DEFAULT_HANDLE_XY,
							((*it)->anchor_end().y - pt.y) * BezierCurve::DEFAULT_HANDLE_XY,
						},
						mkaul::Point{
							(pt.x - (*it)->anchor_end().x) * BezierCurve::DEFAULT_HANDLE_XY,
							(pt.y - (*it)->anchor_end().y) * BezierCurve::DEFAULT_HANDLE_XY,
						}
					}
				};

				// 元々あった要素のthis->prevやthis->nextを更新
				if ((*it)->next()) {
					(*it)->next()->set_prev(new_curve.get());
				}
				(*it)->set_next(new_curve.get());
				// 既存のカーブを移動させる
				(*it)->set_anchor_end(pt);

				// 左のカーブがベジェの場合、右のハンドルをnew_curveの右ハンドルにコピーする
				// TODO: このあたりの処理がゴリ押しだから何とかしたい
				if (typeid(**it) == typeid(BezierCurve)) {
					auto bezier_prev = dynamic_cast<BezierCurve*>((*it).get());
					auto bezier_new = dynamic_cast<BezierCurve*>(new_curve.get());
					bezier_new->set_handle_right(
						bezier_prev->get_handle_right_x() - pt.x + new_curve->anchor_end().x,
						bezier_prev->get_handle_right_y() - pt.y + new_curve->anchor_end().y,
						true
					);
					bezier_new->set_handle_left(
						pt.x + HANDLE_DEFAULT_LENGTH / scale_x,
						pt.y,
						true
					);
					bezier_prev->set_handle_right(
						pt.x - HANDLE_DEFAULT_LENGTH / scale_x,
						pt.y,
						true
					);
					bezier_prev->set_handle_left(
						bezier_prev->get_handle_left_x(),
						bezier_prev->get_handle_left_y(),
						true
					);
				}

				curve_segments_.insert(std::next(it), std::move(new_curve));
				return true;
			}
		}
		return false;
	}

	// ポイントを削除
	bool NormalCurve::delete_curve(GraphCurve* p_segment) noexcept {
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			// 先頭のカーブでない場合
			if ((*it).get() == p_segment and *it != curve_segments_.front()) {
				// 前のカーブの終点を削除するカーブの終点に移動させる
				(*std::prev(it))->set_anchor_end((*it)->anchor_end());
				// prev, nextの更新
				(*std::prev(it))->set_next((*it)->next());
				if ((*it)->next()) {
					(*it)->next()->set_prev((*std::prev(it)).get());
				}
				// 自身と前のカーブがともにベジェの場合
				if (typeid(**it) == typeid(BezierCurve) and typeid(**std::prev(it)) == typeid(BezierCurve)) {
					// 前のベジェの右ハンドルを自身の右ハンドルにする
					auto bezier_prev = dynamic_cast<BezierCurve*>((*std::prev(it)).get());
					auto bezier_self = dynamic_cast<BezierCurve*>((*it).get());
					bezier_prev->set_handle_right(
						bezier_self->get_handle_right_x(), bezier_self->get_handle_right_y(), true
					);
				}
				// ハンドルの整列が有効で、かつ自身が非ベジェのカーブで、自身の両端がベジェのカーブの場合
				else if (
					global::config.get_align_handle()
					and *it != curve_segments_.back()
					and typeid(**it) != typeid(BezierCurve)
					and typeid(**std::prev(it)) == typeid(BezierCurve)
					and typeid(**std::next(it)) == typeid(BezierCurve)
				) {
					// 前のベジェの右ハンドルを次のベジェの左ハンドルに合わせて回転
					auto bezier_prev = dynamic_cast<BezierCurve*>((*std::prev(it)).get());
					auto bezier_next = dynamic_cast<BezierCurve*>((*std::next(it)).get());
					/*bezier_prev->set_handle_right(
						(*it)->anchor_end().pt() - bezier_next->handle_left()->pt_offset(),
						view, true
					);*/
				}
				curve_segments_.erase(it);
				return true;
			}
		}
		return false;
	}

	bool NormalCurve::replace_curve(size_t idx, CurveSegmentType segment_type) noexcept {
		if (idx >= curve_segments_.size()) return false;
		auto current_curve = curve_segments_[idx].get();
		std::unique_ptr<GraphCurve> new_curve;
		switch (segment_type) {
		case CurveSegmentType::Linear:
			new_curve = std::make_unique<LinearCurve>(
				current_curve->anchor_start(),
				current_curve->anchor_end(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			break;

		case CurveSegmentType::Bezier:
			new_curve = std::make_unique<BezierCurve>(
				current_curve->anchor_start(),
				current_curve->anchor_end(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			new_curve->clear();
			break;

		case CurveSegmentType::Elastic:
			new_curve = std::make_unique<ElasticCurve>(
				current_curve->anchor_start(),
				current_curve->anchor_end(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			break;

		case CurveSegmentType::Bounce:
			new_curve = std::make_unique<BounceCurve>(
				current_curve->anchor_start(),
				current_curve->anchor_end(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			break;

		default:
			return false;
		}
		// 両端のカーブのprev,nextの更新
		if (current_curve->prev()) {
			current_curve->prev()->set_next(new_curve.get());
		}
		if (current_curve->next()) {
			current_curve->next()->set_prev(new_curve.get());
		}
		curve_segments_[idx] = std::move(new_curve);

		return true;
	}
}