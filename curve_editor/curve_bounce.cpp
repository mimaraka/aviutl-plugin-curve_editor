#include "curve_bounce.hpp"
#include "curve_data.hpp"
#include "enum.hpp"



namespace cved {
	// コンストラクタ
	BounceCurve::BounceCurve(
		const mkaul::Point<double>& pt_start,
		const mkaul::Point<double>& pt_end,
		uint32_t sampling_resolution,
		uint32_t quantization_resolution,
		bool pt_fixed,
		GraphCurve* prev,
		GraphCurve* next,
		double cor,
		double period
	) : NumericGraphCurve{ pt_start, pt_end, sampling_resolution, quantization_resolution, pt_fixed, prev, next}, handle_{}, cor_{ cor }, period_{ period } {
		handle_.from_param(cor, period, pt_start, pt_end);
	}

	// コピーコンストラクタ
	BounceCurve::BounceCurve(const BounceCurve& curve) noexcept : NumericGraphCurve{ curve }, handle_{ curve.handle_ }, cor_{ curve.cor_ }, period_{ curve.period_ } {}

	// カーブの関数
	double BounceCurve::curve_function(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - pt_start().x()) / (pt_end().x() - pt_start().x()), 0., 1.);

		double ret;
		const double limit_value = period_ * (1. / (1. - cor_) - .5);

		auto func_1 = [](double prog, double cor) {
			return std::floor(std::log((cor - 1.) * prog + 1.) / std::log(cor));
		};
		auto func_2 = [&](double prog, double cor) {
			return prog + 0.5 + 1. / (cor - 1.) - (cor + 1.) * std::pow(cor, func_1(prog + .5, cor)) / (2. * cor - 2.);
		};

		if (handle_.is_reverse()) {
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
		if (handle_.is_reverse()) {
			ret = -1. - ret;
		}
		return start + (end - start) * (pt_end().y() + (pt_end().y() - pt_start().y()) * ret);
	}

	void BounceCurve::clear() noexcept {
		handle_.from_param(DEFAULT_COR, DEFAULT_PERIOD, pt_start(), pt_end());
		cor_ = DEFAULT_COR;
		period_ = DEFAULT_PERIOD;
	}

	void BounceCurve::reverse(bool fix_pt) noexcept {
		handle_.reverse(pt_start(), pt_end());
		GraphCurve::reverse(fix_pt);
		handle_.from_param(cor_, period_, pt_start(), pt_end());
	}

	void BounceCurve::create_data(std::vector<byte>& data) const noexcept {
		BounceCurveData data_bounce{
			.data_graph = GraphCurveData{
				.start_x = pt_start().x(),
				.start_y = pt_start().y(),
				.end_x = pt_end().x(),
				.end_y = pt_end().y(),
				.sampling_resolution = get_sampling_resolution(),
				.quantization_resolution = get_quantization_resolution()
			},
			.cor = cor_,
			.period = period_
		};
		auto bytes_bounce = reinterpret_cast<byte*>(&data_bounce);
		size_t n = sizeof(BounceCurveData) / sizeof(byte);
		data = std::vector<byte>{ bytes_bounce, bytes_bounce + n };
		data.insert(data.begin(), (byte)CurveSegmentType::Bounce);
	}

	bool BounceCurve::load_data(const byte* data, size_t size) noexcept {
		if (size < sizeof(BounceCurveData) / sizeof(byte)) return false;
		auto p_curve_data = reinterpret_cast<const BounceCurveData*>(data);
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
		handle_.from_param(p_curve_data->cor, p_curve_data->period, pt_start(), pt_end());
		cor_ = handle_.get_cor(pt_start(), pt_end());
		period_ = handle_.get_period(pt_start(), pt_end());
		set_sampling_resolution(p_curve_data->data_graph.sampling_resolution);
		set_quantization_resolution(p_curve_data->data_graph.quantization_resolution);
		return true;
	}

	int32_t BounceCurve::encode() const noexcept {
		int x = (int)((period_ * (cor_ + 1) * 0.5) * 1000);
		int y = (int)((1. - cor_ * cor_) * 1000);
		int ret = y + 1001 * x + 10211202;
		if (handle_.is_reverse()) {
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

		constexpr int MIN = 10211202;
		constexpr int MAX = 11213202;
		int number;

		if (mkaul::in_range(code, -MAX, -MIN, true)) {
			handle_.set_is_reverse(true, pt_start(), pt_end());
			number = -code - MIN;
		}
		else if (mkaul::in_range(code, MIN, MAX, true)) {
			handle_.set_is_reverse(false, pt_start(), pt_end());
			number = code - MIN;
		}
		else return false;

		int x, y;

		x = number / 1001;
		y = number - x * 1001;
		cor_ = std::sqrt(1. - y * 0.001);
		period_ = 2. * 0.001 * x / (cor_ + 1.);

		handle_.from_param(cor_, period_, pt_start(), pt_end());

		return true;
	}

	// ハンドルを描画
	void BounceCurve::draw_handle(
		mkaul::graphics::Graphics* p_graphics,
		const View& view,
		float, float,
		float tip_radius,
		float tip_thickness,
		bool,
		const mkaul::ColorF& color
	) const noexcept {
		if (p_graphics) {
			mkaul::WindowRectangle rect_wnd;
			p_graphics->get_rect(&rect_wnd);

			auto handle_client = view.view_to_client(handle_.pt(), rect_wnd);
			p_graphics->draw_ellipse(handle_client, tip_radius, tip_radius, color, mkaul::graphics::Stroke{ tip_thickness });
		}
	}

	bool BounceCurve::is_handle_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		return handle_.is_hovered(pt, view);
	}

	bool BounceCurve::handle_check_hover(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept {
		return handle_.check_hover(pt, view);
	}

	bool BounceCurve::handle_update(
		const mkaul::Point<double>& pt,
		const GraphView& view
	) noexcept {
		if (handle_.update(pt, pt_start(), pt_end())) {
			cor_ = handle_.get_cor(pt_start(), pt_end());
			period_ = handle_.get_period(pt_start(), pt_end());
			return true;
		}
		else return false;
	}

	void BounceCurve::handle_end_control() noexcept {
		handle_.end_control();
	}

	BounceCurve::ActivePoint BounceCurve::pt_check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		bool start = pt_start_.check_hover(pt, view);
		bool end = pt_end_.check_hover(pt, view);
		
		if (start or end) {
			cor_ = handle_.get_cor(pt_start(), pt_end());
			period_ = handle_.get_period(pt_start(), pt_end());

			if (start) return ActivePoint::Start;
			else return ActivePoint::End;
		}
		else return ActivePoint::Null;
	}

	bool BounceCurve::pt_begin_move(ActivePoint active_pt) noexcept {
		cor_ = handle_.get_cor(pt_start(), pt_end());
		period_ = handle_.get_period(pt_start(), pt_end());
		return true;
	}

	BounceCurve::ActivePoint BounceCurve::pt_update(const mkaul::Point<double>& pt, const GraphView&) noexcept {
		bool moved_start = pt_start_.update(mkaul::Point{ std::min(pt.x, pt_end().x()), pt.y });
		bool moved_end = pt_end_.update(mkaul::Point{ std::max(pt.x, pt_start().x()), pt.y });
		
		if (moved_start or moved_end) {
			handle_.from_param(cor_, period_, pt_start(), pt_end());

			if (moved_start) return ActivePoint::Start;
			else return ActivePoint::End;
		}
		else return ActivePoint::Null;
	}

	bool BounceCurve::pt_move(ActivePoint active_pt, const mkaul::Point<double>& pt) noexcept {
		if (GraphCurve::pt_move(active_pt, pt)) {
			handle_.from_param(cor_, period_, pt_start(), pt_end());
			return true;
		}
		else return false;
	}
}