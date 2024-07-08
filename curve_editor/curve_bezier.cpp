#include "curve_bezier.hpp"
#include <mkaul/graphics.hpp>
#include <mkaul/util.hpp>
#include "config.hpp"
#include "util.hpp"



namespace cved {
	// コンストラクタ
	BezierCurve::BezierCurve(
		const mkaul::Point<double>& pt_start,
		const mkaul::Point<double>& pt_end,
		bool pt_fixed,
		GraphCurve* prev,
		GraphCurve* next,
		const mkaul::Point<double>& handle_left,
		const mkaul::Point<double>& handle_right
	) noexcept :
		NumericGraphCurve{ pt_start, pt_end, pt_fixed, prev, next },
		handle_left_{ this, BezierHandle::Type::Left, handle_left },
		handle_right_{ this, BezierHandle::Type::Right, handle_right },
		handle_buffer_left_{ this, BezierHandle::Type::Left },
		handle_buffer_right_{ this, BezierHandle::Type::Right },
		flag_prev_move_symmetrically_{ false }
	{
		handle_left_.set_position(pt_start + handle_left);
		handle_right_.set_position(pt_end + handle_right);
		if (prev) {
			set_prev(prev);
		}
		if (next) {
			set_next(next);
		}
	}

	// コピーコンストラクタ
	BezierCurve::BezierCurve(const BezierCurve& curve) noexcept :
		NumericGraphCurve{ curve },
		handle_left_{this, curve.handle_left_.type(), curve.handle_left_.pt_offset(), curve.handle_left_.is_fixed() },
		handle_right_{ this, curve.handle_right_.type(), curve.handle_right_.pt_offset(), curve.handle_right_.is_fixed() },
		handle_buffer_left_{ this, BezierHandle::Type::Left },
		handle_buffer_right_{ this, BezierHandle::Type::Right },
		flag_prev_move_symmetrically_{ curve.flag_prev_move_symmetrically_ }
	{}

	void BezierCurve::set_prev(GraphCurve* p) noexcept {
		BezierHandle* handle_opposite;
		if (p and typeid(*p) == typeid(BezierCurve)) {
			handle_opposite = dynamic_cast<BezierCurve*>(p)->handle_right();
		}
		else {
			handle_opposite = nullptr;
		}
		handle_left_.set_handle_opposite(handle_opposite);
		GraphCurve::set_prev(p);
	}

	void BezierCurve::set_next(GraphCurve* p) noexcept {
		BezierHandle* handle_opposite;
		if (p and typeid(*p) == typeid(BezierCurve)) {
			handle_opposite = dynamic_cast<BezierCurve*>(p)->handle_left();
		}
		else {
			handle_opposite = nullptr;
		}
		handle_right_.set_handle_opposite(handle_opposite);
		GraphCurve::set_next(p);
	}

	// カーブの値を取得
	double BezierCurve::curve_function(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp(progress, pt_start().x(), pt_end().x());

		auto pt0 = pt_start().pt();
		auto pt1 = pt_start().pt() + handle_left_.pt_offset();
		auto pt2 = pt_end().pt() + handle_right_.pt_offset();
		auto pt3 = pt_end().pt();

		auto func_bezier = [](double v0, double v1, double v2, double v3, double t) {
			return (v3 + 3. * (v1 - v2) - v0) * t * t * t + (v2 - 2 * v1 + v0) * 3 * t * t + (v1 - v0) * 3 * t + v0;
			};

		double t = 0.;
		const double tmp = pt3.x + 3. * (pt1.x - pt2.x) - pt0.x;
		const double tmp2 = pt2.x - 2. * pt1.x + pt0.x;
		const double tmp3 = pt1.x - pt0.x;
		const double tmp4 = pt0.x - progress;
		// tの次数は2以下
		if (mkaul::real_equal(tmp, 0.)) {
			// tの次数は1以下
			if (mkaul::real_equal(tmp2, 0.)) {
				if (!mkaul::real_equal(tmp3, 0.)) {
					t = -tmp4 / (3. * tmp3);
				}
			}
			// tの次数は2
			else {
				const double inside_sqrt = 9. * tmp3 * tmp3 - 12. * tmp4 * tmp2;
				const double t_1 = (-3. * tmp3 + std::sqrt(inside_sqrt)) / (6. * tmp2);
				const double t_2 = (-3. * tmp3 - std::sqrt(inside_sqrt)) / (6. * tmp2);
				if (mkaul::real_in_range(t_1, 0., 1., true)) {
					t = t_1;
				}
				else if (mkaul::real_in_range(t_2, 0., 1., true)) {
					t = t_2;
				}
			}
		}
		else {
			const double a0 = tmp4 / tmp;
			const double a1 = 3. * tmp3 / tmp;
			const double a2 = 3. * tmp2 / tmp;
			const double tmp5 = a2 / 3.;
			const double p = a1 - a2 * a2 / 3.;
			const double q = a0 - a1 * a2 / 3. + a2 * a2 * a2 * 2. / 27.;
			const double tmp6 = q * q * 0.25 + p * p * p / 27.;

			// 相異なる3つの実数解を持つ場合
			if (tmp6 < 0.) {
				double tmp7 = std::sqrt(-tmp6);
				double r_2 = std::pow(q * q * 0.25 - tmp6, 0.5 / 3.) * 2.;
				double theta = std::atan2(tmp7, -q * 0.5) / 3.;
				double re_t0 = r_2 * std::cos(theta) - tmp5;
				double re_t1 = r_2 * std::cos(theta + std::numbers::pi * 2. / 3.) - tmp5;
				double re_t2 = r_2 * std::cos(theta + std::numbers::pi * 4. / 3.) - tmp5;

				if (mkaul::real_in_range(re_t0, 0., 1., true)) {
					t = re_t0;
				}
				else if (mkaul::real_in_range(re_t1, 0., 1., true)) {
					t = re_t1;
				}
				else if (mkaul::real_in_range(re_t2, 0., 1., true)) {
					t = re_t2;
				}
			}
			// 実数の2・3重解を持つ場合
			else if (mkaul::real_equal(tmp6, 0.)) {
				double tmp7 = std::pow(-q * 0.5, 1. / 3.);
				double t0 = -tmp7 - tmp5;
				double t1 = 2. * tmp7 - tmp5;

				if (mkaul::real_in_range(t0, 0., 1., true)) {
					t = t0;
				}
				else if (mkaul::real_in_range(t1, 0., 1., true)) {
					t = t1;
				}
			}
			// 1つの実数解と2つの共役な虚数解を持つ場合
			else {
				double tmp7 = std::sqrt(tmp6);
				double tmp8 = -q * 0.5;
				double tmp9 = tmp8 + tmp7;
				double tmp10 = tmp8 - tmp7;
				double tmp11 = tmp9 != 0. ? tmp9 * std::pow(std::abs(tmp9), -2. / 3.) : 0.;
				double tmp12 = tmp10 != 0. ? tmp10 * std::pow(std::abs(tmp10), -2. / 3.) : 0.;
				t = tmp11 + tmp12 - tmp5;
			}
		}

		// 相対値
		double rel_value;
		// 端点がおかしくなる問題の解決
		if (mkaul::real_equal(progress, 0.)) {
			rel_value = pt_start().y();
		}
		else if (mkaul::real_equal(progress, 1.)) {
			rel_value = pt_end().y();
		}
		else {
			rel_value = func_bezier(pt0.y, pt1.y, pt2.y, pt3.y, t);
		}
		return start + (end - start) * rel_value;
	}

	void BezierCurve::clear() noexcept {
		handle_left_.set_position(
			pt_start().pt() + mkaul::Point{
				(pt_end().x() - pt_start().x()) * 0.3,
				(pt_end().y() - pt_start().y()) * 0.3,
			}
		);
		handle_right_.set_position(
			pt_end().pt() + mkaul::Point{
				(pt_start().x() - pt_end().x()) * 0.3,
				(pt_start().y() - pt_end().y()) * 0.3,
			}
		);
	}

	void BezierCurve::reverse(bool fix_pt) noexcept {
		auto offset_tmp = handle_left_.pt_offset();
		handle_left_.set_pt_offset(-handle_right_.pt_offset());
		handle_right_.set_pt_offset(-offset_tmp);
		GraphCurve::reverse(fix_pt);
	}

	int32_t BezierCurve::encode() const noexcept {
		constexpr float MAX_Y = 3.73f;
		constexpr float MIN_Y = -2.73f;
		int ret;
		float x1, y1, x2, y2;
		int ix1, iy1, ix2, iy2;
		const double width = pt_end().x() - pt_start().x();
		const double height = pt_end().y() - pt_start().y();

		x1 = static_cast<float>(handle_left_.pt_offset().x / width);
		y1 = mkaul::clamp(
			static_cast<float>(handle_left_.pt_offset().y / height),
			MIN_Y, MAX_Y
		);
		x2 = static_cast<float>(1. + handle_right_.pt_offset().x / width);
		y2 = mkaul::clamp(
			static_cast<float>(1. + handle_right_.pt_offset().y / height),
			MIN_Y, MAX_Y
		);

		ix1 = (int)std::round(x1 * 100.f);
		iy1 = (int)std::round(y1 * 100.f);
		ix2 = (int)std::round(x2 * 100.f);
		iy2 = (int)std::round(y2 * 100.f);

		// 計算
		ret = 6600047 * (iy2 + 273) + 65347 * ix2 + 101 * (iy1 + 273) + ix1 - 2147483647;
		if (ret >= -12368442) {
			ret += 24736885;
		}
		return ret;
	}

	bool BezierCurve::decode(int32_t code) noexcept {
		// -2147483647 ~  -12368443 : For bezier curves
		//   -12368442 ~         -1 : For ID curves
		//           0              : Unused
		//           1 ~   12368442 : For ID curves
		//    12368443 ~ 2147483646 : For bezier curves
		//  2147483647              : Unused

		int64_t tmp;

		if (code <= -12368443) {
			tmp = (int64_t)code + (int64_t)INT32_MAX;
		}
		else if (12368443 <= code and code < INT32_MAX) {
			tmp = (int64_t)code + (int64_t)2122746762;
		}
		else return false;

		int ix1, iy1, ix2, iy2;
		double x1, y1, x2, y2;
		const double width = pt_end().x() - pt_start().x();
		const double height = pt_end().y() - pt_start().y();

		iy2 = (int32_t)(tmp / 6600047);
		ix2 = (int32_t)((tmp - (int64_t)iy2 * 6600047) / 65347);
		iy1 = (int32_t)((tmp - ((int64_t)iy2 * 6600047 + (int64_t)ix2 * 65347)) / 101);
		ix1 = (int32_t)((tmp - ((int64_t)iy2 * 6600047 + (int64_t)ix2 * 65347)) % 101);
		iy1 -= 273;
		iy2 -= 273;
		x1 = (double)ix1 * 0.01;
		y1 = (double)iy1 * 0.01;
		x2 = (double)ix2 * 0.01;
		y2 = (double)iy2 * 0.01;

		handle_left_.set_position(
			mkaul::Point{
				pt_start().x() + width * x1,
				pt_start().y() + height * y1
			}
		);

		handle_right_.set_position(
			mkaul::Point{
				pt_start().x() + width * x2,
				pt_start().y() + height * y2
			}
		);

		return true;
	}

	std::string BezierCurve::make_param() const noexcept {
		const double width = pt_end().x() - pt_start().x();
		const double height = pt_end().y() - pt_start().y();

		float x1 = static_cast<float>(handle_left_.pt_offset().x / width);
		float y1 = static_cast<float>(handle_left_.pt_offset().y / height);
		float x2 = static_cast<float>(handle_right_.pt_offset().x / width + 1.f);
		float y2 = static_cast<float>(handle_right_.pt_offset().y / height + 1.f);

		return std::format("{:.2f}, {:.2f}, {:.2f}, {:.2f}", x1, y1, x2, y2);
	}

	// ハンドルを描画
	void BezierCurve::draw_handle(
		mkaul::graphics::Graphics* p_graphics,
		const View& view,
		float thickness,
		float root_radius,
		float tip_radius,
		float tip_thickness,
		bool cutoff_line,
		const mkaul::ColorF& color
	) const noexcept {
		if (p_graphics) {
			mkaul::WindowRectangle rect_wnd;
			p_graphics->get_rect(&rect_wnd);

			auto start_client = view.view_to_client(pt_start().pt(), rect_wnd);
			auto left_client = view.view_to_client(pt_start().pt() + handle_left_.pt_offset(), rect_wnd);
			auto right_client = view.view_to_client(pt_end().pt() + handle_right_.pt_offset(), rect_wnd);
			auto end_client = view.view_to_client(pt_end().pt(), rect_wnd);

			auto start_cutoff = start_client;
			auto left_cutoff = left_client;
			auto right_cutoff = right_client;
			auto end_cutoff = end_client;

			if (cutoff_line) {
				util::get_cutoff_line(&start_cutoff, left_client, start_client, root_radius + 4.f);
				util::get_cutoff_line(&left_cutoff, start_client, left_client, tip_radius + 4.f);
				util::get_cutoff_line(&right_cutoff, end_client, right_client, tip_radius + 4.f);
				util::get_cutoff_line(&end_cutoff, right_client, end_client, root_radius + 4.f);
			}
			// 線 (左ハンドル)
			p_graphics->draw_line(start_cutoff, left_cutoff, color, mkaul::graphics::Stroke{ thickness });
			// 線 (右ハンドル)
			p_graphics->draw_line(end_cutoff, right_cutoff, color, mkaul::graphics::Stroke{ thickness });
			// 先端の円 (左ハンドル)
			p_graphics->draw_ellipse(left_client, tip_radius, tip_radius, color, mkaul::graphics::Stroke{ tip_thickness });
			// 先端の円 (右ハンドル)
			p_graphics->draw_ellipse(right_client, tip_radius, tip_radius, color, mkaul::graphics::Stroke{ tip_thickness });
		}
	}

	// カーソルが左右いずれかのハンドルにホバーしているか
	bool BezierCurve::is_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		return is_left_handle_hovered(pt, view) or is_right_handle_hovered(pt, view);
	}

	// 左ハンドルにホバーしているか
	bool BezierCurve::is_left_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		return handle_left_.is_hovered(pt, view);
	}

	// 右ハンドルにホバーしているか
	bool BezierCurve::is_right_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		return handle_right_.is_hovered(pt, view);
	}

	void BezierCurve::adjust_handle_angle(BezierHandle::Type type, const GraphView& view) noexcept {
		switch (type) {
		case BezierHandle::Type::Left:
			handle_left_.adjust_angle(prev(), view);
			break;

		case BezierHandle::Type::Right:
			handle_right_.adjust_angle(next(), view);
			break;
		}
	}

	bool BezierCurve::handle_check_hover(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept {
		return handle_left_.check_hover(pt, view) or handle_right_.check_hover(pt, view);
	}

	bool BezierCurve::handle_update(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept {
		bool ks_move_symmetrically = ::GetAsyncKeyState(VK_CONTROL) & 0x8000 and ::GetAsyncKeyState(VK_SHIFT) & 0x8000;
		bool ret = false;

		if (handle_left_.update(pt, view)) {
			if (ks_move_symmetrically) {
				// フラグ立ち上がり検出
				if (!flag_prev_move_symmetrically_) {
					handle_right_.begin_move(view);
				}
				handle_right_.move(pt_end().pt() - handle_left_.pt_offset(), view, false, true);
			}
			ret = true;
		}
		else if (handle_right_.update(pt, view)) {
			if (ks_move_symmetrically) {
				// フラグ立ち上がり検出
				if (!flag_prev_move_symmetrically_) {
					handle_left_.begin_move(view);
				}
				handle_left_.move(pt_start().pt() - handle_right_.pt_offset(), view, false, true);
			}
			ret = true;
		}
		flag_prev_move_symmetrically_ = ks_move_symmetrically;
		return ret;
	}

	// ハンドルの移動を終了
	void BezierCurve::handle_end_control() noexcept {
		handle_left_.end_control();
		handle_right_.end_control();
	}

	BezierCurve::ActivePoint BezierCurve::pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		bool start = pt_start_.check_hover(pt, view);
		bool end = pt_end_.check_hover(pt, view);

		if (start or end) {
			handle_buffer_left_.set_pt_offset(handle_left_.pt_offset());
			handle_buffer_right_.set_pt_offset(handle_right_.pt_offset());

			if (start) return ActivePoint::Start;
			else return ActivePoint::End;
		}
		else return ActivePoint::Null;
	}

	bool BezierCurve::pt_begin_move(ActivePoint active_pt) noexcept {
		handle_buffer_left_.set_pt_offset(handle_left_.pt_offset());
		handle_buffer_right_.set_pt_offset(handle_right_.pt_offset());
		return true;
	}

	BezierCurve::ActivePoint BezierCurve::pt_update(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		bool moved_start = pt_start_.update(mkaul::Point{ std::min(pt.x, pt_end().x()), pt.y });
		bool moved_end = pt_end_.update(mkaul::Point{ std::max(pt.x, pt_start().x()), pt.y });

		if (moved_start or moved_end) {
			handle_left_.set_position(pt_start().pt() + handle_buffer_left_.pt_offset());
			handle_right_.set_position(pt_end().pt() + handle_buffer_right_.pt_offset());
			if (moved_start) return ActivePoint::Start;
			else return ActivePoint::End;
		}
		else return ActivePoint::Null;
	}

	bool BezierCurve::pt_move(ActivePoint active_pt, const mkaul::Point<double>& pt) noexcept {
		if (GraphCurve::pt_move(active_pt, pt)) {
			handle_left_.set_position(pt_start().pt() + handle_buffer_left_.pt_offset());
			handle_right_.set_position(pt_end().pt() + handle_buffer_right_.pt_offset());
			return true;
		}
		else return false;
	}

	void BezierCurve::pt_end_move() noexcept {
		handle_left_.end_move();
		handle_right_.end_move();
	}

	void BezierCurve::pt_end_control() noexcept {
		handle_left_.end_control();
		handle_right_.end_control();
		GraphCurve::pt_end_control();
	}
}