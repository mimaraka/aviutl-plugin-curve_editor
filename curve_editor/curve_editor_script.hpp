#pragma once

#include <vector>
#include "curve_script.hpp"



namespace cved {
	namespace global {
		class ScriptCurveEditor {
		private:
			std::vector<ScriptCurve> curves_script_;

			int index_ = 0;

		public:
			ScriptCurveEditor() :
				curves_script_{}
			{}

			ScriptCurve* curve_script() noexcept { return &curves_script_[index_]; }
			auto index() const noexcept { return index_; }
		};
	}
}