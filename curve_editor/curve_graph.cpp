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

	void GraphCurve::begin_move_anchor_start(bool bound) noexcept {
		if (is_locked()) return;
		if (!bound and prev_) {
			prev_->begin_move_anchor_end(true);
		}
	}

	void GraphCurve::begin_move_anchor_end(bool bound) noexcept {
		if (is_locked()) return;
		if (!bound and next_) {
			next_->begin_move_anchor_start(true);
		}
	}

	void GraphCurve::move_anchor_start(double x, double y, bool forced, bool bound) noexcept {
		if ((anchor_fixed_ or !prev_ or is_locked()) and !forced) {
			return;
		}
		auto ret = mkaul::Point{
			mkaul::clamp(x, prev_ ? prev_->anchor_start_.x : 0., anchor_end_.x),
			y
		};
		anchor_start_ = ret;
		if (!bound and prev_) {
			prev_->move_anchor_end(ret, false, true);
		}
	}

	void GraphCurve::move_anchor_end(double x, double y, bool forced, bool bound) noexcept {
		if ((anchor_fixed_ or !next_ or is_locked()) and !forced) {
			return;
		}
		auto ret = mkaul::Point{
			mkaul::clamp(x, anchor_start_.x, next_ ? next_->anchor_end_.x : 1.),
			y
		};
		anchor_end_ = ret;
		if (!bound and next_) {
			next_->move_anchor_start(ret, false, true);
		}
	}

	void GraphCurve::end_move_anchor_start(bool bound) noexcept {
		if (!bound and prev_) {
			prev_->end_move_anchor_end(true);
		}
	}

	void GraphCurve::end_move_anchor_end(bool bound) noexcept {
		if (!bound and next_) {
			next_->end_move_anchor_start(true);
		}
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

	nlohmann::json GraphCurve::create_json() const noexcept {
		auto data = Curve::create_json();
		data["start"] = { anchor_start_.x, anchor_start_.y };
		data["end"] = { anchor_end_.x, anchor_end_.y };
		data["modifiers"] = nlohmann::json::array();
		for (const auto& modifier : modifiers_) {
			data["modifiers"].emplace_back(modifier->create_json());
		}
		return data;
	}

	bool GraphCurve::load_json(const nlohmann::json& data) noexcept {
		try {
			auto start_x = data.at("start")[0].get<double>();
			auto start_y = data.at("start")[1].get<double>();
			auto end_x = data.at("end")[0].get<double>();
			auto end_y = data.at("end")[1].get<double>();
			if (!mkaul::real_in_range(start_x, 0., 1.) or !mkaul::real_in_range(end_x, 0., 1.) or end_x < start_x) {
				return false;
			}
			anchor_start_ = mkaul::Point{ start_x, start_y };
			anchor_end_ = mkaul::Point{ end_x, end_y };
			modifiers_.clear();
			for (const auto& modifier : data.at("modifiers")) {
				auto mod_type = modifier.at("type").get<std::string>();
				std::unique_ptr<Modifier> mod;
				if (mod_type == global::MODIFIER_NAME_DISCRETIZATION) {
					mod = std::make_unique<DiscretizationModifier>();
				}
				else if (mod_type == global::MODIFIER_NAME_NOISE) {
					mod = std::make_unique<NoiseModifier>();
				}
				else if (mod_type == global::MODIFIER_NAME_SINE_WAVE) {
					mod = std::make_unique<SineWaveModifier>();
				}
				else if (mod_type == global::MODIFIER_NAME_SQUARE_WAVE) {
					mod = std::make_unique<SquareWaveModifier>();
				}
				if (!mod or !mod->load_json(modifier)) {
					return false;
				}
				mod->set_curve(this);
				modifiers_.emplace_back(std::move(mod));
			}
			return true;
		}
		catch (const nlohmann::json::exception&) {
			return false;
		}
	}
} // namespace cved