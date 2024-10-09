#pragma once

#include "curve_editor.hpp"
#include <mkaul/host_object.hpp>



namespace cved {
	class EditorHostObject : public mkaul::wv2::HostObject {
	public:
		EditorHostObject() {
			register_member(L"currentIdx", DispatchType::PropertyGet, +[] { return global::editor.current_idx(); });
			register_member(L"isIdxFirst", DispatchType::PropertyGet, +[] { return global::editor.is_idx_first(); });
			register_member(L"isIdxLast", DispatchType::PropertyGet, +[] { return global::editor.is_idx_last(); });
			register_member(L"advanceIdx", DispatchType::Method, +[](int idx) { return global::editor.advance_idx(idx); });
		}
	};
} // namespace cved