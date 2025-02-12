#pragma once

#include "constants.hpp"
#include "my_webview2_reference.hpp"
#include <mkaul/version.hpp>
#include <thread>



namespace curve_editor::global {
	inline class UpdateChecker {
		bool is_update_available_ = false;
		std::string release_notes_;
		mkaul::Version latest_version_;
		std::thread th_;

		bool fetch_latest_version();

	public:
		~UpdateChecker() noexcept {
			if (th_.joinable()) {
				th_.join();
			}
		}

		void check_for_update() noexcept;
		[[nodiscard]] auto is_update_available() const noexcept { return is_update_available_; }
		[[nodiscard]] const auto& latest_version() const noexcept { return latest_version_; }
		[[nodiscard]] const auto& release_notes() const noexcept { return release_notes_; }
	} update_checker;
} // namespace curve_editor::global