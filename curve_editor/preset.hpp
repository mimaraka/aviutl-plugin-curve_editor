#pragma once

#include "curve_base.hpp"
#include <memory>
#include <nlohmann/json.hpp>



namespace cved {
	class Preset {
    private:
		std::unique_ptr<Curve> p_curve_;
		std::string name_;
		uint32_t collection_id_;

    public:
		Preset(uint32_t collection_id) noexcept :
			collection_id_{ collection_id }
		{}

		template<class CurveType>
		void create(const CurveType& curve, const std::string& name) noexcept {
			name_ = name;
			p_curve_ = std::make_unique<CurveType>(curve);
		}

		const Curve* get_curve() const noexcept { return p_curve_.get();}
		auto get_id() const noexcept { return p_curve_->get_id();}
		auto get_collection_id() const noexcept { return collection_id_; }

		const auto& get_name() const noexcept { return name_; }
		void set_name(const std::string& name) noexcept { name_ = name; }

		nlohmann::json create_json() const noexcept;
		bool load_json(const nlohmann::json& data) noexcept;
	};
} // namespace cved