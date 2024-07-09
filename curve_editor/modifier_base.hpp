#pragma once

#include <string>
#include <functional>
#include <cereal/types/base_class.hpp>
#include <cereal/types/string.hpp>
#include <cereal/cereal.hpp>



namespace cved {
	using CurveFunction = std::function<double(double, double, double)>;
	class GraphCurve;

	class Modifier {
		std::string name_;
		bool enabled_ = true;

	protected:
		GraphCurve* p_curve_ = nullptr;

	public:
		Modifier(const std::string& name = "") noexcept : name_{ name } {}
		Modifier(const Modifier& modifier) noexcept :
			name_{ modifier.name_ },
			enabled_{ modifier.enabled_ },
			p_curve_{ modifier.p_curve_ }
		{}

		auto name() const noexcept { return name_; }
		void set_name(const std::string& name) noexcept { name_ = name; }

		auto enabled() const noexcept { return enabled_; }
		void set_enabled(bool enabled) noexcept { enabled_ = enabled; }

		auto curve() const noexcept { return p_curve_; }
		void set_curve(GraphCurve* p_curve) noexcept { p_curve_ = p_curve; }

		virtual CurveFunction convert(const CurveFunction& function) const noexcept = 0;
		virtual CurveFunction apply(const CurveFunction& function) const noexcept;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				name_,
				enabled_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t) {
			archive(
				name_,
				enabled_
			);
		}
	};
}

CEREAL_CLASS_VERSION(cved::Modifier, 0)