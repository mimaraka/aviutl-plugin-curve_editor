#include "handle_elastic_amp.hpp"



namespace cved {
	bool ElasticAmpHandle::is_hovered(const mkaul::Point<double>& point, const GraphView& view) const noexcept {
		return point_left_.is_hovered(point, view) or point_right_.is_hovered(point, view);
	}

	bool ElasticAmpHandle::check_hover(const mkaul::Point<double>& point, const GraphView& view) noexcept {
		return point_left_.check_hover(point, view) or point_right_.check_hover(point, view);
	}

	bool ElasticAmpHandle::update(
		const mkaul::Point<double>& point,
		const ControlPoint& point_start,
		const ControlPoint& point_end
	) noexcept {
		double dest_y = mkaul::clamp(
			point.y,
			std::max(point_start.y(), point_end.y()),
			std::min(point_start.y(), point_end.y()) + std::abs(point_start.y() - point_end.y()) * 2.
		);
		auto point_dest_left = mkaul::Point{ point_start.x(), dest_y };
		auto point_dest_right = mkaul::Point{ point_end.x(), dest_y };

		if (point_left_.is_controlled() or point_right_.is_controlled()) {
			point_left_.move(point_dest_left);
			point_right_.move(point_dest_right);
			return true;
		}
		else return false;
	}

	void ElasticAmpHandle::move(
		const mkaul::Point<double>& point,
		const ControlPoint& point_start,
		const ControlPoint& point_end
	) noexcept {
		double dest_y = mkaul::clamp(
			point.y,
			std::max(point_start.y(), point_end.y()),
			std::min(point_start.y(), point_end.y()) + std::abs(point_start.y() - point_end.y()) * 2.
		);

		auto point_dest_left = mkaul::Point{ point_start.x(), dest_y };
		auto point_dest_right = mkaul::Point{ point_end.x(), dest_y };

		point_left_.move(point_dest_left);
		point_right_.move(point_dest_right);
	}

	void ElasticAmpHandle::end_control() noexcept {
		point_left_.end_control();
		point_right_.end_control();
	}

	double ElasticAmpHandle::get_amp(const ControlPoint& point_start, const ControlPoint& point_end) const noexcept {
		return (point_left_.y() - std::max(point_start.y(), point_end.y())) / std::abs(point_start.y() - point_end.y());
	}

	void ElasticAmpHandle::from_param(double amp, const ControlPoint& point_start, const ControlPoint& point_end) noexcept {
		move(
			mkaul::Point{
				0.,
				std::max(point_start.y(), point_end.y()) + mkaul::clamp(amp, 0., 1.) * std::abs(point_start.y() - point_end.y())
			}, point_start, point_end
		);
	}
}