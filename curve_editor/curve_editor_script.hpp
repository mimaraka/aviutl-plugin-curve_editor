#pragma once

#include "curve_script.hpp"
#include <vector>



namespace cved {
	namespace global {
		class ScriptCurveEditor {
		private:
			std::vector<ScriptCurve> curves_script_;
			size_t idx_ = 0u;

		public:
			ScriptCurveEditor();

			void reset() noexcept;
			ScriptCurve* curve_script(size_t idx) noexcept;
			ScriptCurve* curve_script() noexcept { return curve_script(idx_); }
			auto idx() const noexcept { return idx_; }
			bool set_idx(size_t idx) noexcept;
			void jump_to_last_idx() noexcept;
			void delete_last_idx() noexcept;
			bool is_idx_last() const noexcept { return idx_ == curves_script_.size() - 1; }

			template <class Archive>
			void save(Archive& archive, const std::uint32_t) const {
				archive(curves_script_);
			}

			template <class Archive>
			void load(Archive& archive, const std::uint32_t) {
				archive(curves_script_);
				if (curves_script_.empty()) {
					curves_script_.emplace_back(ScriptCurve{});
				}
			}
		};
	}
}

CEREAL_CLASS_VERSION(cved::global::ScriptCurveEditor, 0);