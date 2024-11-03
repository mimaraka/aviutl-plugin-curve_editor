#pragma once

#include "constants.hpp"
#include "curve_base.hpp"
#include "string_table.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>



namespace cved {
	// カーブ(スクリプト)
	class ScriptCurve : public Curve {
		static constexpr auto DEFAULT_SCRIPT = "return (ed - st) * t + st";
		std::string script_;

	public:
		ScriptCurve() : script_{ DEFAULT_SCRIPT }
		{}

		ScriptCurve(const ScriptCurve& curve) :
			Curve{ curve },
			script_{ curve.script_ }
		{}

		[[nodiscard]] constexpr std::string get_name() const noexcept override { return global::CURVE_NAME_SCRIPT; }
		[[nodiscard]] std::string get_disp_name() const noexcept override { return global::string_table[global::StringTable::StringId::LabelEditModeScript]; }
		[[nodiscard]] double curve_function(double, double start, double) const noexcept override;
		void clear() noexcept override { script_ = DEFAULT_SCRIPT; }
		[[nodiscard]] bool is_default() const noexcept override { return true; }

		[[nodiscard]] const auto& script() const noexcept { return script_; }
		void set_script(const std::string& script) noexcept { script_ = script; }

		[[nodiscard]] nlohmann::json create_json() const noexcept override;
		bool load_json(const nlohmann::json& data) noexcept override;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(script_);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(script_);
		}
	};
}

CEREAL_CLASS_VERSION(cved::ScriptCurve, 0);
CEREAL_REGISTER_TYPE(cved::ScriptCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::Curve, cved::ScriptCurve)
