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
			auto size() const noexcept { return curves_script_.size(); }
			bool set_idx(int idx) noexcept;
			bool advance_idx(int n) noexcept { return set_idx((int)idx_ + n); }
			void jump_to_last_idx() noexcept;
			void append_idx() noexcept { append_curve(ScriptCurve{}); }
			void append_curve(const ScriptCurve& curve) noexcept { curves_script_.emplace_back(curve); }
			void pop_idx() noexcept;
			bool is_idx_first() const noexcept { return idx_ == 0u; }
			bool is_idx_last() const noexcept { return idx_ == curves_script_.size() - 1; }
			bool is_idx_max() const noexcept { return idx_ == global::CURVE_ID_MAX; }

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
	} // namespace global
} // namespace cved

CEREAL_CLASS_VERSION(cved::global::ScriptCurveEditor, 0);