#include "config.hpp"
#include "curve_bezier.hpp"
#include "util.hpp"
#include <mkaul/graphics.hpp>
#include <mkaul/util.hpp>



namespace cved {
	// コンストラクタ
	BezierCurve::BezierCurve(
		const mkaul::Point<double>& anchor_start,
		const mkaul::Point<double>& anchor_end,
		bool pt_fixed,
		GraphCurve* prev,
		GraphCurve* next,
		mkaul::Point<double> handle_left,
		mkaul::Point<double> handle_right
	) noexcept :
		NumericGraphCurve{ anchor_start, anchor_end, pt_fixed, prev, next },
		handle_{ handle_left, handle_right }
	{}

	// コピーコンストラクタ
	BezierCurve::BezierCurve(const BezierCurve& curve) noexcept :
		NumericGraphCurve{ curve },
		handle_{ curve.handle_ }
	{}

	// カーブの値を取得
	double BezierCurve::curve_function(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp(progress, anchor_start().x, anchor_end().x);

		auto pt0 = anchor_start();
		auto pt1 = anchor_start() + handle_.left_offset();
		auto pt2 = anchor_end() + handle_.right_offset();
		auto pt3 = anchor_end();

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
				if (mkaul::real_in_range(t_1, 0., 1.)) {
					t = t_1;
				}
				else if (mkaul::real_in_range(t_2, 0., 1.)) {
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

				if (mkaul::real_in_range(re_t0, 0., 1.)) {
					t = re_t0;
				}
				else if (mkaul::real_in_range(re_t1, 0., 1.)) {
					t = re_t1;
				}
				else if (mkaul::real_in_range(re_t2, 0., 1.)) {
					t = re_t2;
				}
			}
			// 実数の2・3重解を持つ場合
			else if (mkaul::real_equal(tmp6, 0.)) {
				double tmp7 = std::pow(-q * 0.5, 1. / 3.);
				double t0 = -tmp7 - tmp5;
				double t1 = 2. * tmp7 - tmp5;

				if (mkaul::real_in_range(t0, 0., 1.)) {
					t = t0;
				}
				else if (mkaul::real_in_range(t1, 0., 1.)) {
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
			rel_value = anchor_start().y;
		}
		else if (mkaul::real_equal(progress, 1.)) {
			rel_value = anchor_end().y;
		}
		else {
			rel_value = func_bezier(pt0.y, pt1.y, pt2.y, pt3.y, t);
		}
		return start + (end - start) * rel_value;
	}

	void BezierCurve::begin_move_anchor_start(bool bound) noexcept {
		if (is_locked()) return;
		GraphCurve::begin_move_anchor_start(bound);
		handle_.on_anchor_begin_move();
	}

	void BezierCurve::begin_move_anchor_end(bool bound) noexcept {
		if (is_locked()) return;
		GraphCurve::begin_move_anchor_end(bound);
		handle_.on_anchor_begin_move();
	}

	void BezierCurve::move_anchor_start(double x, double y, bool forced, bool bound) noexcept {
		if (is_locked()) return;
		GraphCurve::move_anchor_start(x, y, forced, bound);
		if (!forced) {
			handle_.on_anchor_move(anchor_start(), anchor_end());
		}
	}

	void BezierCurve::move_anchor_end(double x, double y, bool forced, bool bound) noexcept {
		if (is_locked()) return;
		GraphCurve::move_anchor_end(x, y, forced, bound);
		if (!forced) {
			handle_.on_anchor_move(anchor_start(), anchor_end());
		}
	}

	void BezierCurve::begin_move_handle_left(double scale_x, double scale_y) noexcept {
		if (is_locked()) return;
		const mkaul::Point<double>* p_prev_handle_right = nullptr;
		auto prev_bezier = prev() ? global::id_manager.get_curve<BezierCurve>(prev()->get_id()) : nullptr;
		if (prev_bezier and global::config.get_align_handle()) {
			p_prev_handle_right = &prev_bezier->handle_.right_offset();
		}
		handle_.begin_move_handle_left(scale_x, scale_y, p_prev_handle_right);
	}

	void BezierCurve::begin_move_handle_right(double scale_x, double scale_y) noexcept {
		if (is_locked()) return;
		const mkaul::Point<double>* p_next_handle_left = nullptr;
		auto next_bezier = next() ? global::id_manager.get_curve<BezierCurve>(next()->get_id()) : nullptr;
		if (next_bezier and global::config.get_align_handle()) {
			p_next_handle_left = &next_bezier->handle_.left_offset();
		}
		handle_.begin_move_handle_right(scale_x, scale_y, p_next_handle_left);
	}

	void BezierCurve::move_handle_left(const mkaul::Point<double>& pt, bool keep_angle, bool bound) noexcept {
		if (is_locked()) return;		
		auto prev_bezier = prev() ? global::id_manager.get_curve<BezierCurve>(prev()->get_id()) : nullptr;
		if (!bound and prev_bezier and global::config.get_align_handle()) {
			mkaul::Point<double> prev_handle_right;
			handle_.move_handle_left(pt, anchor_start(), anchor_end(), keep_angle, &prev_handle_right);
			prev_bezier->handle_.move_handle_right(
				anchor_start() + prev_handle_right,
				prev_bezier->anchor_start(),
				prev_bezier->anchor_end(),
				true, nullptr
			);
		}
		else {
			handle_.move_handle_left(pt, anchor_start(), anchor_end(), keep_angle, nullptr);
		}
	}

	void BezierCurve::move_handle_right(const mkaul::Point<double>& pt, bool keep_angle, bool bound) noexcept {
		if (is_locked()) return;
		auto next_bezier = next() ? global::id_manager.get_curve<BezierCurve>(next()->get_id()) : nullptr;
		if (!bound and next_bezier and global::config.get_align_handle()) {
			mkaul::Point<double> next_handle_left;
			handle_.move_handle_right(pt, anchor_start(), anchor_end(), keep_angle, & next_handle_left);
			next_bezier->handle_.move_handle_left(
				anchor_end() + next_handle_left,
				next_bezier->anchor_start(),
				next_bezier->anchor_end(),
				true, nullptr
			);
		}
		else {
			handle_.move_handle_right(pt, anchor_start(), anchor_end(), keep_angle, nullptr);
		}
	}

	void BezierCurve::clear() noexcept {
		auto width = anchor_end().x - anchor_start().x;
		auto height = anchor_end().y - anchor_start().y;
		handle_.clear(width, height);
	}

	bool BezierCurve::is_default() const noexcept {
		auto width = anchor_end().x - anchor_start().x;
		auto height = anchor_end().y - anchor_start().y;
		return handle_.is_default(width, height);
	}

	void BezierCurve::reverse(bool fix_pt) noexcept {
		if (is_locked()) return;
		handle_.reverse();
		GraphCurve::reverse(fix_pt);
	}

	int32_t BezierCurve::encode() const noexcept {
		constexpr float MAX_Y = 3.73f;
		constexpr float MIN_Y = -2.73f;
		int ret;
		float x1, y1, x2, y2;
		int ix1, iy1, ix2, iy2;
		const double width = anchor_end().x - anchor_start().x;
		const double height = anchor_end().y - anchor_start().y;

		x1 = static_cast<float>(handle_.left_offset().x / width);
		y1 = mkaul::clamp(
			static_cast<float>(handle_.left_offset().y / height),
			MIN_Y, MAX_Y
		);
		x2 = static_cast<float>(1. + handle_.right_offset().x / width);
		y2 = mkaul::clamp(
			static_cast<float>(1. + handle_.right_offset().y / height),
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

		if (is_locked()) return false;

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
		const double width = anchor_end().x - anchor_start().x;
		const double height = anchor_end().y - anchor_start().y;

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

		handle_.move_handle_left(
			anchor_start() + mkaul::Point{ width * x1, height * y1 },
			anchor_start(),
			anchor_end(),
			false,
			nullptr
		);

		handle_.move_handle_right(
			anchor_end() + mkaul::Point{ width * (x2 - 1.), height * (y2 - 1.) },
			anchor_start(),
			anchor_end(),
			false,
			nullptr
		);

		return true;
	}

	std::string BezierCurve::create_params_str(size_t precision) const noexcept {
		const double width = anchor_end().x - anchor_start().x;
		const double height = anchor_end().y - anchor_start().y;

		auto x1 = handle_.left_offset().x / width;
		auto y1 = handle_.left_offset().y / height;
		auto x2 = handle_.right_offset().x / width + 1.;
		auto y2 = handle_.right_offset().y / height + 1.;

		std::ostringstream oss;
		oss << std::fixed << std::setprecision(precision) << x1;
		oss << ", " << std::fixed << std::setprecision(precision) << y1;
		oss << ", " << std::fixed << std::setprecision(precision) << x2;
		oss << ", " << std::fixed << std::setprecision(precision) << y2;
		return oss.str();
	}

	bool BezierCurve::read_params(const std::vector<double>& params) noexcept {
		if (is_locked()) return true;
		if (params.size() != 4) {
			return false;
		}
		auto x1 = params[0];
		auto y1 = params[1];
		auto x2 = params[2];
		auto y2 = params[3];
		auto width = anchor_end().x - anchor_start().x;
		auto height = anchor_end().y - anchor_start().y;
		if (!mkaul::real_in_range(x1, 0., 1.) or !mkaul::real_in_range(x2, 0., 1.)) {
			return false;
		}

		handle_.move_handle_left(
			anchor_start() + mkaul::Point{ x1 * width, y1 * height },
			anchor_start(),
			anchor_end(),
			false,
			nullptr
		);
		handle_.move_handle_right(
			anchor_end() + mkaul::Point{ (x2 - 1.) * width, (y2 - 1.) * height },
			anchor_start(),
			anchor_end(),
			false,
			nullptr
		);
		return true;
	}

	nlohmann::json BezierCurve::create_json() const noexcept {
		auto data = GraphCurve::create_json();
		data["handle_left"] = {
			handle_.left_offset().x, handle_.left_offset().y
		};
		data["handle_right"] = {
			handle_.right_offset().x, handle_.right_offset().y
		};
		return data;
	}

	bool BezierCurve::load_json(const nlohmann::json& data) noexcept {
		if (!GraphCurve::load_json(data)) {
			return false;
		}
		try {
			auto left_x = data.at("handle_left")[0].get<double>();
			auto left_y = data.at("handle_left")[1].get<double>();
			auto right_x = data.at("handle_right")[0].get<double>();
			auto right_y = data.at("handle_right")[1].get<double>();
			auto width = anchor_end().x - anchor_start().x;
			if (!mkaul::real_in_range(left_x, 0., width) or !mkaul::real_in_range(right_x, -width, 0.)) {
				return false;
			}
			handle_.move_handle_left(
				anchor_start() + mkaul::Point{ left_x, left_y },
				anchor_start(),
				anchor_end(),
				false,
				nullptr
			);

			handle_.move_handle_right(
				anchor_end() + mkaul::Point{ right_x, right_y },
				anchor_start(),
				anchor_end(),
				false,
				nullptr
			);
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}
} // namespace cved