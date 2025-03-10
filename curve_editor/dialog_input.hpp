#pragma once

#include "dialog.hpp"
#include <functional>
#include <string>



namespace curve_editor {
	class InputDialog : public Dialog {
		static constexpr size_t MAX_TEXT = 1024u;

		HWND hwnd_edit_ = NULL;
		const std::wstring prompt_;
		const std::wstring caption_;
		const std::wstring default_text_;
		std::function<bool(HWND, const std::wstring&)> on_submit_ = nullptr;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;

	public:
		InputDialog(
			const std::wstring_view& prompt,
			const std::wstring_view& caption,
			std::function<bool(HWND, const std::wstring&)> on_submit,
			const std::wstring_view& default_text = L""
		) noexcept :
			prompt_{ prompt },
			caption_{ caption },
			default_text_{ default_text },
			on_submit_{ on_submit }
		{}
	};
} // namespace curve_editor