#pragma once

#include "dialog.hpp"
#include <vector>
#include "dialog_pref_general.hpp"
#include "dialog_pref_appearance.hpp"
#include "dialog_pref_behavior.hpp"
#include "dialog_pref_editing.hpp"



namespace cved {
	class PrefDialog : public Dialog {
		static constexpr int ID_CATEGORY_GENERAL = 0x8000;
		static constexpr int ID_CATEGORY_APPEARANCE = 0x8001;
		static constexpr int ID_CATEGORY_BEHAVIOR = 0x8002;
		static constexpr int ID_CATEGORY_EDITING = 0x8003;

		struct CategoryInfo {
			int id;
			const char* name;
			Dialog* p_dialog;
			HWND hwnd;
		};

		GeneralPrefDialog dialog_general_;
		AppearancePrefDialog dialog_appearance_;
		BehaviorPrefDialog dialog_behavior_;
		EditingPrefDialog dialog_editing_;
		HWND hwnd_list_categories_ = NULL;
		std::vector<CategoryInfo> categories_;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd) noexcept override;
		void update() noexcept;
		void load_config() noexcept;
		void save_config() noexcept;

	public:
		PrefDialog();
	};
}