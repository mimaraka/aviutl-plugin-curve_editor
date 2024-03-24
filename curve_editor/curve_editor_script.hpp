#pragma once

#include <vector>
#include "curve_script.hpp"



namespace cved {
	namespace global {
		class ScriptCurveEditor {
		private:
			std::vector<ScriptCurve> curves_script_;
			int32_t index_ = 0u;

		public:
			ScriptCurveEditor() :
				curves_script_{}
			{}

			void reset() noexcept;
			ScriptCurve* curve_script() noexcept { return &curves_script_[index_]; }
			auto index() const noexcept { return index_; }
		};
	}
}