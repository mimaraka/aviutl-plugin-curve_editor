#pragma once

#include <string>
#include <functional>



namespace cved {
	using CurveFunction = std::function<double(double, double, double)>;
	
	class Modifier {
		std::string name_;

	public:
		Modifier(const std::string& name = "") noexcept : name_{name} {}

		auto name() const noexcept { return name_; }
		void set_name(const std::string& name) noexcept { name_ = name; }

		virtual CurveFunction apply(const CurveFunction& function) const noexcept = 0;
	};
}