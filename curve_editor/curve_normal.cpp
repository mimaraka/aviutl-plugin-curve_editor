#include "curve_normal.hpp"
#include "config.hpp"
#include "curve_bezier.hpp"
#include "curve_bounce.hpp"
#include "curve_elastic.hpp"
#include "curve_linear.hpp"
#include "curve_step.hpp"



namespace cved {
	NormalCurve::NormalCurve(
		const mkaul::Point<double>& point_start,
		const mkaul::Point<double>& point_end
	) :
		GraphCurve{ point_start, point_end, true, nullptr, nullptr },
		curve_segments_{}
	{
		clear();
	}

	// カーブの値を取得
	double NormalCurve::get_value(double progress, double start, double end) const noexcept {
		for (auto it = curve_segments_.begin(), ed = curve_segments_.end(); it != ed; it++) {
			if (*it == curve_segments_.front() and progress < (*it)->point_start().x()) {
				return start;
			}
			else if (*it == curve_segments_.back() and (*it)->point_end().x() < progress) {
				return end;
			}
			else {
				if (*it != curve_segments_.back() and mkaul::in_range(progress, (*it)->point_end().x(), (*std::next(it))->point_start().x())) {
					// 線形補間
					double tmp = (progress - (*it)->point_end().x()) / ((*std::next(it))->point_start().x() - (*it)->point_end().x());
					return ((*std::next(it))->point_start().y() - (*it)->point_end().y()) * tmp + (*it)->point_end().y();
				}
				else if (mkaul::in_range(progress, (*it)->point_start().x(), (*it)->point_end().x(), true)) {
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
		for (size_t i = 0; i < curve_segments_.size() / 2; i++) {
			std::swap(curve_segments_[i], curve_segments_[curve_segments_.size() - i - 1]);
		}
		for (auto& p_curve : curve_segments_) {
			p_curve->reverse();
			// prev, nextも反転
			auto tmp = p_curve->prev();
			p_curve->set_prev(p_curve->next());
			p_curve->set_next(tmp);
		}
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
				auto x_client = view.view_to_client((*it)->point_start().point(), rect_wnd).x;
				p_graphics->draw_line(
					mkaul::Point{ x_client, 0.f },
					mkaul::Point{ x_client, (float)rect_wnd.bottom },
					line_color,
					dash_stroke
				);
			}
			// 各カーブのハンドル・ポイントを描画
			(*it)->draw_handle(p_graphics, view, thickness, root_radius, tip_radius, tip_thickness, cutoff_line, color);
			(*it)->draw_point(p_graphics, view, root_radius, color);
		}
	}

	// ポイントを追加
	bool NormalCurve::add_curve(const mkaul::Point<double>& point, const GraphView& view) noexcept {
		constexpr double HANDLE_DEFAULT_LENGTH = 50;

		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			// X座標がカーブの始点と終点の範囲内であった場合
			if (mkaul::in_range(point.x, (*it)->point_start().x(), (*it)->point_end().x())) {
				std::unique_ptr<GraphCurve> new_curve{
					new BezierCurve{
						point,
						(*it)->point_end().point(),
						false,
						(*it).get(),
						(*it)->next(),
						mkaul::Point{
							((*it)->point_end().x() - point.x) * 0.3,
							((*it)->point_end().y() - point.y) * 0.3,
						},
						mkaul::Point{
							(point.x - (*it)->point_end().x()) * 0.3,
							(point.y - (*it)->point_end().y()) * 0.3,
						}
					}
				};
				// 既存のカーブを移動させる
				(*it)->point_begin_move(ActivePoint::End, view);
				(*it)->point_move(ActivePoint::End, point);
				(*it)->point_end_move();

				// 左のカーブがベジェの場合、右のハンドルをnew_curveの右ハンドルにコピーする
				if (typeid(**it) == typeid(BezierCurve)) {
					auto bezier_prev = reinterpret_cast<BezierCurve*>((*it).get());
					auto bezier_new = reinterpret_cast<BezierCurve*>(new_curve.get());
					bezier_new->handle_right()->set_position(
						new_curve->point_end().point() + bezier_prev->handle_right()->point_offset()
					);
					bezier_new->handle_left()->set_position(
						new_curve->point_start().point() + mkaul::Point{ HANDLE_DEFAULT_LENGTH / view.scale_x(), 0. }
					);
					bezier_prev->handle_right()->set_position(
						new_curve->point_start().point() - mkaul::Point{ HANDLE_DEFAULT_LENGTH / view.scale_x(), 0. }
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
	bool NormalCurve::delete_curve(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept {
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			// 先頭のカーブでない場合
			if (*it != curve_segments_.front()) {
				// カーソルが始点にホバーしている場合
				if ((*it)->point_start().is_hovered(point, box_width, view)) {
					// 前のカーブの終点を削除するカーブの終点に移動させる
					(*std::prev(it))->point_move(ActivePoint::End, (*it)->point_end().point());

					// 自身と前のカーブがともにベジェの場合
					if (typeid(**it) == typeid(BezierCurve) and typeid(**std::prev(it)) == typeid(BezierCurve)) {
						// 前のベジェの右ハンドルを自身の右ハンドルにする
						auto bezier_prev = reinterpret_cast<BezierCurve*>((*std::prev(it)).get());
						auto bezier_self = reinterpret_cast<BezierCurve*>((*it).get());
						bezier_prev->handle_right()->set_position(
							(*it)->point_end().point() + bezier_self->handle_right()->point_offset()
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
						auto bezier_prev = reinterpret_cast<BezierCurve*>((*std::prev(it)).get());
						auto bezier_next = reinterpret_cast<BezierCurve*>((*std::next(it)).get());
						bezier_prev->handle_right()->lock_length(view);
						bezier_prev->handle_right()->move(
							(*it)->point_end().point() - bezier_next->handle_left()->point_offset(),
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

	bool NormalCurve::replace_curve(size_t index, CurveSegmentType segment_type) noexcept {
		if (index >= curve_segments_.size()) return false;
		auto current_curve = curve_segments_[index].get();
		std::unique_ptr<GraphCurve> new_curve;
		switch (segment_type) {
		case CurveSegmentType::Linear:
			new_curve = std::make_unique<LinearCurve>(
				current_curve->point_start().point(),
				current_curve->point_end().point(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			break;

		case CurveSegmentType::Bezier:
			// TODO: ハンドルの整列が有効かつ隣がベジェのカーブの場合にハンドルを回転させる処理
			new_curve = std::make_unique<BezierCurve>(
				current_curve->point_start().point(),
				current_curve->point_end().point(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			new_curve->clear();
			break;

		case CurveSegmentType::Elastic:
			new_curve = std::make_unique<ElasticCurve>(
				current_curve->point_start().point(),
				current_curve->point_end().point(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			break;

		case CurveSegmentType::Bounce:
			new_curve = std::make_unique<BounceCurve>(
				current_curve->point_start().point(),
				current_curve->point_end().point(),
				false,
				current_curve->prev(),
				current_curve->next()
			);
			break;

		case CurveSegmentType::Step:
			new_curve = std::make_unique<StepCurve>(
				current_curve->point_start().point(),
				current_curve->point_end().point(),
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
		curve_segments_[index] = std::move(new_curve);

		return true;
	}

	bool NormalCurve::is_point_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept {
		for (auto& curve : curve_segments_) {
			if (curve->is_point_hovered(point, box_width, view)) {
				return true;
			}
		}
		return false;
	}

	bool NormalCurve::is_handle_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept {
		for (auto& curve : curve_segments_) {
			if (curve->is_handle_hovered(point, box_width, view)) {
				return true;
			}
		}
		return false;
	}

	// カーソルがハンドルにホバーしているかを判定
	bool NormalCurve::handle_check_hover(
		const mkaul::Point<double>& point,
		float box_width,
		const GraphView& view
	) noexcept {
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			auto previous = *it != curve_segments_.front() ? (*std::prev(it)).get() : nullptr;
			auto next = *it != curve_segments_.back() ? (*std::next(it)).get() : nullptr;

			if ((*it)->handle_check_hover(point, box_width, view)) {
				return true;
			}
		}
		return false;
	}

	// ハンドルの位置をアップデート
	bool NormalCurve::handle_update(
		const mkaul::Point<double>& point,
		const GraphView& view
	) noexcept {
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			auto previous = *it != curve_segments_.front() ? (*std::prev(it)).get() : nullptr;
			auto next = *it != curve_segments_.back() ? (*std::next(it)).get() : nullptr;

			if ((*it)->handle_update(point, view)) {
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

	NormalCurve::ActivePoint NormalCurve::point_check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept {
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			// 必ずpoint_endに最初にヒットする
			auto result = (*it)->point_check_hover(point, box_width, view);

			if (result == ActivePoint::End and *it != curve_segments_.back()) {
				// 後ろのカーブのpoint_startもbegin_move()させる
				(*std::next(it))->point_begin_move(ActivePoint::Start, view);
				return result;
			}
			else {
				(*it)->point_end_control();
			}
		}
		return ActivePoint::Null;
	}

	NormalCurve::ActivePoint NormalCurve::point_update(const mkaul::Point<double>& point, const GraphView& view) noexcept {
		auto result = ActivePoint::Null;
		for (auto it = curve_segments_.begin(), end = curve_segments_.end(); it != end; it++) {
			auto next = *it != curve_segments_.back() ? (*std::next(it)).get() : nullptr;
			double min_x = (*it)->point_start().x();
			double max_x = next ? next->point_end().x() : 1.;
			double x = mkaul::clamp(point.x, min_x, max_x);
			auto pt = mkaul::Point{ x, point.y };
			auto tmp = (*it)->point_update(pt, view);
			if (tmp == ActivePoint::End and next) {
				next->point_move(ActivePoint::Start, pt);
				result = tmp;
			}
		}
		return result;
	}

	void NormalCurve::point_end_move() noexcept {
		for (auto& curve : curve_segments_) {
			curve->point_end_move();
		}
	}

	void NormalCurve::point_end_control() noexcept {
		for (auto& curve : curve_segments_) {
			curve->point_end_control();
		}
	}
}