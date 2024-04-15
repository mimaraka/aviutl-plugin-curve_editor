#include "curve_normal.hpp"
#include "config.hpp"
#include "curve_data.hpp"
#include "curve_bezier.hpp"
#include "curve_bounce.hpp"
#include "curve_elastic.hpp"
#include "curve_linear.hpp"



namespace cved {
	NormalCurve::NormalCurve(
		const mkaul::Point<double>& pt_start,
		const mkaul::Point<double>& pt_end
	) :
		GraphCurve{ pt_start, pt_end, true, nullptr, nullptr },
		curve_segments_{}
	{
		clear();
	}

	// カーブの値を取得
	double NormalCurve::curve_function(double progress, double start, double end) const noexcept {
		for (auto it = curve_segments_.begin(), ed = curve_segments_.end(); it != ed; it++) {
			if (*it == curve_segments_.front() and progress < (*it)->pt_start().x()) {
				return start;
			}
			else if (*it == curve_segments_.back() and (*it)->pt_end().x() < progress) {
				return end;
			}
			else {
				if (*it != curve_segments_.back() and mkaul::in_range(progress, (*it)->pt_end().x(), (*std::next(it))->pt_start().x())) {
					// 線形補間
					double tmp = (progress - (*it)->pt_end().x()) / ((*std::next(it))->pt_start().x() - (*it)->pt_end().x());
					return ((*std::next(it))->pt_start().y() - (*it)->pt_end().y()) * tmp + (*it)->pt_end().y();
				}
				else if (mkaul::in_range(progress, (*it)->pt_start().x(), (*it)->pt_end().x(), true)) {
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

	// カーブを反転
	void NormalCurve::reverse() noexcept {
		std::reverse(curve_segments_.begin(), curve_segments_.end());

		for (auto& p_curve : curve_segments_) {
			p_curve->reverse();
			// prev, nextも反転
			auto tmp = p_curve->prev();
			p_curve->set_prev(p_curve->next());
			p_curve->set_next(tmp);
		}
	}

	// カーブのデータを作成
	void NormalCurve::create_data(std::vector<byte>& data) const noexcept {
		data.clear();
		for (const auto& p_curve : curve_segments_) {
			std::vector<byte> tmp;
			p_curve->create_data(tmp);
			std::copy(tmp.begin(), tmp.end(), std::back_inserter(data));
		}
		GraphCurveData data_graph = {
			.start_x = pt_start().x(),
			.start_y = pt_start().y(),
			.end_x = pt_end().x(),
			.end_y = pt_end().y(),
			.sampling_resolution = get_sampling_resolution(),
			.quantization_resolution = get_quantization_resolution()
		};
		auto bytes_graph = reinterpret_cast<byte*>(&data_graph);
		size_t n = sizeof(GraphCurveData) / sizeof(byte);
		data.insert(data.begin(), bytes_graph, bytes_graph + n);
	}

	// カーブのデータを読み込み
	bool NormalCurve::load_data(const byte* data, size_t size) noexcept {
		std::vector<std::unique_ptr<GraphCurve>> vec_tmp;
		auto data_graph = reinterpret_cast<const GraphCurveData*>(data);
		set_sampling_resolution(data_graph->sampling_resolution);
		set_quantization_resolution(data_graph->quantization_resolution);

		for (size_t idx = sizeof(GraphCurveData) / sizeof(byte); idx < size;) {
			std::unique_ptr<GraphCurve> new_curve;

			switch (*(data + idx)) {
			case (byte)CurveSegmentType::Linear:
				idx++;
				if (!load_segment_data<LinearCurve, LinearCurveData>(data, idx, size, new_curve)) return false;
				break;

			case (byte)CurveSegmentType::Bezier:
				idx++;
				if (!load_segment_data<BezierCurve, BezierCurveData>(data, idx, size, new_curve)) return false;
				break;

			case (byte)CurveSegmentType::Elastic:
				idx++;
				if (!load_segment_data<ElasticCurve, ElasticCurveData>(data, idx, size, new_curve)) return false;
				break;

			case (byte)CurveSegmentType::Bounce:
				idx++;
				if (!load_segment_data<BounceCurve, BounceCurveData>(data, idx, size, new_curve)) return false;
				break;

			default:
				return false;
			}
			if (!vec_tmp.empty()) {
				new_curve->set_prev(vec_tmp.back().get());
				vec_tmp.back()->set_next(new_curve.get());
			}
			vec_tmp.emplace_back(std::move(new_curve));
		}
		curve_segments_ = std::move(vec_tmp);
		return true;
	}

	bool NormalCurve::load_data_v1(const byte* data, size_t pt_n) noexcept {
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
					or !mkaul::in_range(pts[i].pt_right.x, pts[i].pt_center.x, pts[i + 1u].pt_center.x)
					or !mkaul::in_range(pts[i + 1u].pt_left.x, pts[i].pt_center.x, pts[i + 1u].pt_center.x)
					) {
					return false;
				}

				std::unique_ptr<GraphCurve> new_curve = std::make_unique<BezierCurve>();
				// ポイントの移動
				new_curve->pt_begin_move(GraphCurve::ActivePoint::Start);
				new_curve->pt_move(
					GraphCurve::ActivePoint::Start,
					mkaul::Point{
						pts[i].pt_center.x / (double)GRAPH_RESOLUTION,
						pts[i].pt_center.y / (double)GRAPH_RESOLUTION
					}
				);
				new_curve->pt_move(
					GraphCurve::ActivePoint::End,
					mkaul::Point{
						pts[i + 1u].pt_center.x / (double)GRAPH_RESOLUTION,
						pts[i + 1u].pt_center.y / (double)GRAPH_RESOLUTION
					}
				);
				new_curve->pt_end_move();

				// ハンドルの移動
				auto p_curve_bezier = dynamic_cast<BezierCurve*>(new_curve.get());
				p_curve_bezier->handle_left()->set_pt_offset(
					mkaul::Point{
						(pts[i].pt_right.x - pts[i].pt_center.x) / (double)GRAPH_RESOLUTION,
						(pts[i].pt_right.y - pts[i].pt_center.y) / (double)GRAPH_RESOLUTION
					}
				);
				p_curve_bezier->handle_right()->set_pt_offset(
					mkaul::Point{
						(pts[i + 1u].pt_left.x - pts[i + 1u].pt_center.x) / (double)GRAPH_RESOLUTION,
						(pts[i + 1u].pt_left.y - pts[i + 1u].pt_center.y) / (double)GRAPH_RESOLUTION
					}
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

	// ハンドルを描画
	void NormalCurve::draw_handle(
		mkaul::graphics::Graphics* p_graphics,
		const View& view,
		float thickness,
		float root_radius,
		float tip_radius,
		float tip_thickness,
		bool cutoff_line,
		const mkaul::ColorF& color
	) const noexcept {
		using Stroke = mkaul::graphics::Stroke;
		constexpr double DASH_CONTRAST = 0.5;
		constexpr float DASH_LENGTH = 8.f;
		constexpr float DASH_BLANK_LENGTH = 5.f;
		constexpr float LINE_THICKNESS = 0.5f;

		if (p_graphics) {
			for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
				// ポイントの境界に点線を引く
				if (it != curve_segments_.begin()) {
					mkaul::WindowRectangle rect_wnd;
					float custom_dashes[] = { DASH_LENGTH, DASH_BLANK_LENGTH };
					auto dash_stroke = Stroke{
						LINE_THICKNESS,
						Stroke::DashStyle::Custom,
						Stroke::CapStyle::Flat,
						Stroke::CapStyle::Flat,
						Stroke::CapStyle::Round,
						Stroke::LineJoin::Round,
						custom_dashes,
						ARRAYSIZE(custom_dashes)
					};
					auto line_color = global::config.get_theme().bg_graph;
					line_color.invert();
					line_color.change_contrast(DASH_CONTRAST);

					p_graphics->get_rect(&rect_wnd);
					// 点線を引く(クライアントでの)X座標
					auto x_client = view.view_to_client((*it)->pt_start().pt(), rect_wnd).x;
					p_graphics->draw_line(
						mkaul::Point{ x_client, 0.f },
						mkaul::Point{ x_client, (float)rect_wnd.bottom },
						line_color,
						dash_stroke
					);
				}
				// 各カーブのハンドル・ポイントを描画
				(*it)->draw_handle(p_graphics, view, thickness, root_radius, tip_radius, tip_thickness, cutoff_line, color);
				(*it)->draw_pt(p_graphics, view, root_radius, color);
			}
		}
	}

	// ポイントを追加
	bool NormalCurve::add_curve(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		constexpr double HANDLE_DEFAULT_LENGTH = 50;

		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			// X座標がカーブの始点と終点の範囲内であった場合
			if (mkaul::in_range(pt.x, (*it)->pt_start().x(), (*it)->pt_end().x())) {
				std::unique_ptr<GraphCurve> new_curve{
					new BezierCurve{
						pt,
						(*it)->pt_end().pt(),
						false,
						(*it).get(),
						(*it)->next(),
						mkaul::Point{
							((*it)->pt_end().x() - pt.x) * 0.3,
							((*it)->pt_end().y() - pt.y) * 0.3,
						},
						mkaul::Point{
							(pt.x - (*it)->pt_end().x()) * 0.3,
							(pt.y - (*it)->pt_end().y()) * 0.3,
						}
					}
				};
				// 既存のカーブを移動させる
				(*it)->pt_begin_move(ActivePoint::End);
				(*it)->pt_move(ActivePoint::End, pt);
				(*it)->pt_end_move();

				// 左のカーブがベジェの場合、右のハンドルをnew_curveの右ハンドルにコピーする
				// TODO: このあたりの処理がゴリ押しだから何とかしたい
				if (typeid(**it) == typeid(BezierCurve)) {
					auto bezier_prev = dynamic_cast<BezierCurve*>((*it).get());
					auto bezier_new = dynamic_cast<BezierCurve*>(new_curve.get());
					bezier_new->handle_right()->set_position(
						new_curve->pt_end().pt() + bezier_prev->handle_right()->pt_offset()
					);
					bezier_new->handle_left()->set_position(
						new_curve->pt_start().pt() + mkaul::Point{ HANDLE_DEFAULT_LENGTH / view.scale_x(), 0. }
					);
					bezier_prev->handle_right()->set_position(
						new_curve->pt_start().pt() - mkaul::Point{ HANDLE_DEFAULT_LENGTH / view.scale_x(), 0. }
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
	bool NormalCurve::delete_curve(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			// 先頭のカーブでない場合
			if (*it != curve_segments_.front()) {
				// カーソルが始点にホバーしている場合
				if ((*it)->pt_start().is_hovered(pt, view)) {
					// 前のカーブの終点を削除するカーブの終点に移動させる
					(*std::prev(it))->pt_move(ActivePoint::End, (*it)->pt_end().pt());

					// 自身と前のカーブがともにベジェの場合
					if (typeid(**it) == typeid(BezierCurve) and typeid(**std::prev(it)) == typeid(BezierCurve)) {
						// 前のベジェの右ハンドルを自身の右ハンドルにする
						auto bezier_prev = dynamic_cast<BezierCurve*>((*std::prev(it)).get());
						auto bezier_self = dynamic_cast<BezierCurve*>((*it).get());
						bezier_prev->handle_right()->set_position(
							(*it)->pt_end().pt() + bezier_self->handle_right()->pt_offset()
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
						bezier_prev->handle_right()->lock_length(view);
						bezier_prev->handle_right()->move(
							(*it)->pt_end().pt() - bezier_next->handle_left()->pt_offset(),
							view, true
						);
						bezier_prev->handle_right()->unlock_length();
					}

					// prev, nextの更新
					(*std::prev(it))->set_next((*it)->next());
					if ((*it)->next()) {
						(*it)->next()->set_prev((*std::prev(it)).get());
					}
					curve_segments_.erase(it);
					return true;
				}
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
				current_curve->pt_start().pt(),
				current_curve->pt_end().pt(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			break;

		case CurveSegmentType::Bezier:
			// TODO: ハンドルの整列が有効かつ隣がベジェのカーブの場合にハンドルを回転させる処理
			new_curve = std::make_unique<BezierCurve>(
				current_curve->pt_start().pt(),
				current_curve->pt_end().pt(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			new_curve->clear();
			break;

		case CurveSegmentType::Elastic:
			new_curve = std::make_unique<ElasticCurve>(
				current_curve->pt_start().pt(),
				current_curve->pt_end().pt(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			break;

		case CurveSegmentType::Bounce:
			new_curve = std::make_unique<BounceCurve>(
				current_curve->pt_start().pt(),
				current_curve->pt_end().pt(),
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

	bool NormalCurve::is_pt_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		for (auto& curve : curve_segments_) {
			if (curve->is_pt_hovered(pt, view)) {
				return true;
			}
		}
		return false;
	}

	bool NormalCurve::is_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		for (auto& curve : curve_segments_) {
			if (curve->is_handle_hovered(pt, view)) {
				return true;
			}
		}
		return false;
	}

	// カーソルがハンドルにホバーしているかを判定
	bool NormalCurve::handle_check_hover(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept {
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			auto previous = *it != curve_segments_.front() ? (*std::prev(it)).get() : nullptr;
			auto next = *it != curve_segments_.back() ? (*std::next(it)).get() : nullptr;

			if ((*it)->handle_check_hover(pt, view)) {
				return true;
			}
		}
		return false;
	}

	// ハンドルの位置をアップデート
	bool NormalCurve::handle_update(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept {
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			auto previous = *it != curve_segments_.front() ? (*std::prev(it)).get() : nullptr;
			auto next = *it != curve_segments_.back() ? (*std::next(it)).get() : nullptr;

			if ((*it)->handle_update(pt, view)) {
				return true;
			}
		}
		return false;
	}

	void NormalCurve::handle_end_control() noexcept {
		for (auto& curve : curve_segments_) {
			curve->handle_end_control();
		}
	}

	NormalCurve::ActivePoint NormalCurve::pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			// 必ずpt_endに最初にヒットする
			auto ret = (*it)->pt_check_hover(pt, view);

			if (ret == ActivePoint::End and *it != curve_segments_.back()) {
				// 後ろのカーブのpt_startもbegin_move()させる
				(*std::next(it))->pt_begin_move(ActivePoint::Start);
				return ret;
			}
			else {
				(*it)->pt_end_control();
			}
		}
		return ActivePoint::Null;
	}

	NormalCurve::ActivePoint NormalCurve::pt_update(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		auto ret = ActivePoint::Null;
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			auto next = *it != curve_segments_.back() ? (*std::next(it)).get() : nullptr;
			double min_x = (*it)->pt_start().x();
			double max_x = next ? next->pt_end().x() : 1.;
			double x = mkaul::clamp(pt.x, min_x, max_x);
			auto pt_tmp = mkaul::Point{ x, pt.y };
			auto active_pt = (*it)->pt_update(pt_tmp, view);
			if (active_pt == ActivePoint::End and next) {
				next->pt_move(ActivePoint::Start, pt);
				ret = active_pt;
			}
		}
		return ret;
	}

	void NormalCurve::pt_end_move() noexcept {
		for (auto& curve : curve_segments_) {
			curve->pt_end_move();
		}
	}

	void NormalCurve::pt_end_control() noexcept {
		for (auto& curve : curve_segments_) {
			curve->pt_end_control();
		}
	}
}