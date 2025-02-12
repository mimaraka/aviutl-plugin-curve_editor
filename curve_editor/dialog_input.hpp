#pragma once

#include "dialog.hpp"
#include <functional>
#include <string>



namespace curve_editor {
	class InputDialog : public Dialog {
		static constexpr size_t MAX_TEXT = 1024u;

		HWND hwnd_edit_ = NULL;
		const std::string prompt_;
		const std::string caption_;
		const std::string default_text_;
		std::function<bool(HWND, const std::string&)> on_submit_ = nullptr;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;

	public:
		InputDialog(
			const std::string& prompt,
			const std::string& caption,
			std::function<bool(HWND, const std::string&)> on_submit,
			const std::string& default_text = ""
		) noexcept :
			prompt_{ prompt },
			caption_{ caption },
			default_text_{ default_text },
			on_submit_{ on_submit }
		{}
	};
} // namespace curve_editor