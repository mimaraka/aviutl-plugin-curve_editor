#pragma once

#include "dialog.hpp"
#include <functional>
#include <string>



namespace curve_editor {
	// ハンドル・アンカーの座標を数値で入力するダイアログ
	class ControlPositionDialog : public Dialog {
		static constexpr size_t MAX_TEXT = 64u;

		HWND hwnd_edit_x_ = NULL;
		HWND hwnd_edit_y_ = NULL;
		const std::wstring caption_;
		const double init_x_;
		const double init_y_;
		// X座標を編集可能にするか（振幅ハンドルのようにY座標のみ意味を持つ場合はfalse）
		const bool enable_x_;
		// 入力値(x, y)を受け取り、適用に成功してダイアログを閉じる場合はtrueを返す
		std::function<bool(HWND, double, double)> on_submit_ = nullptr;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;

	public:
		ControlPositionDialog(
			const std::wstring_view& caption,
			double init_x,
			double init_y,
			std::function<bool(HWND, double, double)> on_submit,
			bool enable_x = true
		) noexcept :
			caption_{ caption },
			init_x_{ init_x },
			init_y_{ init_y },
			enable_x_{ enable_x },
			on_submit_{ on_submit }
		{}
	};
} // namespace curve_editor
