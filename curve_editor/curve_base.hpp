#pragma once

#include "curve_id_manager.hpp"
#include <mkaul/graphics.hpp>
#include <nlohmann/json.hpp>



namespace cved {
	// カーブ(抽象クラス)
	class Curve {
		bool locked_;
		const uint32_t id_;

	public:
		Curve() noexcept :
			id_{global::id_manager.create_id(this)},
			locked_{false}
		{}
		virtual ~Curve() noexcept {
			global::id_manager.remove_id(id_);
		}

		// カーブの値を取得
		virtual double curve_function(double progress, double start, double end) const noexcept = 0;
		virtual double get_value(double progress, double start, double end) const noexcept;
		double get_velocity(double progress, double start, double end) const noexcept;
		virtual void clear() noexcept = 0;
		virtual void lock() noexcept { locked_ = true; }
		virtual void unlock() noexcept { locked_ = false; }
		auto is_locked() const noexcept { return locked_; }
		virtual bool is_default() const noexcept = 0;
		auto get_id() const noexcept { return id_; }
		constexpr virtual std::string get_type() const noexcept = 0;
		virtual nlohmann::json create_json() const noexcept;
		virtual bool load_json(const nlohmann::json& data) noexcept = 0;
	};
} // namespace cved