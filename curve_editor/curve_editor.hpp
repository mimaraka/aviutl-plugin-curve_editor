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
			GraphCurveEditor editor_graph_;
			ScriptCurveEditor editor_script_;

		public:
			CurveEditor() {}

			int32_t track_param() noexcept;
			Curve* current_curve() noexcept;
			size_t current_idx() noexcept;
			bool set_idx(size_t idx) noexcept;
			bool advance_idx(int n) noexcept;
			void jump_to_last_idx() noexcept;
			bool is_idx_first() noexcept { return current_idx() == 0u; }
			bool is_idx_last() noexcept;
			void delete_last_idx() noexcept;
			void reset_id_curves() noexcept;

			auto& editor_graph() noexcept { return editor_graph_; }
			auto& editor_script() noexcept { return editor_script_; }
			
			void create_data(std::vector<byte>& data) const noexcept;
			bool load_data(const byte* data, size_t size) noexcept;
		} editor;
	}
}