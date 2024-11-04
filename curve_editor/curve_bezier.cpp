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
		handle_left_{ handle_left },
		handle_right_{ handle_right }
	{}

	// コピーコンストラクタ
	BezierCurve::BezierCurve(const BezierCurve& curve) noexcept :
		NumericGraphCurve{ curve },
		handle_left_{ curve.handle_left_ },
		handle_right_{ curve.handle_right_ }
	{}

	// カーブの値を取得
	double BezierCurve::curve_function(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp(progress, anchor_start().x, anchor_end().x);

		auto pt0 = anchor_start();
		auto pt1 = anchor_start() + handle_left_;
		auto pt2 = anchor_end() + handle_right_;
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
		handle_left_buffer_ = handle_left_;
		handle_right_buffer_ = handle_right_;
	}

	void BezierCurve::begin_move_anchor_end(bool bound) noexcept {
		if (is_locked()) return;
		GraphCurve::begin_move_anchor_end(bound);
		handle_left_buffer_ = handle_left_;
		handle_right_buffer_ = handle_right_;
	}

	void BezierCurve::move_anchor_start(double x, double y, bool forced, bool bound) noexcept {
		if (is_locked()) return;
		GraphCurve::move_anchor_start(x, y, forced, bound);
		if (!forced) {
			move_handle_left(anchor_start() + handle_left_buffer_, true, true);
			move_handle_right(anchor_end() + handle_right_buffer_, true, true);
		}
	}

	void BezierCurve::move_anchor_end(double x, double y, bool forced, bool bound) noexcept {
		if (is_locked()) return;
		GraphCurve::move_anchor_end(x, y, forced, bound);
		if (!forced) {
			move_handle_left(anchor_start() + handle_left_buffer_, true, true);
			move_handle_right(anchor_end() + handle_right_buffer_, true, true);
		}
	}

	void BezierCurve::begin_move_handle_left(double scale_x, double scale_y) noexcept {
		if (is_locked()) return;
		handle_left_buffer_ = handle_left_;
		auto prev_bezier = prev() ? global::id_manager.get_curve<BezierCurve>(prev()->get_id()) : nullptr;
		if (prev_bezier and global::config.get_align_handle()) {
			scale_x_buffer_ = scale_x;
			scale_y_buffer_ = scale_y;
			auto scaled_prev_offset_x = prev_bezier->handle_right_.x * scale_x;
			auto scaled_prev_offset_y = prev_bezier->handle_right_.y * scale_y;
			length_buffer_ = std::sqrt(scaled_prev_offset_x * scaled_prev_offset_x + scaled_prev_offset_y * scaled_prev_offset_y);
		}
	}

	void BezierCurve::begin_move_handle_right(double scale_x, double scale_y) noexcept {
		if (is_locked()) return;
		handle_right_buffer_ = handle_right_;
		auto next_bezier = next() ? global::id_manager.get_curve<BezierCurve>(next()->get_id()) : nullptr;
		if (next_bezier and global::config.get_align_handle()) {
			scale_x_buffer_ = scale_x;
			scale_y_buffer_ = scale_y;
			auto scaled_next_offset_x = next_bezier->handle_left_.x * scale_x;
			auto scaled_next_offset_y = next_bezier->handle_left_.y * scale_y;
			length_buffer_ = std::sqrt(scaled_next_offset_x * scaled_next_offset_x + scaled_next_offset_y * scaled_next_offset_y);
		}
	}

	void BezierCurve::move_handle_left(double x, double y, bool keep_angle, bool bound) noexcept {
		if (is_locked()) return;
		const auto offset_x = x - anchor_start().x;
		const auto offset_y = y - anchor_start().y;
		const auto ret_x = mkaul::clamp(offset_x, 0., anchor_end().x - anchor_start().x);
		double ret_y;
		if (keep_angle) {
			auto angle = std::atan2(offset_y, offset_x);
			ret_y = ret_x * std::tan(angle);
		}
		else {
			ret_y = offset_y;
		}
		handle_left_ = mkaul::Point{ ret_x, ret_y };
		
		auto prev_bezier = prev() ? global::id_manager.get_curve<BezierCurve>(prev()->get_id()) : nullptr;
		if (!bound and prev_bezier and global::config.get_align_handle()) {
			const auto scaled_angle = std::atan2(-scale_y_buffer_ * ret_y, scale_x_buffer_ * ret_x);
			const auto scaled_offset_x = length_buffer_ * std::cos(scaled_angle + std::numbers::pi);
			const auto scaled_offset_y = -length_buffer_ * std::sin(scaled_angle + std::numbers::pi);
			const auto ret_prev_x = prev_bezier->anchor_end().x + scaled_offset_x / scale_x_buffer_;
			const auto ret_prev_y = prev_bezier->anchor_end().y + scaled_offset_y / scale_y_buffer_;
			prev_bezier->move_handle_right(ret_prev_x, ret_prev_y, true, true);
	}
	}

	void BezierCurve::move_handle_right(double x, double y, bool keep_angle, bool bound) noexcept {
		if (is_locked()) return;
		auto offset_x = x - anchor_end().x;
		auto offset_y = y - anchor_end().y;
		auto ret_x = mkaul::clamp(offset_x, anchor_start().x - anchor_end().x, 0.);
		double ret_y;
		if (keep_angle) {
			auto angle = std::atan2(offset_y, offset_x);
			ret_y = ret_x * std::tan(angle);
		}
		else {
			ret_y = offset_y;
		}
		handle_right_ = mkaul::Point{ ret_x, ret_y };

		auto next_bezier = next() ? global::id_manager.get_curve<BezierCurve>(next()->get_id()) : nullptr;
		if (!bound and next_bezier and global::config.get_align_handle()) {
			const auto scaled_angle = std::atan2(-scale_y_buffer_ * ret_y, scale_x_buffer_ * ret_x);
			const auto scaled_offset_x = length_buffer_ * std::cos(scaled_angle + std::numbers::pi);
			const auto scaled_offset_y = -length_buffer_ * std::sin(scaled_angle + std::numbers::pi);
			const auto ret_next_x = next_bezier->anchor_start().x + scaled_offset_x / scale_x_buffer_;
			const auto ret_next_y = next_bezier->anchor_start().y + scaled_offset_y / scale_y_buffer_;
			next_bezier->move_handle_left(ret_next_x, ret_next_y, true, true);
		}
	}

	void BezierCurve::clear() noexcept {
		auto width = anchor_end().x - anchor_start().x;
		auto height = anchor_end().y - anchor_start().y;
		handle_left_ = {
			DEFAULT_HANDLE_XY * width, DEFAULT_HANDLE_XY * height 
		};
		handle_right_ = {
			- DEFAULT_HANDLE_XY * width, - DEFAULT_HANDLE_XY * height
		};
	}

	bool BezierCurve::is_default() const noexcept {
		auto width = anchor_end().x - anchor_start().x;
		auto height = anchor_end().y - anchor_start().y;

		bool is_handle_left_x_default = mkaul::real_equal(
			handle_left_.x, DEFAULT_HANDLE_XY * width
		);
		bool is_handle_left_y_default = mkaul::real_equal(
			handle_left_.y, DEFAULT_HANDLE_XY * height
		);
		bool is_handle_right_x_default = mkaul::real_equal(
			handle_right_.x, -DEFAULT_HANDLE_XY * width
		);
		bool is_handle_right_y_default = mkaul::real_equal(
			handle_right_.y, -DEFAULT_HANDLE_XY * height
		);

		return is_handle_left_x_default and is_handle_left_y_default and
			is_handle_right_x_default and is_handle_right_y_default;
	}

	void BezierCurve::reverse(bool fix_pt) noexcept {
		auto tmp = handle_left_;
		handle_left_ = -handle_right_;
		handle_right_ = -tmp;
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

		x1 = static_cast<float>(handle_left_.x / width);
		y1 = mkaul::clamp(
			static_cast<float>(handle_left_.y / height),
			MIN_Y, MAX_Y
		);
		x2 = static_cast<float>(1. + handle_right_.x / width);
		y2 = mkaul::clamp(
			static_cast<float>(1. + handle_right_.y / height),
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

		handle_left_ = mkaul::Point{
			width * x1,
			height * y1
		};

		handle_right_ = mkaul::Point{
			width * (x2 - 1.),
			height * (y2 - 1.)
		};

		return true;
	}

	std::string BezierCurve::create_params_str(size_t precision) const noexcept {
		const double width = anchor_end().x - anchor_start().x;
		const double height = anchor_end().y - anchor_start().y;

		auto x1 = handle_left_.x / width;
		auto y1 = handle_left_.y / height;
		auto x2 = handle_right_.x / width + 1.;
		auto y2 = handle_right_.y / height + 1.;

		std::ostringstream oss;
		oss << std::fixed << std::setprecision(precision) << x1;
		oss << ", " << std::fixed << std::setprecision(precision) << y1;
		oss << ", " << std::fixed << std::setprecision(precision) << x2;
		oss << ", " << std::fixed << std::setprecision(precision) << y2;
		return oss.str();
	}

	bool BezierCurve::read_params(const std::vector<double>& params) noexcept {
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
		handle_left_ = mkaul::Point{ x1 * width, y1 * height };
		handle_right_ = mkaul::Point{ (x2 - 1.) * width, (y2 - 1.) * height };
		return true;
	}

	nlohmann::json BezierCurve::create_json() const noexcept {
		auto data = GraphCurve::create_json();
		data["handle_left"] = {
			handle_left_.x, handle_left_.y
		};
		data["handle_right"] = {
			handle_right_.x, handle_right_.y
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
			handle_left_ = mkaul::Point{ left_x, left_y };
			handle_right_ = mkaul::Point{ right_x, right_y };
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}
}