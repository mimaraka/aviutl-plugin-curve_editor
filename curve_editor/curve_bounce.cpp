#include "curve_bounce.hpp"
#include "curve_data.hpp"
#include "enum.hpp"



namespace cved {
	// コンストラクタ
	BounceCurve::BounceCurve(
		const mkaul::Point<double>& point_start,
		const mkaul::Point<double>& point_end,
		bool point_fixed,
		GraphCurve* prev,
		GraphCurve* next,
		double cor,
		double period
	) : NumericGraphCurve{ point_start, point_end, point_fixed, prev, next}, handle_{}, cor_{ cor }, period_{ period } {
		handle_.from_param(cor, period, point_start, point_end);
	}

	double func_bounce_1(double prog, double cor) {
		return std::floor(std::log((cor - 1.) * prog + 1.) / std::log(cor));
	}

	double func_bounce_2(double prog, double cor) {
		return prog + 0.5 + 1. / (cor - 1.) - (cor + 1.) * std::pow(cor, func_bounce_1(prog + .5, cor)) / (2. * cor - 2.);
	}

	double func_bounce_3(double prog, double cor, double period) {
		return 4. * func_bounce_2(prog / period, cor) * func_bounce_2(prog / period, cor) - std::pow(cor, 2. * func_bounce_1(prog / period + .5, cor));
	}

	double BounceCurve::get_value(double progress, double start, double end) const noexcept {
		progress = mkaul::clamp((progress - point_start_.x()) / (point_end_.x() - point_start_.x()), 0., 1.);

		double ret;
		const double limit_value = period_ * (1. / (1. - cor_) - .5);

		if (handle_.is_reverse()) {
			progress = 1. - progress;
		}
		if (limit_value > 1.) {
			int n = (int)(std::log(1. + (cor_ - 1.) * (1 / period_ + .5)) / std::log(cor_));
			if (progress < period_ * ((std::pow(cor_, n) - 1.) / (cor_ - 1.) - .5)) {
				ret = func_bounce_3(progress, cor_, period_);
			}
			else {
				ret = 0.;
			}
		}
		else {
			if (progress < limit_value)
				ret = func_bounce_3(progress, cor_, period_);
			else
				ret = 0;
		}
		if (handle_.is_reverse()) {
			ret = -1. - ret;
		}
		return start + (end - start) * (point_end_.y() + (point_end_.y() - point_start_.y()) * ret);
	}

	void BounceCurve::clear() noexcept {
		handle_.from_param(DEFAULT_COR, DEFAULT_PERIOD, point_start_, point_end_);
		cor_ = DEFAULT_COR;
		period_ = DEFAULT_PERIOD;
	}

	void BounceCurve::reverse() noexcept {
		handle_.reverse(point_start_, point_end_);
		GraphCurve::reverse();
		handle_.from_param(cor_, period_, point_start_, point_end_);
	}

	void BounceCurve::create_data(std::vector<byte>& data) const noexcept {
		BounceCurveData data_bounce{
			.data_graph = GraphCurveData{
				.start_x = point_start_.x(),
				.start_y = point_start_.y(),
				.end_x = point_end_.x(),
				.end_y = point_end_.y()
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
		point_start_.move(mkaul::Point{ p_curve_data->data_graph.start_x, p_curve_data->data_graph.start_y });
		point_end_.move(mkaul::Point{ p_curve_data->data_graph.end_x, p_curve_data->data_graph.end_y });
		handle_.from_param(p_curve_data->cor, p_curve_data->period, point_start_, point_end_);
		cor_ = handle_.get_cor(point_start_, point_end_);
		period_ = handle_.get_period(point_start_, point_end_);
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
			handle_.set_is_reverse(true, point_start_, point_end_);
			number = -code - MIN;
		}
		else if (mkaul::in_range(code, MIN, MAX, true)) {
			handle_.set_is_reverse(false, point_start_, point_end_);
			number = code - MIN;
		}
		else return false;

		int x, y;

		x = number / 1001;
		y = number - x * 1001;
		cor_ = std::sqrt(1. - y * 0.001);
		period_ = 2. * 0.001 * x / (cor_ + 1.);

		handle_.from_param(cor_, period_, point_start_, point_end_);

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
		mkaul::WindowRectangle rect_wnd;
		p_graphics->get_rect(&rect_wnd);

		auto handle_client = view.view_to_client(handle_.get_point().point(), rect_wnd);
		p_graphics->draw_ellipse(handle_client, tip_radius, tip_radius, color, mkaul::graphics::Stroke{ tip_thickness });
	}

	bool BounceCurve::is_handle_hovered(const mkaul::Point<double>& point, float box_width, const GraphView& view) const noexcept {
		return handle_.is_hovered(point, box_width, view);
	}

	bool BounceCurve::handle_check_hover(
		const mkaul::Point<double>& point,
		float box_width,
		const GraphView& view
	) noexcept {
		return handle_.check_hover(point, box_width, view);
	}

	bool BounceCurve::handle_update(
		const mkaul::Point<double>& point,
		const GraphView& view
	) noexcept {
		if (handle_.update(point, point_start_, point_end_)) {
			cor_ = handle_.get_cor(point_start_, point_end_);
			period_ = handle_.get_period(point_start_, point_end_);
			return true;
		}
		else return false;
	}

	void BounceCurve::handle_end_control() noexcept {
		handle_.end_control();
	}

	BounceCurve::ActivePoint BounceCurve::point_check_hover(const mkaul::Point<double>& point, float box_width, const GraphView& view) noexcept {
		bool start = point_start_.check_hover(point, box_width, view);
		bool end = point_end_.check_hover(point, box_width, view);
		
		if (start or end) {
			cor_ = handle_.get_cor(point_start_, point_end_);
			period_ = handle_.get_period(point_start_, point_end_);

			if (start) return ActivePoint::Start;
			else return ActivePoint::End;
		}
		else return ActivePoint::Null;
	}

	bool BounceCurve::point_begin_move(ActivePoint active_point) noexcept {
		cor_ = handle_.get_cor(point_start_, point_end_);
		period_ = handle_.get_period(point_start_, point_end_);
		return true;
	}

	BounceCurve::ActivePoint BounceCurve::point_update(const mkaul::Point<double>& point, const GraphView&) noexcept {
		bool moved_start = point_start_.update(mkaul::Point{ std::min(point.x, point_end_.x()), point.y });
		bool moved_end = point_end_.update(mkaul::Point{ std::max(point.x, point_start_.x()), point.y });
		
		if (moved_start or moved_end) {
			handle_.from_param(cor_, period_, point_start_, point_end_);

			if (moved_start) return ActivePoint::Start;
			else return ActivePoint::End;
		}
		else return ActivePoint::Null;
	}

	bool BounceCurve::point_move(ActivePoint active_point, const mkaul::Point<double>& point) noexcept {
		if (GraphCurve::point_move(active_point, point)) {
			handle_.from_param(cor_, period_, point_start_, point_end_);
			return true;
		}
		else return false;
	}
}