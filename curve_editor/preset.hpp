#pragma once

#include "curve_base.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>



namespace curve_editor {
	class Preset {
		std::unique_ptr<Curve> p_curve_;
		std::wstring name_;
		std::optional<std::time_t> date_;
		uint32_t collection_id_;

    public:
		Preset(uint32_t collection_id) noexcept :
			collection_id_{ collection_id }
		{}

		template<class CurveType>
		void create(const CurveType& curve, const std::wstring& name) noexcept {
			name_ = name;
			p_curve_ = std::make_unique<CurveType>(curve);
			p_curve_->set_locked(true);
			// TODO: 1(COLLECTION_ID_DEFAULT)がマジックナンバーなので、定数化する
			if (collection_id_ == 1) {
				date_ = std::nullopt;
			}
			else {
				date_ = std::time(nullptr);
			}
		}

		const Curve* get_curve() const noexcept { return p_curve_.get();}
		auto get_id() const noexcept { return p_curve_->get_id();}
		auto get_collection_id() const noexcept { return collection_id_; }

		const auto& get_name() const noexcept { return name_; }
		void set_name(const std::wstring& name) noexcept { name_ = name; }

		const auto& get_date() const noexcept { return date_; }

		nlohmann::json create_json(bool omit_date = false) const noexcept;
		bool load_json(const nlohmann::json& data) noexcept;
	};
} // namespace curve_editor