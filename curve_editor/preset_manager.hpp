#pragma once

#include "preset.hpp"
#include "string_table.hpp"



namespace cved {
	namespace global {
		inline class PresetManager {
			static constexpr uint32_t COLLECTION_ID_ALL = 0;
			static constexpr uint32_t COLLECTION_ID_DEFAULT = 1;
			static constexpr uint32_t COLLECTION_ID_ROOT = 2;
			static constexpr auto PRESET_FILE_NAME = "presets.json";

			std::vector<Preset> presets_;
			std::vector<std::pair<uint32_t, std::string>> collection_info_;
			inline static uint32_t collection_id_counter_ = COLLECTION_ID_ROOT + 1;
			uint32_t current_collection_id_ = COLLECTION_ID_ALL;

		public:
			PresetManager() noexcept;

			struct PresetInfo {
				uint32_t id;
				uint32_t collection_id;
				std::string name;
			};

			template<class CurveClass>
			void create_preset(const CurveClass& curve, const std::string& name) noexcept {
				auto collection_id = current_collection_id_;
				if (collection_id == COLLECTION_ID_ALL or collection_id == COLLECTION_ID_DEFAULT) {
					collection_id = COLLECTION_ID_ROOT;
				}
				Preset preset{collection_id};
				preset.create(curve, name);
				presets_.emplace_back(std::move(preset));
			}
			bool remove_preset(uint32_t id) noexcept;
			void rename_preset(uint32_t id, const std::string& name) noexcept;
			std::string get_preset_name(uint32_t id) const noexcept;
			bool is_preset_default(uint32_t id) const noexcept;
			std::vector<PresetInfo> get_preset_info() const noexcept;

			uint32_t create_collection(const std::string& name, bool change_current_id = true) noexcept;
			bool import_collection(const std::filesystem::path& path) noexcept;
			bool export_collection(uint32_t collection_id, const std::filesystem::path& path) const noexcept;
			bool remove_collection(uint32_t collection_id) noexcept;
			bool rename_collection(uint32_t collection_id, const std::string& name) noexcept;
			bool is_collection_custom(uint32_t collection_id) const noexcept {
				return collection_id != COLLECTION_ID_ALL and collection_id != COLLECTION_ID_DEFAULT and collection_id != COLLECTION_ID_ROOT;
			}
			bool is_collection_custom() const noexcept { return is_collection_custom(current_collection_id_); }
			auto get_current_collection_id() const noexcept { return current_collection_id_; }
			void set_current_collection_id(uint32_t collection_id) noexcept;
			const auto& get_collection_info() const noexcept { return collection_info_; }
			std::string get_collection_name(uint32_t collection_id) const noexcept;

			bool load_json() noexcept;
			bool save_json() const noexcept;
		} preset_manager;
	} // namespace global
} // namespace cved