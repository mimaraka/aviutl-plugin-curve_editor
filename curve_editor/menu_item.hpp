#pragma once

#include <functional>
#include <string>
#include <Windows.h>



namespace cved {
	class MenuItem {
	public:
		static constexpr size_t MAX_SUB_MENU_ITEMS = 128;

		enum class Type : uint32_t {
			String = 0x0L,
			Bitmap = 0x4L,
			MenuBarBreak = 0x20L,
			OwnerDraw = 0x100L,
			RadioCheck = 0x200L,
			Separator = 0x800L,
			RightOrder = 0x2000L,
			RightJustify = 0x4000L
		};

		enum class State : uint32_t {
			Null = 0x0L,
			Disabled = 0x3L,
			Checked = 0x8L,
			Hilite = 0x80L,
			Default = 0x1000L
		};

	private:
		std::string label_;
		Type type_;
		State state_;
		std::function<void()> callback_;
		std::vector<MenuItem> sub_menu_items_;
		HMENU sub_menu_ = nullptr;
		int32_t id_ = 0;

	public:
		MenuItem(
			const std::string& label,
			Type type = Type::String,
			State state = State::Null,
			std::function<void()> callback = nullptr,
			std::vector<MenuItem> sub_menu_items = {}
		) :
			label_{ label },
			type_{ type },
			state_{ state },
			callback_{ callback },
			sub_menu_items_{ sub_menu_items }
		{}

		~MenuItem() {
			if (sub_menu_) {
				::DestroyMenu(sub_menu_);
			}
		}

		const auto& label() const noexcept { return label_; }
		auto type() const noexcept { return type_; }
		auto state() const noexcept { return state_; }
		void set_state(State state) noexcept { state_ = state; }
		void set_label(const std::string& label) noexcept { label_ = label; }
		bool callback(uint32_t id) const noexcept;
		void get_menu_item_info(MENUITEMINFOA& minfo, uint32_t id) noexcept;
	};
}