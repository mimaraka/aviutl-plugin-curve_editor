#pragma once

#include <stdint.h>


namespace cved {
	enum class EditMode : uint32_t {
		Normal,
		Value,
		Bezier,
		Elastic,
		Bounce,
		Step,
		Script,
		NumEditMode
	};

	enum class CurveSegmentType : uint32_t {
		Linear,
		Bezier,
		Elastic,
		Bounce,
		Step,
		NumCurveSegmentType
	};

	enum class WindowCommand: uint32_t {
		Update					= 0x0800,
		Copy					= 0x0801,
		Read					= 0x0802,
		Save					= 0x0803,
		Clear					= 0x0804,
		Fit						= 0x0805,
		Reverse					= 0x0806,
		ShowHandle				= 0x0807,
		AlignHandle				= 0x0808,
		ChangeId				= 0x0809,
		Selected				= 0x080a,
		ChangeEditMode			= 0x080b,
		EditModeNormal			= 0x0900,
		EditModeValue			= 0x0901,
		EditModeBezier			= 0x0902,
		EditModeElastic			= 0x0903,
		EditModeBounce			= 0x0904,
		EditModeStep			= 0x0905,
		EditModeScript			= 0x0906,
		CurveSegmentTypeLinear	= 0x0a00,
		CurveSegmentTypeBezier	= 0x0a01,
		CurveSegmentTypeElastic	= 0x0a02,
		CurveSegmentTypeBounce	= 0x0a03,
		CurveSegmentTypeStep	= 0x0a04,
		IdNext					= 0x0b00,
		IdBack					= 0x0b01,
		RedrawAviutl			= 0x0b02,
		Param					= 0x0b03,
		RedrawParam				= 0x0b04,
		MovePreset				= 0x0b15,
		ChangeColor				= 0x0b16,
		SetStatus				= 0x0b17,
		GetExedit				= 0x0b18
	};

	enum class ThemeId {
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
		NumApplyMode
	};

	enum class Language {
		Automatic,
		Japanese,
		English,
		Korean
	};
}