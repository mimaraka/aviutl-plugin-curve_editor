#pragma once

#include "curve.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/string.hpp>



namespace cved {
	// カーブ(スクリプト)
	class ScriptCurve : public Curve {
		static constexpr auto DEFAULT_SCRIPT = "return (ed - st) * t + st";
		std::string script_;

	public:
		ScriptCurve() : script_{DEFAULT_SCRIPT}
		{}

		std::string get_type() const noexcept override { return "script"; }
		double curve_function(double, double start, double) const noexcept override;
		void clear() noexcept override { script_ = DEFAULT_SCRIPT; }
		bool is_default() const noexcept override { return true; }

		const auto& script() const noexcept { return script_; }
		void set_script(const std::string& script) noexcept { script_ = script; }

		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t) {
			archive(script_);
		}
	};
}

CEREAL_CLASS_VERSION(cved::ScriptCurve, 0);
CEREAL_REGISTER_TYPE(cved::ScriptCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::Curve, cved::ScriptCurve)