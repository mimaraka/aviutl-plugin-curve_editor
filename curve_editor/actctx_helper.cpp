#include "actctx_helper.hpp"
#include "constants.hpp"
#include "resource.h"
#include <WinBase.h>



namespace curve_editor {
	bool ActCtxHelper::init() noexcept {
		ACTCTXA actctx{
			.cbSize = sizeof(ACTCTXA),
			.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID,
			.lpResourceName = MAKEINTRESOURCEA(ID_MANIFEST_VISUALSTYLE),
			.hModule = ::GetModuleHandleA(global::PLUGIN_DLL_NAME)
		};
		HANDLE tmp = NULL;
		::GetCurrentActCtx(&tmp);
		if (!tmp) {
			hactctx_ = ::CreateActCtxA(&actctx);
			if (hactctx_ == INVALID_HANDLE_VALUE) return false;
			auto ret = ::ActivateActCtx(hactctx_, &cookie_);
			if (!ret) {
				::ReleaseActCtx(hactctx_);
				return false;
			}
			return true;
		}
		return false;
	}


	bool ActCtxHelper::exit() {
		HANDLE tmp;
		bool ret = false;
		::GetCurrentActCtx(&tmp);
		if (tmp == hactctx_) {
			ret = ::DeactivateActCtx(NULL, cookie_);
		}
		if (hactctx_) ::ReleaseActCtx(hactctx_);
		return ret;
	}
} // namespace curve_editor