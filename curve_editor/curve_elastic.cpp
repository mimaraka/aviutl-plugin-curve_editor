#include "curve_elastic.hpp"
#include "curve_data.hpp"
#include "enum.hpp"
#include "util.hpp"



namespace cved {
	// コンストラクタ
	ElasticCurve::ElasticCurve(
		const mkaul::Point<double>& pt_start,
		const mkaul::Point<double>& pt_end,
		uint32_t sampling_resolution,
		uint32_t quantization_resolution,
		bool pt_fixed,
		GraphCurve* prev,
		GraphCurve* next,
		double amp,
		double freq,
		double decay
	) noexcept : 
		NumericGraphCurve{ pt_start, pt_end, sampling_resolution, quantization_resolution, pt_fixed, prev, next },
		handle_amp_{},
		handle_freq_decay_{},
		amp_{ amp },
		freq_{ freq },
		decay_{ decay }
	{
		handle_amp_.from_param(amp, pt_start, pt_end);
		handle_freq_decay_.from_param(freq, decay, pt_start, pt_end);
	}


	// コピーコンストラクタ
	ElasticCurve::ElasticCurve(const ElasticCurve& curve) noexcept :
		NumericGraphCurve{ curve },
		handle_amp_{ curve.handle_amp_ },
		handle_freq_decay_{ curve.handle_freq_decay_ },
		amp_{ curve.amp_ },
		freq_{ curve.freq_ },
		decay_{ curve.decay_ }
	{}


	// カーブの値を取得
	double ElasticCurve::curve_function(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - pt_start().x()) / (pt_end().x() - pt_start().x()), 0., 1.);

		if (handle_freq_decay_.is_reverse()) {
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
				double tmp = std::exp(-decay_);
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
		double extremum_x = func_elastic(extremum_t);
		double tmp = func_elastic(progress);
		if (progress < extremum_t) {
			ret = (amp_ * (extremum_x - 1.) + 1.) / extremum_x * tmp;
		}
		else {
			ret = amp_ * (tmp - 1.) + 1.;
		}
		if (handle_freq_decay_.is_reverse()) {
			ret = 1. - ret;
		}
		return start + (end - start) * (pt_start().y() + (pt_end().y() - pt_start().y()) * ret);
	}

	// カーブを初期化
	void ElasticCurve::clear() noexcept {
		handle_amp_.from_param(DEFAULT_AMP, pt_start(), pt_end());
		handle_freq_decay_.from_param(DEFAULT_FREQ, DEFAULT_DECAY, pt_start(), pt_end());
		amp_ = DEFAULT_AMP;
		freq_ = DEFAULT_FREQ;
		decay_ = DEFAULT_DECAY;
	}

	// カーブを反転
	void ElasticCurve::reverse(bool fix_pt) noexcept {
		handle_freq_decay_.reverse(pt_start(), pt_end());
		GraphCurve::reverse(fix_pt);
		handle_amp_.from_param(amp_, pt_start(), pt_end());
		handle_freq_decay_.from_param(freq_, decay_, pt_start(), pt_end());
	}

	void ElasticCurve::create_data(std::vector<byte>& data) const noexcept {
		ElasticCurveData data_elastic{
			.data_graph = GraphCurveData{
				.start_x = pt_start().x(),
				.start_y = pt_start().y(),
				.end_x = pt_end().x(),
				.end_y = pt_end().y(),
				.sampling_resolution = get_sampling_resolution(),
				.quantization_resolution = get_quantization_resolution()
			},
			.amp = amp_,
			.freq = freq_,
			.decay = decay_
		};
		auto bytes_elastic = reinterpret_cast<byte*>(&data_elastic);
		size_t n = sizeof(ElasticCurveData) / sizeof(byte);
		data = std::vector<byte>{ bytes_elastic, bytes_elastic + n };
		data.insert(data.begin(), (byte)CurveSegmentType::Elastic);
	}

	bool ElasticCurve::load_data(const byte* data, size_t size) noexcept {
		if (size < sizeof(ElasticCurveData) / sizeof(byte)) return false;
		auto p_curve_data = reinterpret_cast<const ElasticCurveData*>(data);
		// カーブの整合性チェック
		if (
			!mkaul::real_in_range(p_curve_data->data_graph.start_x, 0., 1., true)
			or !mkaul::real_in_range(p_curve_data->data_graph.end_x, 0., 1., true)
			or p_curve_data->data_graph.end_x < p_curve_data->data_graph.start_x
			) {
			return false;
		}
		pt_start_.move(mkaul::Point{ p_curve_data->data_graph.start_x, p_curve_data->data_graph.start_y });
		pt_end_.move(mkaul::Point{ p_curve_data->data_graph.end_x, p_curve_data->data_graph.end_y });
		handle_amp_.from_param(p_curve_data->amp, pt_start(), pt_end());
		handle_freq_decay_.from_param(p_curve_data->freq, p_curve_data->decay, pt_start(), pt_end());
		amp_ = handle_amp_.get_amp(pt_start(), pt_end());
		freq_ = handle_freq_decay_.get_freq(pt_start(), pt_end());
		decay_ = handle_freq_decay_.get_decay(pt_start(), pt_end());
		set_sampling_resolution(p_curve_data->data_graph.sampling_resolution);
		set_quantization_resolution(p_curve_data->data_graph.quantization_resolution);
		return true;
	}

	// カーブのコードを生成
	int32_t ElasticCurve::encode() const noexcept {
		int ret;
		int f = (int)(500 / freq_);
		int a = (int)(100 * amp_);
		int k = -(int)(100 * (std::exp(-(decay_ - 1.) * .1) - 1.));
		ret = 1 + a + k * 101 + f * 101 * 101;
		if (handle_freq_decay_.is_reverse()) {
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
			handle_freq_decay_.set_is_reverse(true, pt_start(), pt_end());
			number = -code - 1;
		}
		else if (mkaul::in_range(code, 1, 10211201, true)) {
			handle_freq_decay_.set_is_reverse(false, pt_start(), pt_end());
			number = code - 1;
		}
		else return false;

		int f, k, a;

		f = number / (101 * 101);
		k = (number - f * 101 * 101) / 101;
		a = number - f * 101 * 101 - k * 101;
		freq_ = 0.5 / std::max(f * 0.001, 0.001);
		decay_ = -10. * std::log(-k * 0.01 + 1.) + 1.;
		amp_ = std::clamp(a, 0, 100) * 0.01;

		handle_amp_.from_param(amp_, pt_start(), pt_end());
		handle_freq_decay_.from_param(freq_, decay_, pt_start(), pt_end());

		return true;
	}

	// ハンドルを描画
	void ElasticCurve::draw_handle(
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

			auto amp_left_client = view.view_to_client(handle_amp_.pt_left(), rect_wnd);
			auto amp_right_client = view.view_to_client(handle_amp_.pt_right(), rect_wnd);
			auto tmp_amp_left = amp_left_client;
			auto tmp_amp_right = amp_right_client;

			p_graphics->draw_ellipse(
				amp_left_client, tip_radius, tip_radius, color, mkaul::graphics::Stroke{ tip_thickness }
			);
			p_graphics->draw_ellipse(
				amp_right_client, tip_radius, tip_radius, color, mkaul::graphics::Stroke{ tip_thickness }
			);

			if (cutoff_line) {
				util::get_cutoff_line(&tmp_amp_left, amp_right_client, amp_left_client, tip_radius + 4.f);
				util::get_cutoff_line(&tmp_amp_right, amp_left_client, amp_right_client, tip_radius + 4.f);
			}

			p_graphics->draw_line(
				tmp_amp_left, tmp_amp_right, color, mkaul::graphics::Stroke{ thickness }
			);

			// FreqDecayHandleの描画
			auto freq_decay_client_root = view.view_to_client(mkaul::Point{ handle_freq_decay_.pt().x, pt_end().y() }, rect_wnd);
			auto freq_decay_client_tip = view.view_to_client(handle_freq_decay_.pt(), rect_wnd);
			auto tmp_freq_decay_root = freq_decay_client_root;
			auto tmp_freq_decay_tip = freq_decay_client_tip;

			if (cutoff_line) {
				util::get_cutoff_line(&tmp_freq_decay_root, freq_decay_client_tip, freq_decay_client_root, root_radius + 4.f);
				util::get_cutoff_line(&tmp_freq_decay_tip, freq_decay_client_root, freq_decay_client_tip, tip_radius + 4.f);
			}

			p_graphics->draw_ellipse(
				freq_decay_client_tip, tip_radius, tip_radius, color, mkaul::graphics::Stroke{ tip_thickness }
			);
			p_graphics->fill_ellipse(
				freq_decay_client_root, root_radius, root_radius, color
			);
			p_graphics->draw_line(
				tmp_freq_decay_root, tmp_freq_decay_tip, color, mkaul::graphics::Stroke{ tip_thickness }
			);
		}
	}

	// カーソルがハンドルにホバーしているかどうか
	bool ElasticCurve::is_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		return handle_amp_.is_hovered(pt, view) or handle_freq_decay_.is_hovered(pt, view);
	}

	// カーソルがハンドルにホバーしているかをチェックし、操作を開始
	bool ElasticCurve::handle_check_hover(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept {
		return handle_amp_.check_hover(pt, view) or handle_freq_decay_.check_hover(pt, view);
	}

	// ハンドルの位置をアップデート
	bool ElasticCurve::handle_update(const mkaul::Point<double>& pt, const GraphView&) noexcept {
		if (handle_amp_.update(pt, pt_start(), pt_end()) or handle_freq_decay_.update(pt, pt_start(), pt_end())) {
			amp_ = handle_amp_.get_amp(pt_start(), pt_end());
			freq_ = handle_freq_decay_.get_freq(pt_start(), pt_end());
			decay_ = handle_freq_decay_.get_decay(pt_start(), pt_end());
			return true;
		}
		else return false;
	}

	// ハンドルの操作を終了
	void ElasticCurve::handle_end_control() noexcept {
		handle_amp_.end_control();
		handle_freq_decay_.end_control();
	}

	// カーソルがポイントにホバーしているかをチェックし、操作を開始
	ElasticCurve::ActivePoint ElasticCurve::pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		bool start = pt_start_.check_hover(pt, view);
		bool end = pt_end_.check_hover(pt, view);
		
		if (start or end) {
			amp_ = handle_amp_.get_amp(pt_start(), pt_end());
			freq_ = handle_freq_decay_.get_freq(pt_start(), pt_end());
			decay_ = handle_freq_decay_.get_decay(pt_start(), pt_end());
			if (start) return ActivePoint::Start;
			else return ActivePoint::End;
		}
		else return ActivePoint::Null;
	}

	// ポイントの移動を開始
	bool ElasticCurve::pt_begin_move(ActivePoint active_pt) noexcept {
		amp_ = handle_amp_.get_amp(pt_start(), pt_end());
		freq_ = handle_freq_decay_.get_freq(pt_start(), pt_end());
		decay_ = handle_freq_decay_.get_decay(pt_start(), pt_end());
		return true;
	}

	// ポイントを位置をアップデート
	ElasticCurve::ActivePoint ElasticCurve::pt_update(const mkaul::Point<double>& pt, const GraphView&) noexcept {
		bool moved_start = pt_start_.update(mkaul::Point{ std::min(pt.x, pt_end().x()), pt.y });
		bool moved_end = pt_end_.update(mkaul::Point{ std::max(pt.x, pt_start().x()), pt.y });

		if (moved_start or moved_end) {
			handle_amp_.from_param(amp_, pt_start(), pt_end());
			handle_freq_decay_.from_param(freq_, decay_, pt_start(), pt_end());
			if (moved_start) return ActivePoint::Start;
			else return ActivePoint::End;
		}
		else return ActivePoint::Null;
	}

	// ポイントを強制的に動かす
	bool ElasticCurve::pt_move(ActivePoint active_pt, const mkaul::Point<double>& pt) noexcept {
		if (GraphCurve::pt_move(active_pt, pt)) {
			handle_amp_.from_param(amp_, pt_start(), pt_end());
			handle_freq_decay_.from_param(freq_, decay_, pt_start(), pt_end());
			return true;
		}
		else return false;
	}
}