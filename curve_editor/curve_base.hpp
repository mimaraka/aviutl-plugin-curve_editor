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
		// コンストラクタ
		Curve() noexcept :
			id_{global::id_manager.create_id(this)},
			locked_{false}
		{}
		// コピーコンストラクタ
		Curve(const Curve& curve) noexcept :
			id_{global::id_manager.create_id(this)},
			locked_{curve.locked_}
		{}
		// 仮想デストラクタ
		virtual ~Curve() noexcept {
			global::id_manager.remove_id(id_);
		}

		// カーブの値を取得
		[[nodiscard]] virtual double curve_function(double progress, double start, double end) const noexcept = 0;
		[[nodiscard]] virtual double get_value(double progress, double start, double end) const noexcept;
		[[nodiscard]] double get_velocity(double progress, double start, double end) const noexcept;
		virtual void clear() noexcept = 0;
		virtual void lock() noexcept { locked_ = true; }
		virtual void unlock() noexcept { locked_ = false; }
		[[nodiscard]] auto is_locked() const noexcept { return locked_; }
		[[nodiscard]] virtual bool is_default() const noexcept = 0;
		[[nodiscard]] auto get_id() const noexcept { return id_; }
		[[nodiscard]] constexpr virtual std::string get_name() const noexcept = 0;
		[[nodiscard]] virtual std::string get_disp_name() const noexcept = 0;
		[[nodiscard]] virtual nlohmann::json create_json() const noexcept;
		virtual bool load_json(const nlohmann::json& data) noexcept = 0;
	};
} // namespace cved