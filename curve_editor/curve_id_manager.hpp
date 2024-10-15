#pragma once

#include <typeinfo>
#include <vector>



namespace cved {
	class Curve;
	namespace global {
		inline class CurveIdManager {
			inline static uint32_t counter_ = 1u;
			std::vector<std::pair<uint32_t, Curve*>> info_;

		public:
			uint32_t create_id(Curve* curve) noexcept {
				const auto new_id = counter_++;
				info_.emplace_back(new_id, curve);
				return new_id;
			}

			bool remove_id(uint32_t id) noexcept {
				for (auto it = info_.begin(); it != info_.end(); it++) {
					if (std::get<0>(*it) == id) {
						info_.erase(it);
						return true;
					}
				}
				return false;
			}

			template<class CurveClass>
			CurveClass* get_curve(uint32_t id) noexcept {
				for (const auto& [id_, curve] : info_) {
					if (id_ == id) {
						return dynamic_cast<CurveClass*>(curve);
					}
				}
				return nullptr;
			}
		} id_manager;
	} // namespace global
} // namespace cved