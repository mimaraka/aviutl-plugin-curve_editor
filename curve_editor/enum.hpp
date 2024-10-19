#pragma once

#include <stdint.h>


namespace cved {
	enum class EditMode : uint32_t {
		Normal,
		Value,
		Bezier,
		Elastic,
		Bounce,
		Script,
		NumEditMode
	};

	enum class CurveSegmentType : uint32_t {
		Linear,
		Bezier,
		Elastic,
		Bounce,
		NumCurveSegmentType
	};

	enum class IDCurveType : uint32_t {
		Normal,
		Value,
		Script
	};

	enum class ModifierType : uint32_t {
		Discretization,
		Noise,
		SineWave,
		SquareWave,
		NumModifierType
	};

	enum class WindowCommand: uint32_t {
		Update						= 0x0800,
		ChangeId					= 0x0801,
		ChangeApplyMode				= 0x0802,
		CurveSegmentTypeLinear		= 0x0803,
		CurveSegmentTypeBezier		= 0x0804,
		CurveSegmentTypeElastic		= 0x0805,
		CurveSegmentTypeBounce		= 0x0806,
		RedrawAviutl				= 0x0807,
		MovePreset					= 0x0808,
		LoadConfig					= 0x0809,
		SaveConfig					= 0x080a,
		StartDnd					= 0x080b,
		SetCurveInfo				= 0x080c,
		SelectCurveOk				= 0x080d,
		SelectCurveCancel			= 0x080e,
		SelectCurveClose			= 0x080f,
	};

	enum class ThemeId {
		System,
		Dark,
		Light,
		NumThemeId
	};

	enum class LayoutMode {
		Vertical,
		Horizontal
	};

	enum class ApplyMode {
		Normal,
		IgnoreMidPoint,
		Interpolate,
		NumApplyMode
	};

	enum class Language {
		Automatic,
		Japanese,
		English,
		Korean
	};
}