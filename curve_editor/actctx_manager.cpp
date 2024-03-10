#include "actctx_manager.hpp"
#include <WinBase.h>
#include "resource.h"



namespace cved {
	bool ActCtxManager::init(HINSTANCE hinst) noexcept {
		ACTCTXA actctx{
			.cbSize = sizeof(ACTCTXA),
			.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID,
			.lpResourceName = MAKEINTRESOURCEA(ID_MANIFEST_VISUALSTYLE),
			.hModule = hinst
		};
		hactctx_ = ::CreateActCtxA(&actctx);
		if (hactctx_ == INVALID_HANDLE_VALUE) return false;
		auto ret = ::ActivateActCtx(hactctx_, &cookie_);
		if (!ret) {
			::ReleaseActCtx(hactctx_);
			return false;
		}
		return true;
	}


	bool ActCtxManager::exit() noexcept {
		auto ret = ::DeactivateActCtx(NULL, cookie_);
		if (hactctx_) ::ReleaseActCtx(hactctx_);
		return ret;
	}
}