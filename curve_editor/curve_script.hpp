#pragma once

#include "constants.hpp"
#include "curve_base.hpp"
#include "string_table.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>



namespace curve_editor {
	// カーブ(スクリプト)
	class ScriptCurve : public Curve {
		static constexpr auto DEFAULT_SCRIPT = "return (ed - st) * t + st";
		std::string script_;

	public:
		// コンストラクタ
		ScriptCurve() : script_{ DEFAULT_SCRIPT }
		{}

		// コピーコンストラクタ
		ScriptCurve(const ScriptCurve& curve) :
			Curve{ curve },
			script_{ curve.script_ }
		{}

		// コピー代入演算子
		ScriptCurve& operator=(const ScriptCurve& curve) {
			if (this != &curve) {
				Curve::operator=(curve);
				script_ = curve.script_;
			}
			return *this;
		}

		[[nodiscard]] std::unique_ptr<Curve> clone() const noexcept override { return std::make_unique<ScriptCurve>(*this); }

		[[nodiscard]] constexpr std::string get_name() const noexcept override { return global::CURVE_NAME_SCRIPT; }
		[[nodiscard]] std::string get_disp_name() const noexcept override { return global::string_table[global::StringTable::StringId::LabelEditModeScript]; }
		[[nodiscard]] double curve_function(double, double start, double) const noexcept override;
		void clear() noexcept override { script_ = DEFAULT_SCRIPT; }
		[[nodiscard]] bool is_default() const noexcept override { return true; }

		[[nodiscard]] const auto& script() const noexcept { return script_; }
		void set_script(const std::string& script) noexcept {
			if (is_locked()) return;
			script_ = script;
		}

		[[nodiscard]] nlohmann::json create_json() const noexcept override;
		bool load_json(const nlohmann::json& data) noexcept override;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(cereal::base_class<Curve>(this), script_);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(cereal::base_class<Curve>(this), script_);
		}
	};
} // namespace curve_editor

CEREAL_CLASS_VERSION(curve_editor::ScriptCurve, 0);
CEREAL_REGISTER_TYPE(curve_editor::ScriptCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(curve_editor::Curve, curve_editor::ScriptCurve)
