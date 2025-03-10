#include "config.hpp"
#include "curve_bounce.hpp"
#include "curve_elastic.hpp"
#include "curve_linear.hpp"
#include "curve_normal.hpp"



namespace curve_editor {
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
		clone_segments(curve);
	}

	// コピー代入演算子
	NormalCurve& NormalCurve::operator=(const NormalCurve& curve) noexcept {
		if (this != &curve) {
			GraphCurve::operator=(curve);
			curve_segments_.clear();
			clone_segments(curve);
		}
		return *this;
	}

	// 等価演算子
	bool NormalCurve::operator==(const NormalCurve& curve) const noexcept {
		if (curve_segments_.size() != curve.curve_segments_.size()) {
			return false;
		}
		for (size_t i = 0; i < curve_segments_.size(); i++) {
			if (*curve_segments_[i] != *curve.curve_segments_[i]) {
				return false;
			}
		}
		return true;
	}

	// セグメントのクローン
	void NormalCurve::clone_segments(const NormalCurve& curve) noexcept {
		for (const auto& p_curve : curve.curve_segments_) {
			auto new_curve = p_curve->clone_graph();
			if (!curve_segments_.empty()) {
				new_curve->set_prev(curve_segments_.back().get());
				curve_segments_.back()->set_next(new_curve.get());
			}
			curve_segments_.emplace_back(std::move(new_curve));
		}
	}

	void NormalCurve::set_locked(bool locked) noexcept {
		GraphCurve::set_locked(locked);
		for (const auto& p_curve : curve_segments_) {
			p_curve->set_locked(locked);
		}
	}

	bool NormalCurve::adjust_segment_handle_angle(uint32_t segment_id, BezierCurve::HandleType handle_type, double scale_x, double scale_y) noexcept {
		auto curve = global::id_manager.get_curve<BezierCurve>(segment_id);
		const GraphCurve* curve_neighbor = nullptr;
		auto get_new_offset = [scale_x, scale_y](const mkaul::Point<double>& offset, double slope) {
			double angle = std::atan2(offset.y * scale_y, offset.x * scale_x);
			double new_angle = std::atan(slope * scale_y / scale_x);
			double offset_angle = new_angle - angle;
			double new_offset_x = std::cos(offset_angle) * offset.x - std::sin(offset_angle) * offset.y * scale_y / scale_x;
			double new_offset_y = std::sin(offset_angle) * offset.x * scale_x / scale_y + std::cos(offset_angle) * offset.y;
			return mkaul::Point{ new_offset_x, new_offset_y };
		};
		if (curve) {
			switch (handle_type) {
			case BezierCurve::HandleType::Left:
				if ((curve_neighbor = curve->prev())) {
					auto offset = curve->get_handle_left() - curve->anchor_start();
					auto new_offset = get_new_offset(
						offset,
						curve_neighbor->get_velocity(curve_neighbor->anchor_end().x, 0., 1.)
					);
					curve->move_handle_left(curve->anchor_start() + new_offset);
				}
				break;

			case BezierCurve::HandleType::Right:
				if ((curve_neighbor = curve->next())) {
					auto offset = curve->get_handle_right() - curve->anchor_end();
					auto new_offset = get_new_offset(
						offset,
						curve_neighbor->get_velocity(curve_neighbor->anchor_start().x + 0.000001, 0., 1.)
					);
					curve->move_handle_right(curve->anchor_end() - new_offset);
				}
				break;

			default:
				return false;
			}
			return true;
		}
		return false;
	}

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
				if (*it != curve_segments_.back() and mkaul::in_range(progress, (*it)->anchor_end().x, (*std::next(it))->anchor_start().x, false)) {
					// 線形補間
					double tmp = (progress - (*it)->anchor_end().x) / ((*std::next(it))->anchor_start().x - (*it)->anchor_end().x);
					return ((*std::next(it))->anchor_start().y - (*it)->anchor_end().y) * tmp + (*it)->anchor_end().y;
				}
				else if (mkaul::in_range(progress, (*it)->anchor_start().x, (*it)->anchor_end().x)) {
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
		if (is_locked()) return;
		std::reverse(curve_segments_.begin(), curve_segments_.end());

		for (auto& p_curve : curve_segments_) {
			p_curve->reverse();
			// prev, nextも反転
			auto tmp = p_curve->prev();
			p_curve->set_prev(p_curve->next());
			p_curve->set_next(tmp);
		}
	}

	void NormalCurve::reverse_segment(uint32_t id) noexcept {
		for (auto& segment : curve_segments_) {
			if (id == segment->get_id()) {
				segment->reverse(true);
				return;
			}
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

		auto pts = std::bit_cast<const CurvePoint*>(data);
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
					or !mkaul::in_range(pts[i].pt_center.x, 0, GRAPH_RESOLUTION, false)
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
					or !mkaul::in_range(pts[i].pt_right.x, pts[i].pt_center.x, pts[i + 1u].pt_center.x)
					or !mkaul::in_range(pts[i + 1u].pt_left.x, pts[i].pt_center.x, pts[i + 1u].pt_center.x)
					) {
					return false;
				}

				auto new_curve = std::make_unique<BezierCurve>();
				// ポイントの移動
				new_curve->move_anchor_start(
					pts[i].pt_center.x / (double)GRAPH_RESOLUTION,
					pts[i].pt_center.y / (double)GRAPH_RESOLUTION,
					true
				);
				new_curve->move_anchor_end(
					pts[i + 1u].pt_center.x / (double)GRAPH_RESOLUTION,
					pts[i + 1u].pt_center.y / (double)GRAPH_RESOLUTION,
					true
				);

				// ハンドルの移動
				new_curve->move_handle_left(
					mkaul::Point{
						new_curve->anchor_start().x + (pts[i].pt_right.x - pts[i].pt_center.x) / (double)GRAPH_RESOLUTION,
						new_curve->anchor_start().y + (pts[i].pt_right.y - pts[i].pt_center.y) / (double)GRAPH_RESOLUTION
					}
				);
				new_curve->move_handle_right(
					mkaul::Point{
						new_curve->anchor_end().x + (pts[i + 1u].pt_left.x - pts[i + 1u].pt_center.x) / (double)GRAPH_RESOLUTION,
						new_curve->anchor_end().y + (pts[i + 1u].pt_left.y - pts[i + 1u].pt_center.y) / (double)GRAPH_RESOLUTION
					}
				);
				// prev, nextの設定
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
		if (is_locked()) return false;
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			// X座標がカーブの始点と終点の範囲内であった場合
			if (mkaul::in_range(pt.x, (*it)->anchor_start().x, (*it)->anchor_end().x, false)) {
				std::unique_ptr<GraphCurve> new_curve = std::make_unique<BezierCurve>(
					pt,
					(*it)->anchor_end(),
					false,
					(*it).get(),
					(*it)->next(),
					mkaul::Point{
						((*it)->anchor_end().x - pt.x) * BezierHandle::DEFAULT_HANDLE_RATIO,
						((*it)->anchor_end().y - pt.y) * BezierHandle::DEFAULT_HANDLE_RATIO,
					},
					mkaul::Point{
						(pt.x - (*it)->anchor_end().x) * BezierHandle::DEFAULT_HANDLE_RATIO,
						(pt.y - (*it)->anchor_end().y) * BezierHandle::DEFAULT_HANDLE_RATIO,
					}
				);

				// 既存のカーブを移動させる
				(*it)->move_anchor_end(pt, true, true);

				// 左のカーブがベジェの場合、右のハンドルをnew_curveの右ハンドルにコピーする
				auto bezier_prev = dynamic_cast<BezierCurve*>((*it).get());
				auto bezier_new = dynamic_cast<BezierCurve*>(new_curve.get());
				if (bezier_prev and bezier_new) {
					bezier_new->move_handle_right(
						bezier_prev->get_handle_right() - pt + new_curve->anchor_end(),
						true, true
					);
					bezier_new->move_handle_left(
						mkaul::Point{
							pt.x + HANDLE_DEFAULT_LENGTH / scale_x,
							pt.y
						},
						true, true
					);
					bezier_prev->move_handle_right(
						mkaul::Point{
							pt.x - HANDLE_DEFAULT_LENGTH / scale_x,
							pt.y
						},
						true, true
					);
					bezier_prev->move_handle_left(
						bezier_prev->get_handle_left(),
						true, true
					);
				}

				// 元々あった要素のthis->prevやthis->nextを更新
				if ((*it)->next()) {
					(*it)->next()->set_prev(new_curve.get());
				}
				(*it)->set_next(new_curve.get());

				curve_segments_.insert(std::next(it), std::move(new_curve));
				return true;
			}
		}
		return false;
	}

	// ポイントを削除
	bool NormalCurve::delete_curve(uint32_t segment_id) noexcept {
		if (is_locked()) return false;
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			// 先頭のカーブでない場合
			if ((*it).get()->get_id() == segment_id and *it != curve_segments_.front()) {
				// 前のカーブの終点を削除するカーブの終点に移動させる
				(*std::prev(it))->move_anchor_end((*it)->anchor_end(), true, true);
				// prev, nextの更新
				(*std::prev(it))->set_next((*it)->next());
				if ((*it)->next()) {
					(*it)->next()->set_prev((*std::prev(it)).get());
				}
				// 自身と前のカーブがともにベジェの場合
				auto bezier_prev = dynamic_cast<BezierCurve*>((*std::prev(it)).get());
				auto bezier_self = dynamic_cast<BezierCurve*>((*it).get());
				if (bezier_prev and bezier_self) {
					// 前のベジェの右ハンドルを自身の右ハンドルにする
					bezier_prev->move_handle_right(
						bezier_self->get_handle_right(), true, true
					);
				}
				curve_segments_.erase(it);
				return true;
			}
		}
		return false;
	}

	bool NormalCurve::replace_curve(uint32_t id, CurveSegmentType segment_type) noexcept {
		if (is_locked()) return false;
		for (auto& segment : curve_segments_) {
			if (id == segment->get_id()) {
				std::unique_ptr<GraphCurve> new_curve;
				switch (segment_type) {
				case CurveSegmentType::Linear:
					new_curve = std::make_unique<LinearCurve>(
						segment->anchor_start(),
						segment->anchor_end(),
						false,
						segment->prev(),
						segment->next()
					);
					break;

				case CurveSegmentType::Bezier:
					new_curve = std::make_unique<BezierCurve>(
						segment->anchor_start(),
						segment->anchor_end(),
						false,
						segment->prev(),
						segment->next()
					);
					new_curve->clear();
					break;

				case CurveSegmentType::Elastic:
					new_curve = std::make_unique<ElasticCurve>(
						segment->anchor_start(),
						segment->anchor_end(),
						false,
						segment->prev(),
						segment->next()
					);
					break;

				case CurveSegmentType::Bounce:
					new_curve = std::make_unique<BounceCurve>(
						segment->anchor_start(),
						segment->anchor_end(),
						false,
						segment->prev(),
						segment->next()
					);
					break;

				default:
					return false;
				}
				// 両端のカーブのprev,nextの更新
				if (segment->prev()) {
					segment->prev()->set_next(new_curve.get());
				}
				if (segment->next()) {
					segment->next()->set_prev(new_curve.get());
				}
				segment = std::move(new_curve);

				return true;
			}
		}
		return false;
	}

	nlohmann::json NormalCurve::create_json() const noexcept {
		nlohmann::json data = GraphCurve::create_json();
		data["segments"] = nlohmann::json::array();
		for (const auto& p_curve : curve_segments_) {
			data["segments"].emplace_back(p_curve->create_json());
		}
		return data;
	}

	bool NormalCurve::load_json(const nlohmann::json& data) noexcept {
		if (!GraphCurve::load_json(data)) {
			return false;
		}
		try {
			std::vector<std::unique_ptr<GraphCurve>> vec_tmp;
			for (const auto& segment : data.at("segments")) {
				std::unique_ptr<GraphCurve> new_curve;
				if (segment.at("type") == global::CURVE_NAME_BEZIER.data()) {
					new_curve = std::make_unique<BezierCurve>();
				}
				else if (segment.at("type") == global::CURVE_NAME_LINEAR.data()) {
					new_curve = std::make_unique<LinearCurve>();
				}
				else if (segment.at("type") == global::CURVE_NAME_ELASTIC.data()) {
					new_curve = std::make_unique<ElasticCurve>();
				}
				else if (segment.at("type") == global::CURVE_NAME_BOUNCE.data()) {
					new_curve = std::make_unique<BounceCurve>();
				}
				if (!new_curve or !new_curve->load_json(segment)) {
					return false;
				}
				if (!vec_tmp.empty()) {
					if (
						!mkaul::real_equal(new_curve->anchor_start().x, vec_tmp.back()->anchor_end().x)
						or !mkaul::real_equal(new_curve->anchor_start().y, vec_tmp.back()->anchor_end().y)
					) {
						return false;
					}
					new_curve->set_prev(vec_tmp.back().get());
					vec_tmp.back()->set_next(new_curve.get());
				}
				else if (!mkaul::real_equal(new_curve->anchor_start().x, 0.) or !mkaul::real_equal(new_curve->anchor_start().y, 0.)) {
					return false;
				}
				vec_tmp.emplace_back(std::move(new_curve));
			}
			if (!mkaul::real_equal(vec_tmp.back()->anchor_end().x, 1.) or !mkaul::real_equal(vec_tmp.back()->anchor_end().y, 1.)) {
				return false;
			}
			curve_segments_ = std::move(vec_tmp);
		}
		catch (const std::exception&) {
			return false;
		}
		return true;
	}
} // namespace curve_editor