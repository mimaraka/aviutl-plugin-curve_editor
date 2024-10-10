#include "curve_elastic.hpp"
#include "enum.hpp"
#include "util.hpp"



namespace cved {
	// コンストラクタ
	ElasticCurve::ElasticCurve(
		const mkaul::Point<double>& anchor_start,
		const mkaul::Point<double>& anchor_end,
		bool pt_fixed,
		GraphCurve* prev,
		GraphCurve* next,
		double amp,
		double freq,
		double decay,
		bool reversed
	) noexcept : 
		NumericGraphCurve{ anchor_start, anchor_end, pt_fixed, prev, next },
		amp_{ amp },
		freq_{ freq },
		decay_{ decay },
		reversed_{ reversed }
	{}


	// コピーコンストラクタ
	ElasticCurve::ElasticCurve(const ElasticCurve& curve) noexcept :
		NumericGraphCurve{ curve },
		amp_{ curve.amp_ },
		freq_{ curve.freq_ },
		decay_{ curve.decay_ },
		reversed_{ curve.reversed_ }
	{}


	// カーブの値を取得
	double ElasticCurve::curve_function(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - anchor_start().x) / (anchor_end().x - anchor_start().x), 0., 1.);

		if (is_reversed()) {
			progress = 1. - progress;
		}
		double ret;
		const double omega = 2. * std::numbers::pi * freq_;
		const double exp_k = std::exp(-decay_);

		// 減衰振動の関数
		// 始値0, 終値1
		auto func_elastic = [&](double prog) {
			double coef;
			if (decay_ == 0.) {
				coef = 1. - prog;
			}
			else {
				const double tmp = std::exp(-decay_);
				coef = (std::pow(tmp, prog) - tmp) / (1. - tmp);
			}
			return 1. - coef * std::cos(2. * std::numbers::pi * freq_ * prog);
		};

		// 減衰振動の導関数(の定数倍)
		auto func_elastic_derivative = [&](double prog) {
			const double angle = omega * prog;
			const double value_cos = std::cos(angle);
			const double value_sin = std::sin(angle);
			if (decay_ == 0.) {
				return value_cos + omega * (1. - prog) * value_sin;
			}
			else {
				const double exp_kt = std::pow(exp_k, prog);
				return (decay_ * exp_kt * value_cos + omega * (exp_kt - exp_k) * value_sin);
			}
		};

		// 減衰振動の2階導関数(の定数倍)
		auto func_elastic_derivative_2 = [&](double prog) {
			const double angle = omega * prog;
			const double value_cos = std::cos(angle);
			const double value_sin = std::sin(angle);
			if (decay_ == 0.) {
				return omega * omega * (1. - prog) * value_cos - 2. * omega * value_sin;
			}
			else {
				const double exp_kt = std::pow(exp_k, prog);
				const double omega_sq = omega * omega;
				return (((omega_sq - decay_ * decay_) * exp_kt - omega_sq * exp_k) * value_cos - 2. * omega * decay_ * exp_kt * value_sin);
			}
		};

		// ニュートン法による極値をとるtの近似
		double extremum_t = 0.5 / freq_;
		for (int i = 0; i < 4; i++) {
			extremum_t -= func_elastic_derivative(extremum_t) / func_elastic_derivative_2(extremum_t);
		}
		const double extremum_x = func_elastic(extremum_t);
		const double tmp = func_elastic(progress);
		if (progress < extremum_t) {
			ret = (amp_ * (extremum_x - 1.) + 1.) / extremum_x * tmp;
		}
		else {
			ret = amp_ * (tmp - 1.) + 1.;
		}
		if (is_reversed()) {
			ret = 1. - ret;
		}
		return start + (end - start) * (anchor_start().y + (anchor_end().y - anchor_start().y) * ret);
	}

	// カーブを初期化
	void ElasticCurve::clear() noexcept {
		amp_ = DEFAULT_AMP;
		freq_ = DEFAULT_FREQ;
		decay_ = DEFAULT_DECAY;
	}

	// カーブがデフォルトかどうか
	bool ElasticCurve::is_default() const noexcept {
		return amp_ == DEFAULT_AMP and freq_ == DEFAULT_FREQ and decay_ == DEFAULT_DECAY;
	}

	// カーブを反転
	void ElasticCurve::reverse(bool fix_pt) noexcept {
		GraphCurve::reverse(fix_pt);
		reversed_ = !reversed_;
	}

	double ElasticCurve::get_handle_amp_left_y() const noexcept {
		return std::max(anchor_start().y, anchor_end().y) + amp_ * std::abs(anchor_start().y - anchor_end().y);
	}

	double ElasticCurve::get_handle_freq_decay_x() const noexcept {
		const auto width = anchor_end().x - anchor_start().x;
		double value;
		if (is_reversed()) {
			value = 1. - 0.5 / freq_;
		}
		else {
			value = 0.5 / freq_;
		}
		return anchor_start().x + width * value;
	}

	double ElasticCurve::get_handle_freq_decay_y() const noexcept {
		const auto height_abs = std::abs(anchor_end().y - anchor_start().y);
		double anchor_y;
		if (is_reversed()) {
			anchor_y = anchor_start().y;
		}
		else {
			anchor_y = anchor_end().y;
		}
		return anchor_y - std::exp(-(decay_ - 1.) * 0.1) * height_abs;
	}

	double ElasticCurve::get_handle_freq_decay_root_y() const noexcept {
		if (is_reversed()) {
			return anchor_start().y;
		}
		else {
			return anchor_end().y;
		}
	}
	

	void ElasticCurve::set_handle_amp_left(double y) noexcept {
		const auto height_abs = std::abs(anchor_end().y - anchor_start().y);
		amp_ = mkaul::clamp(
			(y - std::max(anchor_start().y, anchor_end().y)) / height_abs,
			0., 1.
		);
	}

	void ElasticCurve::set_handle_freq_decay(double x, double y) noexcept {
		const auto width = anchor_end().x - anchor_start().x;
		const auto height_abs = std::abs(anchor_end().y - anchor_start().y);
		double value_freq;
		double anchor_y;
		if (is_reversed()) {
			value_freq = anchor_end().x - x;
			anchor_y = anchor_start().y;
		}
		else {
			value_freq = x - anchor_start().x;
			anchor_y = anchor_end().y;
		}
		double value_decay = y - anchor_y + height_abs;
		freq_ = std::max(0.5 / std::max(value_freq / width, 0.0001), 0.5);
		decay_ = -10. * std::log(-mkaul::clamp(value_decay / height_abs, 0., 0.9999) + 1.) + 1.;
	}

	// カーブのコードを生成
	int32_t ElasticCurve::encode() const noexcept {
		const int f = (int)(500 / freq_);
		const int a = (int)(100 * amp_);
		const int k = -(int)(100 * (std::exp(-(decay_ - 1.) * .1) - 1.));
		int ret = 1 + a + k * 101 + f * 101 * 101;
		if (is_reversed()) {
			ret *= -1;
		}
		return ret;
	}

	bool ElasticCurve::decode(int32_t code) noexcept {
		// -2147483647 ~  -11213203 : Unused
		//   -11213202 ~  -10211202 : Bounce
		//   -10211201 ~         -1 : Elastic
		//           0              : Unused
		//           1 ~   10211201 : Elastic
		//    10211202 ~   11213202 : Bounce
		//    11213203 ~ 2147483647 : Unused

		int number;

		if (mkaul::in_range(code, -10211201, -1, true)) {
			reversed_ = true;
			number = -code - 1;
		}
		else if (mkaul::in_range(code, 1, 10211201, true)) {
			reversed_ = false;
			number = code - 1;
		}
		else return false;

		const int f = number / (101 * 101);
		const int k = (number - f * 101 * 101) / 101;
		const int a = number - f * 101 * 101 - k * 101;
		freq_ = 0.5 / std::max(f * 0.001, 0.001);
		decay_ = -10. * std::log(-k * 0.01 + 1.) + 1.;
		amp_ = std::clamp(a, 0, 100) * 0.01;

		return true;
	}
}