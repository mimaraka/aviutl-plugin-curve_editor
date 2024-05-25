#pragma once

#include <string>
#include <functional>



namespace cved {
	using CurveFunction = std::function<double(double, double, double)>;
	
	class Modifier {
		std::string name_;
		bool enabled_ = true;

	public:
		Modifier(const std::string& name = "") noexcept : name_{name} {}

		auto name() const noexcept { return name_; }
		void set_name(const std::string& name) noexcept { name_ = name; }

		auto enabled() const noexcept { return enabled_; }
		void set_enabled(bool enabled) noexcept { enabled_ = enabled; }

		virtual CurveFunction convert(const CurveFunction& function) const noexcept = 0;
		virtual CurveFunction apply(const CurveFunction& function) const noexcept;
	};
}