#include "config.hpp"
#include "global.hpp"
#include "preset.hpp"


namespace cved {
	bool Preset::create(const mkaul::WindowRectangle& rect) noexcept {
		return (bool)window_.create(
			global::fp->dll_hinst,
			hwnd_parent_,
			NULL,
			"PRESET",
			wndproc,
			WS_CHILD,
			CS_DBLCLKS,
			rect,
			mkaul::WindowRectangle{},
			::LoadCursorA(NULL, IDC_HAND),
			this
		);
	}

	void Preset::draw() {

	}

	LRESULT CALLBACK Preset::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
		Preset* inst = (Preset*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);

		// ウィンドウがまだ作成されていない場合
		if (!inst) {
			// ここでウィンドウが作成される場合
			if (message == WM_CREATE) {
				// lParamに格納されているLPCREATESTRUCTからユーザーデータにアクセスできるためそこから取得
				inst = (Preset*)((LPCREATESTRUCT)lparam)->lpCreateParams;
				if (inst) ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)inst);
			}
			else return ::DefWindowProc(hwnd, message, wparam, lparam);
		}

		switch (message) {
		case WM_CREATE:
			inst->p_graphics_ = mkaul::graphics::Factory::create_graphics();
			return 0;

		case WM_PAINT:
			inst->draw();
			return 0;

		default:
			return ::DefWindowProc(hwnd, message, wparam, lparam);
		}
	}
}