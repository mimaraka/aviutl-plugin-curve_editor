#include "curve_elastic.hpp"
#include "curve_data.hpp"
#include "enum.hpp"
#include "util.hpp"



namespace cved {
	// コンストラクタ
	ElasticCurve::ElasticCurve(
		const mkaul::Point<double>& point_start,
		const mkaul::Point<double>& point_end,
		bool point_fixed,
		GraphCurve* prev,
		GraphCurve* next,
		double amp,
		double freq,
		double decay
	) : 
		NumericGraphCurve{ point_start, point_end, point_fixed, prev, next },
		handle_amp_{},
		handle_freq_decay_{},
		amp_{ amp },
		freq_{ freq },
		decay_{ decay }
	{
		handle_amp_.from_param(amp, point_start, point_end);
		handle_freq_decay_.from_param(freq, decay, point_start, point_end);
	}

	// カーブの値を取得
	double ElasticCurve::get_value(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - point_start_.x()) / (point_end_.x() - point_start_.x()), 0., 1.);

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
		return start + (end - start) * (point_start_.y() + (point_end_.y() - point_start_.y()) * ret);
	}

	// カーブを初期化
	void ElasticCurve::clear() noexcept {
		handle_amp_.from_param(DEFAULT_AMP, point_start_, point_end_);
		handle_freq_decay_.from_param(DEFAULT_FREQ, DEFAULT_DECAY, point_start_, point_end_);
		amp_ = DEFAULT_AMP;
		freq_ = DEFAULT_FREQ;
		decay_ = DEFAULT_DECAY;
	}

	// カーブを反転
	void ElasticCurve::reverse() noexcept {
		handle_freq_decay_.reverse(point_start_, point_end_);
		GraphCurve::reverse();
		handle_amp_.from_param(amp_, point_start_, point_end_);
		handle_freq_decay_.from_param(freq_, decay_, point_start_, point_end_);
	}

	void ElasticCurve::create_data(std::vector<byte>& data) const noexcept {
		ElasticCurveData data_elastic{
			.data_graph = GraphCurveData{
				.start_x = point_start_.x(),
				.start_y = point_start_.y(),
				.end_x = point_end_.x(),
				.end_y = point_end_.y()
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
		point_start_.move(mkaul::Point{ p_curve_data->data_graph.start_x, p_curve_data->data_graph.start_y });
		point_end_.move(mkaul::Point{ p_curve_data->data_graph.end_x, p_curve_data->data_graph.end_y });
		handle_amp_.from_param(p_curve_data->amp, point_start_, point_end_);
		handle_freq_decay_.from_param(p_curve_data->freq, p_curve_data->decay, point_start_, point_end_);
		amp_ = handle_amp_.get_amp(point_start_, point_end_);
		freq_ = handle_freq_decay_.get_freq(point_start_, point_end_);
		decay_ = handle_freq_decay_.get_decay(point_start_, point_end_);
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
			handle_freq_decay_.set_is_reverse(true, point_start_, point_end_);
			number = -code - 1;
		}
		else if (mkaul::in_range(code, 1, 10211201, true)) {
			handle_freq_decay_.set_is_reverse(false, point_start_, point_end_);
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

		handle_amp_.from_param(amp_, point_start_, point_end_);
		handle_freq_decay_.from_param(freq_, decay_, point_start_, point_end_);

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
		mkaul::WindowRectangle rect_wnd;
		p_graphics->get_rect(&rect_wnd);

		auto amp_left_client = view.view_to_client(handle_amp_.point_left().point(), rect_wnd);
		auto amp_right_client = view.view_to_client(handle_amp_.point_right().point(), rect_wnd);
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
		auto freq_decay_client_root = view.view_to_client(mkaul::Point{ handle_freq_decay_.point().x(), point_end_.y() }, rect_wnd);
		auto freq_decay_client_tip = view.view_to_client(handle_freq_decay_.point().point(), rect_wnd);
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

	// カーソルがハンドルにホバーしているかどうか
	bool ElasticCurve::is_handle_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept {
		return handle_amp_.is_hovered(point, box_width, view) or handle_freq_decay_.is_hovered(point, box_width, view);
	}

	// カーソルがハンドルにホバーしているかをチェックし、操作を開始
	bool ElasticCurve::handle_check_hover(
		const mkaul::Point<double>& point,
		float box_width,
		const GraphView& view
	) noexcept {
		return handle_amp_.check_hover(point, box_width, view) or handle_freq_decay_.check_hover(point, box_width, view);
	}

	// ハンドルの位置をアップデート
	bool ElasticCurve::handle_update(const mkaul::Point<double>& point, const GraphView&) noexcept {
		if (handle_amp_.update(point, point_start_, point_end_) or handle_freq_decay_.update(point, point_start_, point_end_)) {
			amp_ = handle_amp_.get_amp(point_start_, point_end_);
			freq_ = handle_freq_decay_.get_freq(point_start_, point_end_);
			decay_ = handle_freq_decay_.get_decay(point_start_, point_end_);
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
	ElasticCurve::ActivePoint ElasticCurve::point_check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept {
		bool start = point_start_.check_hover(point, box_width, view);
		bool end = point_end_.check_hover(point, box_width, view);
		
		if (start or end) {
			amp_ = handle_amp_.get_amp(point_start_, point_end_);
			freq_ = handle_freq_decay_.get_freq(point_start_, point_end_);
			decay_ = handle_freq_decay_.get_decay(point_start_, point_end_);
			if (start) return ActivePoint::Start;
			else return ActivePoint::End;
		}
		else return ActivePoint::Null;
	}

	// ポイントの移動を開始
	bool ElasticCurve::point_begin_move(ActivePoint active_point) noexcept {
		amp_ = handle_amp_.get_amp(point_start_, point_end_);
		freq_ = handle_freq_decay_.get_freq(point_start_, point_end_);
		decay_ = handle_freq_decay_.get_decay(point_start_, point_end_);
		return true;
	}

	// ポイントを位置をアップデート
	ElasticCurve::ActivePoint ElasticCurve::point_update(const mkaul::Point<double>& point, const GraphView&) noexcept {
		bool moved_start = point_start_.update(mkaul::Point{ std::min(point.x, point_end_.x()), point.y });
		bool moved_end = point_end_.update(mkaul::Point{ std::max(point.x, point_start_.x()), point.y });

		if (moved_start or moved_end) {
			handle_amp_.from_param(amp_, point_start_, point_end_);
			handle_freq_decay_.from_param(freq_, decay_, point_start_, point_end_);
			if (moved_start) return ActivePoint::Start;
			else return ActivePoint::End;
		}
		else return ActivePoint::Null;
	}

	// ポイントを強制的に動かす
	bool ElasticCurve::point_move(ActivePoint active_point, const mkaul::Point<double>& point) noexcept {
		if (GraphCurve::point_move(active_point, point)) {
			handle_amp_.from_param(amp_, point_start_, point_end_);
			handle_freq_decay_.from_param(freq_, decay_, point_start_, point_end_);
			return true;
		}
		else return false;
	}
}