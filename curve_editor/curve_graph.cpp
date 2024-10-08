#include "curve_graph.hpp"



namespace cved {
	double GraphCurve::get_value(double progress, double start, double end) const noexcept {
		CurveFunction func_ret = [this](double prog, double st, double ed) {return curve_function(prog, st, ed); };
		for (const auto& modifier : modifiers_) {
			if (modifier->enabled()) {
				func_ret = modifier->apply(func_ret);
			}
		}
		return func_ret(progress, start, end);
	}

	void GraphCurve::reverse(bool fix_pt) noexcept {
		if (!fix_pt) {
			auto tmp = anchor_start_;
			anchor_start_ = mkaul::Point{ 1., 1. } - anchor_end_;
			anchor_end_ = mkaul::Point{ 1., 1. } - tmp;
		}
	}

	void GraphCurve::set_anchor_start(double x, double y) noexcept {
		if (anchor_fixed_ or !prev_) {
			return;
		}
		auto ret = mkaul::Point{
			mkaul::clamp(x, prev_->anchor_start_.x, anchor_end_.x),
			y
		};
		anchor_start_ = ret;
		//prev_->anchor_end_ = ret;
	}

	void GraphCurve::set_anchor_end(double x, double y) noexcept {
		if (anchor_fixed_ or !next_) {
			return;
		}
		auto ret = mkaul::Point{
			mkaul::clamp(x, anchor_start_.x, next_->anchor_end_.x),
			y
		};
		anchor_end_ = ret;
		//next_->anchor_start_ = ret;
	}



	Modifier* GraphCurve::get_modifier(size_t idx) const noexcept {
		if (idx < modifiers_.size()) {
			return modifiers_[idx].get();
		}
		else return nullptr;
	}

	bool GraphCurve::remove_modifier(size_t idx) noexcept {
		if (idx < modifiers_.size()) {
			modifiers_.erase(modifiers_.begin() + idx);
			return true;
		}
		else return false;
	}
}