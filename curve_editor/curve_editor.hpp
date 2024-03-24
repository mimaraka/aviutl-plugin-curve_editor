#pragma once

#include "curve.hpp"
#include "enum.hpp"
#include "curve_editor_graph.hpp"
#include "curve_editor_script.hpp"
#include <vector>



namespace cved {
	namespace global {
		inline class CurveEditor {
		private:
			static constexpr size_t IDCURVE_MAX_N = 4096u;

			GraphCurveEditor editor_graph_;
			ScriptCurveEditor editor_script_;

		public:
			CurveEditor() {}

			int32_t track_param() noexcept;
			Curve* current_curve() noexcept;
			auto& editor_graph() noexcept { return editor_graph_; }
			auto& editor_script() noexcept { return editor_script_; }
			void reset_id_curves() noexcept;
			
			void create_data(std::vector<byte>& data) const noexcept;
			bool load_data(const byte* data, size_t size) noexcept;
		} editor;
	}
}