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
		ChangeId					= 0x0809,
		ChangeApplyMode				= 0x0900,
		CurveSegmentTypeLinear		= 0x0a00,
		CurveSegmentTypeBezier		= 0x0a01,
		CurveSegmentTypeElastic		= 0x0a02,
		CurveSegmentTypeBounce		= 0x0a03,
		RedrawAviutl				= 0x0b02,
		MovePreset					= 0x0b15,
		LoadConfig					= 0x0b18,
		SaveConfig					= 0x0b19,
		StartDnd					= 0x0b1f,
		SetCurveInfo				= 0x0b20
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