#pragma once

#include "curve_base.hpp"
#include <memory>
#include <nlohmann/json.hpp>



namespace cved {
	class Preset {
    private:
        inline static size_t n_presets = 0u;
		std::unique_ptr<Curve> p_curve_;
		std::string name_;
        size_t id_;

        static size_t register_preset();

    public:
		Preset();

		template<class CurveType>
		void create(const CurveType& p_curve, const std::string& name) noexcept {
			name_ = name;
			p_curve_ = std::make_unique<CurveType>(p_curve);
		}

		nlohmann::json create_json() const noexcept;
		bool load_json(const nlohmann::json& data) noexcept;
	};
}