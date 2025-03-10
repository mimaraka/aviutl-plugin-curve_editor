#pragma once

#include <cereal/cereal.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/string.hpp>
#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <strconv2.h>



namespace curve_editor {
	using CurveFunction = std::function<double(double, double, double)>;
	class GraphCurve;

	class Modifier {
		std::wstring name_;
		bool enabled_ = true;

	protected:
		GraphCurve* p_curve_ = nullptr;

	public:
		Modifier(const std::wstring& name = L"") noexcept : name_{ name } {}
		Modifier(const Modifier& modifier) noexcept :
			name_{ modifier.name_ },
			enabled_{ modifier.enabled_ },
			p_curve_{ modifier.p_curve_ }
		{}
		virtual ~Modifier() = default;

		virtual std::unique_ptr<Modifier> clone() const noexcept = 0;

		constexpr virtual const std::string_view& get_type() const noexcept = 0;

		auto name() const noexcept { return name_; }
		void set_name(const std::wstring& name) noexcept { name_ = name; }

		auto enabled() const noexcept { return enabled_; }
		void set_enabled(bool enabled) noexcept { enabled_ = enabled; }

		auto curve() const noexcept { return p_curve_; }
		void set_curve(GraphCurve* p_curve) noexcept { p_curve_ = p_curve; }

		virtual CurveFunction apply(const CurveFunction& function) const noexcept = 0;

		virtual nlohmann::json create_json() const noexcept;
		virtual bool load_json(const nlohmann::json& data) noexcept = 0;

		template <class Archive>
		void save(Archive& archive, const std::uint32_t) const {
			archive(
				name_,
				enabled_
			);
		}

		template <class Archive>
		void load(Archive& archive, const std::uint32_t version) {
			if (version < 1) {
				std::string name;
				archive(
					name,
					enabled_
				);
				name_ = ::sjis_to_wide(name);
			}
			else {
				archive(
					name_,
					enabled_
				);
			}
		}
	};
} // namespace curve_editor

CEREAL_CLASS_VERSION(curve_editor::Modifier, 1)