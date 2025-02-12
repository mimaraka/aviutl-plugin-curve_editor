#pragma once

#include "curve_id_manager.hpp"
#include <cereal/cereal.hpp>
#include <nlohmann/json.hpp>



namespace curve_editor {
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
			id_{ global::id_manager.create_id(this) },
			locked_{ curve.locked_ }
		{}
		// 仮想デストラクタ
		virtual ~Curve() noexcept {
			global::id_manager.remove_id(id_);
		}

		// コピー代入演算子
		Curve& operator=(const Curve& curve) noexcept {
			locked_ = curve.locked_;
			return *this;
		}

		virtual std::unique_ptr<Curve> clone() const noexcept = 0;

		// カーブの値を取得
		[[nodiscard]] virtual double curve_function(double progress, double start, double end) const noexcept = 0;
		[[nodiscard]] virtual double get_value(double progress, double start, double end) const noexcept;
		[[nodiscard]] double get_velocity(double progress, double start, double end) const noexcept;
		virtual void clear() noexcept = 0;
		[[nodiscard]] auto is_locked() const noexcept { return locked_; }
		virtual void set_locked(bool locked) noexcept { locked_ = locked; }
		[[nodiscard]] virtual bool is_default() const noexcept = 0;
		[[nodiscard]] auto get_id() const noexcept { return id_; }
		[[nodiscard]] constexpr virtual std::string get_name() const noexcept = 0;
		[[nodiscard]] virtual std::string get_disp_name() const noexcept = 0;
		[[nodiscard]] virtual nlohmann::json create_json() const noexcept;
		virtual bool load_json(const nlohmann::json& data) noexcept = 0;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(locked_);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(locked_);
		}
	};
} // namespace curve_editor

CEREAL_CLASS_VERSION(curve_editor::Curve, 0)