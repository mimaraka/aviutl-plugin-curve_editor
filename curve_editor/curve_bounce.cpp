#include "curve_bounce.hpp"
#include "enum.hpp"



namespace cved {
	// コンストラクタ
	BounceCurve::BounceCurve(
		const mkaul::Point<double>& anchor_start,
		const mkaul::Point<double>& anchor_end,
		bool pt_fixed,
		GraphCurve* prev,
		GraphCurve* next,
		double cor,
		double period,
		bool reversed
	) :
		NumericGraphCurve{ anchor_start, anchor_end, pt_fixed, prev, next },
		cor_{ cor },
		period_{ period },
		reversed_{ reversed }
	{}
	
	// コピーコンストラクタ
	BounceCurve::BounceCurve(const BounceCurve& curve) noexcept :
		NumericGraphCurve{ curve },
		cor_{ curve.cor_ },
		period_{ curve.period_ },
		reversed_{ curve.reversed_ }
	{}

	// カーブの関数
	double BounceCurve::curve_function(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - anchor_start().x) / (anchor_end().x - anchor_start().x), 0., 1.);

		double ret;
		const double limit_value = period_ * (1. / (1. - cor_) - .5);

		auto func_1 = [](double prog, double cor) {
			return std::floor(std::log((cor - 1.) * prog + 1.) / std::log(cor));
		};
		auto func_2 = [&](double prog, double cor) {
			return prog + 0.5 + 1. / (cor - 1.) - (cor + 1.) * std::pow(cor, func_1(prog + .5, cor)) / (2. * cor - 2.);
		};

		if (is_reversed()) {
			progress = 1. - progress;
		}
		double tmp = 4. * func_2(progress / period_, cor_) * func_2(progress / period_, cor_) - std::pow(cor_, 2. * func_1(progress / period_ + .5, cor_));
		
		if (limit_value > 1.) {
			int n = (int)(std::log(1. + (cor_ - 1.) * (1 / period_ + .5)) / std::log(cor_));
			if (progress < period_ * ((std::pow(cor_, n) - 1.) / (cor_ - 1.) - .5)) {
				ret = tmp;
			}
			else {
				ret = 0.;
			}
		}
		else {
			if (progress < limit_value)
				ret = tmp;
			else
				ret = 0;
		}
		if (is_reversed()) {
			ret = -1. - ret;
		}
		return start + (end - start) * (anchor_end().y + (anchor_end().y - anchor_start().y) * ret);
	}

	void BounceCurve::clear() noexcept {
		cor_ = DEFAULT_COR;
		period_ = DEFAULT_PERIOD;
	}

	bool BounceCurve::is_default() const noexcept {
		return cor_ == DEFAULT_COR and period_ == DEFAULT_PERIOD;
	}

	void BounceCurve::reverse(bool fix_pt) noexcept {
		if (is_locked()) return;
		GraphCurve::reverse(fix_pt);
		reversed_ = !reversed_;
	}

	double BounceCurve::get_handle_x() const noexcept {
		const auto width = anchor_end().x - anchor_start().x;
		auto x_rel = mkaul::clamp(period_ * (cor_ + 1.) * 0.5, 0., 1.);
		if (is_reversed()) {
			x_rel = 1. - x_rel;
		}
		return anchor_start().x + width * x_rel;
	}

	double BounceCurve::get_handle_y() const noexcept {
		const auto height = anchor_end().y - anchor_start().y;
		double y_rel = mkaul::clamp(1. - cor_ * cor_, 0., 1.);
		if (is_reversed()) {
			y_rel = 1. - y_rel;
		}
		return anchor_start().y + height * y_rel;
	}

	void BounceCurve::set_handle(double x, double y) noexcept {
		if (is_locked()) return;
		const auto width = anchor_end().x - anchor_start().x;
		const auto height = anchor_end().y - anchor_start().y;
		auto x_rel = mkaul::clamp((x - anchor_start().x) / width, 0., 1.);
		auto y_rel = mkaul::clamp((y - anchor_start().y) / height, 0., 1.);
		if (is_reversed()) {
			x_rel = 1. - x_rel;
			y_rel = 1. - y_rel;
		}
		cor_ = std::sqrt(1. - mkaul::clamp(y_rel, 0.001, 1.));
		period_ = 2. * std::clamp(x_rel, 0.001, 1.) / (cor_ + 1.);
	}

	int32_t BounceCurve::encode() const noexcept {
		int x = (int)((period_ * (cor_ + 1) * 0.5) * 1000);
		int y = (int)((1. - cor_ * cor_) * 1000);
		int ret = y + 1001 * x + 10211202;
		if (is_reversed()) {
			ret *= -1;
		}
		return ret;
	}

	bool BounceCurve::decode(int32_t code) noexcept {
		// -2147483647 ~  -11213203 : Unused
		//   -11213202 ~  -10211202 : Bounce
		//   -10211201 ~         -1 : Elastic
		//           0              : Unused
		//           1 ~   10211201 : Elastic
		//    10211202 ~   11213202 : Bounce
		//    11213203 ~ 2147483647 : Unused
		if (is_locked()) return false;

		constexpr int MIN = 10211202;
		constexpr int MAX = 11213202;
		int number;

		if (mkaul::in_range(code, -MAX, -MIN)) {
			reversed_ = true;
			number = -code - MIN;
		}
		else if (mkaul::in_range(code, MIN, MAX)) {
			reversed_ = false;
			number = code - MIN;
		}
		else return false;

		int x, y;

		x = number / 1001;
		y = number - x * 1001;
		cor_ = std::sqrt(1. - y * 0.001);
		period_ = 2. * 0.001 * x / (cor_ + 1.);
		return true;
	}

	std::string BounceCurve::create_params_str(size_t precision) const noexcept {
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(precision) << cor_;
		oss << ", " << std::fixed << std::setprecision(precision) << period_;
		return oss.str();
	}

	bool BounceCurve::read_params(const std::vector<double>& params) noexcept {
		if (is_locked()) return true;
		if (params.size() != 2) {
			return false;
		}
		if (!mkaul::real_in_range(params[0], 0., 1.) or !mkaul::real_in_range(params[1], 0., 2.)) {
			return false;
		}
		cor_ = params[0];
		period_ = params[1];
		return true;
	}

	nlohmann::json BounceCurve::create_json() const noexcept {
		auto data = GraphCurve::create_json();
		data["cor"] = cor_;
		data["period"] = period_;
		data["reversed"] = reversed_;
		return data;
	}

	bool BounceCurve::load_json(const nlohmann::json& data) noexcept {
		if (!GraphCurve::load_json(data)) {
			return false;
		}
		try {
			auto cor = data.at("cor").get<double>();
			auto period = data.at("period").get<double>();
			if (!mkaul::real_in_range(cor, 0., 1.) or !mkaul::real_in_range(period, 0., 2.)) {
				return false;
			}
			cor_ = cor;
			period_ = period;
			reversed_ = data.at("reversed").get<bool>();
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
		return true;
	}
} // namespace cved