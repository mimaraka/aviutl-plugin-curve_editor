#include "handle_elastic_amp.hpp"



namespace cved {
	bool ElasticAmpHandle::is_hovered(const mkaul::Point<double>& pt, const GraphView& view) const noexcept {
		return ctl_pt_left_.is_hovered(pt, view) or ctl_pt_right_.is_hovered(pt, view);
	}

	bool ElasticAmpHandle::check_hover(const mkaul::Point<double>& pt, const GraphView& view) noexcept {
		return ctl_pt_left_.check_hover(pt, view) or ctl_pt_right_.check_hover(pt, view);
	}

	bool ElasticAmpHandle::update(
		const mkaul::Point<double>& pt,
		const ControlPoint& pt_start,
		const ControlPoint& pt_end
	) noexcept {
		double dest_y = mkaul::clamp(
			pt.y,
			std::max(pt_start.y(), pt_end.y()),
			std::min(pt_start.y(), pt_end.y()) + std::abs(pt_start.y() - pt_end.y()) * 2.
		);
		auto pt_dest_left = mkaul::Point{ pt_start.x(), dest_y };
		auto pt_dest_right = mkaul::Point{ pt_end.x(), dest_y };

		if (ctl_pt_left_.is_controlled() or ctl_pt_right_.is_controlled()) {
			ctl_pt_left_.move(pt_dest_left);
			ctl_pt_right_.move(pt_dest_right);
			return true;
		}
		else return false;
	}

	void ElasticAmpHandle::move(
		const mkaul::Point<double>& pt,
		const ControlPoint& pt_start,
		const ControlPoint& pt_end
	) noexcept {
		double dest_y = mkaul::clamp(
			pt.y,
			std::max(pt_start.y(), pt_end.y()),
			std::min(pt_start.y(), pt_end.y()) + std::abs(pt_start.y() - pt_end.y()) * 2.
		);

		auto pt_dest_left = mkaul::Point{ pt_start.x(), dest_y };
		auto pt_dest_right = mkaul::Point{ pt_end.x(), dest_y };

		ctl_pt_left_.move(pt_dest_left);
		ctl_pt_right_.move(pt_dest_right);
	}

	void ElasticAmpHandle::end_control() noexcept {
		ctl_pt_left_.end_control();
		ctl_pt_right_.end_control();
	}

	double ElasticAmpHandle::get_amp(const ControlPoint& pt_start, const ControlPoint& pt_end) const noexcept {
		return (ctl_pt_left_.y() - std::max(pt_start.y(), pt_end.y())) / std::abs(pt_start.y() - pt_end.y());
	}

	void ElasticAmpHandle::from_param(double amp, const ControlPoint& pt_start, const ControlPoint& pt_end) noexcept {
		move(
			mkaul::Point{
				0.,
				std::max(pt_start.y(), pt_end.y()) + mkaul::clamp(amp, 0., 1.) * std::abs(pt_start.y() - pt_end.y())
			}, pt_start, pt_end
		);
	}
}