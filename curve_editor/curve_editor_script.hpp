#pragma once

#include <vector>
#include "curve_script.hpp"



namespace cved {
	namespace global {
		class ScriptCurveEditor {
		private:
			std::vector<ScriptCurve> curves_script_;
			size_t index_ = 0u;

		public:
			ScriptCurveEditor() :
				curves_script_{}
			{}

			void reset() noexcept;
			ScriptCurve* curve_script() noexcept { return &curves_script_[index_]; }
			auto idx() const noexcept { return index_; }
			bool set_idx(size_t idx) noexcept;
			void jump_to_last_idx() noexcept;
			bool is_idx_last() const noexcept { return index_ == curves_script_.size() - 1; }
		};
	}
}