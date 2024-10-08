#pragma once

#include "curve.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/polymorphic.hpp>



namespace cved {
	// カーブ(スクリプト)
	class ScriptCurve : public Curve {
		std::string script_;

	public:
		std::string get_type() const noexcept override { return "script"; }
		double curve_function(double, double start, double) const noexcept override { return start; }
		void clear() noexcept override {}
		bool is_default() const noexcept override { return true; }

		const auto& script() const noexcept { return script_; }

		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t) {
			archive(script_);
		}
	};
}

CEREAL_CLASS_VERSION(cved::ScriptCurve, 0);
CEREAL_REGISTER_TYPE(cved::ScriptCurve)
CEREAL_REGISTER_POLYMORPHIC_RELATION(cved::Curve, cved::ScriptCurve)