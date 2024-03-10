#include "string_table.hpp"
#include "resource.h"



namespace cved {
	namespace global {
		// 文字列テーブルの読み込み
		bool StringTable::load(HINSTANCE hinst)
		{
			for (size_t i = 0; i < (size_t)StringId::NumStringId; i++) {
				char tmp[MAX_LEN];
				UINT id;

				switch ((StringId)i) {
				case StringId::ErrorConnectionFailed:
					id = IDS_ERROR_CONNECTION_FAILED;
					break;

				case StringId::ErrorGraphicsInitFailed:
					id = IDS_ERROR_GRAPHICS_INIT_FAILED;
					break;

				case StringId::ErrorExeditNotFound:
					id = IDS_ERROR_EXEDIT_NOT_FOUND;
					break;

				case StringId::ErrorLuaDLLNotFound:
					id = IDS_ERROR_LUA_NOT_FOUND;
					break;

				case StringId::ErrorExeditHookFailed:
					id = IDS_ERROR_EXEDIT_HOOK_FAILED;
					break;

				case StringId::ErrorOutOfRange:
					id = IDS_ERROR_OUT_OF_RANGE;
					break;

				case StringId::ErrorInvalidInput:
					id = IDS_ERROR_INVALID_INPUT;
					break;

				case StringId::ErrorCodeCopyFailed:
					id = IDS_ERROR_CODE_COPY_FAILED;
					break;

				case StringId::WarningDeleteCurve:
					id = IDS_WARNING_DELETE_CURVE;
					break;

				case StringId::WarningDeleteAllCurves:
					id = IDS_WARNING_DELETE_ALL_CURVES;
					break;

				case StringId::InfoLatestVersion:
					id = IDS_INFO_LATEST_VERSION;
					break;

				case StringId::InfoUpdateAvailable:
					id = IDS_INFO_UPDATE_AVAILABLE;
					break;

				case StringId::InfoRestartAviutl:
					id = IDS_INFO_RESTART_AVIUTL;
					break;

				case StringId::LabelEditModeNormal:
					id = IDS_LABEL_EDIT_MODE_NORMAL;
					break;

				case StringId::LabelEditModeValue:
					id = IDS_LABEL_EDIT_MODE_VALUE;
					break;

				case StringId::LabelEditModeBezier:
					id = IDS_LABEL_EDIT_MODE_BEZIER;
					break;

				case StringId::LabelEditModeElastic:
					id = IDS_LABEL_EDIT_MODE_ELASTIC;
					break;

				case StringId::LabelEditModeBounce:
					id = IDS_LABEL_EDIT_MODE_BOUNCE;
					break;

				case StringId::LabelEditModeStep:
					id = IDS_LABEL_EDIT_MODE_STEP;
					break;

				case StringId::LabelEditModeScript:
					id = IDS_LABEL_EDIT_MODE_SCRIPT;
					break;

				case StringId::LabelCurveSegmentTypeLinear:
					id = IDS_LABEL_CURVE_SEGMENT_TYPE_LINEAR;
					break;

				case StringId::LabelTooltipCopy:
					id = IDS_LABEL_TOOLTIP_COPY;
					break;

				case StringId::LabelTooltipRead:
					id = IDS_LABEL_TOOLTIP_READ;
					break;

				case StringId::LabelTooltipSave:
					id = IDS_LABEL_TOOLTIP_SAVE;
					break;

				case StringId::LabelTooltipClear:
					id = IDS_LABEL_TOOLTIP_CLEAR;
					break;

				case StringId::LabelTooltipFit:
					id = IDS_LABEL_TOOLTIP_FIT;
					break;

				case StringId::LabelTooltipMore:
					id = IDS_LABEL_TOOLTIP_MORE;
					break;

				case StringId::LabelTooltipIdBack:
					id = IDS_LABEL_TOOLTIP_ID_BACK;
					break;

				case StringId::LabelTooltipIdNext:
					id = IDS_LABEL_TOOLTIP_ID_NEXT;
					break;

				case StringId::LabelTooltipCurrentId:
					id = IDS_LABEL_TOOLTIP_CURRENT_ID;
					break;

				case StringId::LabelThemeNameDark:
					id = IDS_LABEL_THEME_NAME_DARK;
					break;

				case StringId::LabelThemeNameLight:
					id = IDS_LABEL_THEME_NAME_LIGHT;
					break;

				case StringId::LabelPreferenceGeneral:
					id = IDS_LABEL_PREFERENCE_GENERAL;
					break;

				case StringId::LabelPreferenceAppearance:
					id = IDS_LABEL_PREFERENCE_APPEARANCE;
					break;

				case StringId::LabelPreferenceBehavior:
					id = IDS_LABEL_PREFERENCE_BEHAVIOR;
					break;

				case StringId::LabelPreferenceEditing:
					id = IDS_LABEL_PREFERENCE_EDITING;
					break;

				case StringId::WordVersion:
					id = IDS_WORD_VERSION;
					break;

				case StringId::WordEditMode:
					id = IDS_WORD_EDIT_MODE;
					break;

				case StringId::WordLanguageAutomatic:
					id = IDS_WORD_LANGUAGE_AUTOMATIC;
					break;

				default:
					continue;
				}

				if (!::LoadStringA(hinst, id, tmp, MAX_LEN)) {
					return false;
				}
				string_data[i] = tmp;
			}
			loaded_ = true;
			return true;
		}


		const char* StringTable::operator[] (StringTable::StringId str_id)
		{
			if (str_id == StringId::NumStringId) {
				return nullptr;
			}
			return string_data[(size_t)str_id].c_str();
		}
	}
}