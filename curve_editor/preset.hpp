#pragma once

#include "curve.hpp"
#include <mkaul/window.hpp>


namespace cved {
	class Preset {
    private:
        inline static size_t n_presets = 0u;
        Curve* p_curve_;
        mkaul::ui::Window window_;
        HWND hwnd_parent_;
        std::unique_ptr<mkaul::graphics::Graphics> p_graphics_;
        size_t id;

        static size_t register_preset();
        static LRESULT CALLBACK wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
        void draw();

    public:
        Preset(HWND hwnd, Curve* p_curve) :
            p_curve_(p_curve),
            window_(),
            hwnd_parent_(hwnd),
            p_graphics_(nullptr)
        {}

        bool create(const mkaul::WindowRectangle& rect) noexcept;
	};
}