#pragma once

#include "curve_editor.hpp"
#include <mkaul/host_object.hpp>



namespace cved {
	class EditorHostObject : public mkaul::wv2::HostObject {
	public:
		EditorHostObject() {
			register_member(L"currentIdx", DispatchType::PropertyGet, +[] { return global::editor.current_idx(); });
		}
	};
} // namespace cved