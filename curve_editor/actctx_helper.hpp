#pragma once

#include <Windows.h>



namespace curve_editor {
    class ActCtxHelper {
        ULONG_PTR cookie_ = 0ul;
        HANDLE hactctx_ = NULL;

    public:
        bool init() noexcept;
        bool exit();
    };
} // namespace curve_editor