#pragma once

#include "curve_graph.hpp"
#include "dialog.hpp"
#include <type_traits>



namespace curve_editor {
	class ModifierDialog : public Dialog {
		HWND hwnd_list_modifier_ = NULL;
		HWND hwnd_button_add_ = NULL;
		HWND hwnd_button_remove_ = NULL;
		HWND hwnd_button_edit_ = NULL;
		HWND hwnd_button_rename_ = NULL;
		HWND hwnd_button_up_ = NULL;
		HWND hwnd_button_down_ = NULL;
		HWND hwnd_check_bypass_ = NULL;
		HMENU menu_modifier_ = NULL;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd, const GraphCurve& curve) noexcept;
		void update_list(const GraphCurve& curve) noexcept;
		void update_buttons(const GraphCurve& curve) noexcept;

		template<uint32_t N>
		std::unique_ptr<Modifier> create_modifier(uint32_t) { return nullptr; }

		template<uint32_t N = 0u, typename Head, typename... Tail>
		std::unique_ptr<Modifier> create_modifier(uint32_t type) {
			if (type == N) {
				return std::make_unique<Head>();
			}
			else {
				return create_modifier<N + 1u, Tail...>(type);
			}
		}

		template<int N = 0> void show_editor_dialog(HWND, const Modifier*) {}

		template<typename ModifierType, typename DialogType, typename... Tail, int N = 0>
		void show_editor_dialog(HWND hwnd, const Modifier* p_mod_base) {
			if (typeid(*p_mod_base) == typeid(ModifierType)) {
				auto p_mod_derived = dynamic_cast<const ModifierType*>(p_mod_base);
				DialogType dialog;
				dialog.show(hwnd, std::bit_cast<LPARAM>(p_mod_derived));
			}
			else {
				show_editor_dialog<Tail...>(hwnd, p_mod_base);
			}
		}
	};

	template<int dummy = 0> void save_prev_modifier(const std::unique_ptr<Modifier>&, std::vector<std::unique_ptr<Modifier>>&) {}

	template<typename ModifierType, typename... Tail, int dummy = 0>
	void save_prev_modifier(const std::unique_ptr<Modifier>& p_modifier, std::vector<std::unique_ptr<Modifier>>& modifiers_prev) {
		if (typeid(*p_modifier) == typeid(ModifierType)) {
			modifiers_prev.emplace_back(
				std::make_unique<ModifierType>(
					dynamic_cast<ModifierType&>(*p_modifier)
				)
			);
		}
		else {
			save_prev_modifier<Tail...>(p_modifier, modifiers_prev);
		}
	}
} // namespace curve_editor