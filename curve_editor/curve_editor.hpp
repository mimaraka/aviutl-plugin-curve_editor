#pragma once

#include "curve_editor_graph.hpp"
#include "curve_editor_script.hpp"
#include "enum.hpp"
#include <vector>



namespace curve_editor::global {
	inline class CurveEditor {
	private:
		GraphCurveEditor editor_graph_;
		ScriptCurveEditor editor_script_;

	public:
		CurveEditor() {}

		int32_t track_param() noexcept;
		Curve* p_current_curve() noexcept;
		size_t current_idx() noexcept;
		bool set_idx(size_t idx) noexcept;
		bool advance_idx(int n) noexcept;
		void jump_to_last_idx() noexcept;
		bool is_idx_first() noexcept { return current_idx() == 0u; }
		bool is_idx_last() noexcept;
		void pop_idx() noexcept;
		void reset_id_curves() noexcept;

		static EditMode get_mode(const std::string& type_name) noexcept;

		auto& editor_graph() noexcept { return editor_graph_; }
		auto& editor_script() noexcept { return editor_script_; }

		Curve* get_curve(EditMode mode) noexcept;

		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t) {
			archive(
				editor_graph_,
				editor_script_
			);
		}
	} editor;
} // namespace curve_editor::global

CEREAL_CLASS_VERSION(curve_editor::global::CurveEditor, 0)