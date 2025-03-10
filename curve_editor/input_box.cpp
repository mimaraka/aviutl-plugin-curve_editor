#include "input_box.hpp"
#include "dialog_input.hpp"



namespace curve_editor::util {
	void input_box(
		HWND hwnd,
		const std::wstring_view& prompt,
		const std::wstring_view& caption,
		std::function<bool(HWND, const std::wstring&)> on_submit,
		const std::wstring_view& default_text
	) {
		InputDialog dialog{ prompt, caption, on_submit, default_text };
		dialog.show(hwnd);
	}
} // namespace curve_editor::util