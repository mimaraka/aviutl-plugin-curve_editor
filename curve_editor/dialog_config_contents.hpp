#pragma once

#include <vector>
#include <Windows.h>



namespace cved {
	class ConfigDialogContents {
		static constexpr int ID_CATEGORY_GENERAL = 0x8000;
		static constexpr int ID_CATEGORY_APPEARANCE = 0x8001;
		static constexpr int ID_CATEGORY_BEHAVIOR = 0x8002;
		static constexpr int ID_CATEGORY_EDITING = 0x8003;

		struct CategoryInfo {
			int id;
			const char* name;
			int resource;
			DLGPROC proc;
			HWND hwnd;
		};
		std::vector<CategoryInfo> categories_;
		HWND hwnd_list_;

	public:
		ConfigDialogContents() noexcept;

		bool init(HWND hwnd) noexcept;
		auto hwnd_list() const noexcept { return hwnd_list_; }
		void update() noexcept;
	};
}