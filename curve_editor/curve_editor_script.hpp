#pragma once

#include <vector>
#include "curve_script.hpp"



namespace cved {
	namespace global {
		class ScriptCurveEditor {
		private:
			std::vector<ScriptCurve> curves_script_;
			size_t idx_ = 0u;

		public:
			ScriptCurveEditor() :
				curves_script_{}
			{}

			void reset() noexcept;
			ScriptCurve* curve_script(size_t idx) noexcept;
			ScriptCurve* curve_script() noexcept { return curve_script(idx_); }
			auto idx() const noexcept { return idx_; }
			bool set_idx(size_t idx) noexcept;
			void jump_to_last_idx() noexcept;
			void delete_last_idx() noexcept;
			bool is_idx_last() const noexcept { return idx_ == curves_script_.size() - 1; }

			template <class Archive>
			void serialize(Archive& archive, const std::uint32_t) {
				archive(curves_script_);
			}
		};
	}
}

CEREAL_CLASS_VERSION(cved::global::ScriptCurveEditor, 0);