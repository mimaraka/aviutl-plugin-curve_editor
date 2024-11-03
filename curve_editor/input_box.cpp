#include "input_box.hpp"
#include "dialog_input.hpp"



namespace cved {
	namespace util {
		void input_box(
			HWND hwnd,
			const char* prompt,
			const char* caption,
			std::function<bool(HWND, const std::string&)> on_submit,
			const char* default_text
		) {
			InputDialog dialog{ prompt, caption, on_submit, default_text };
			dialog.show(hwnd);
		}
	} // namespace util
} // namespace cved