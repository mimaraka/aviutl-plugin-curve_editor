#include "string_table.hpp"

#include <magic_enum/magic_enum.hpp>
#include <strconv2.h>

#include "config.hpp"
#include "resource.h"
#include "util.hpp"


namespace curve_editor::global {
	// 文字列テーブルの読み込み
	StringTable::StringTable() noexcept {
		constexpr std::array<LANGID, static_cast<size_t>(Language::NumLanguage)> lang_ids = {
			MAKELANGID(LANG_SYSTEM_DEFAULT, SUBLANG_SYS_DEFAULT),
			MAKELANGID(LANG_JAPANESE, SUBLANG_JAPANESE_JAPAN),
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
			MAKELANGID(LANG_GERMAN, SUBLANG_GERMAN),
			MAKELANGID(LANG_INDONESIAN, SUBLANG_INDONESIAN_INDONESIA),
			MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN),
			MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED),
		};

		// TODO: config, string_tableの初期化順序が怪しい(Debugで確認したら一応config->string_tableだったが...)
		::SetThreadLocale(MAKELCID(lang_ids[static_cast<size_t>(config.get_language())], SORT_DEFAULT));
		::SetThreadUILanguage(lang_ids[static_cast<size_t>(config.get_language())]);

		for (size_t i = 0; i < (size_t)StringId::NumStringId; i++) {
			wchar_t tmp[MAX_LEN];
			UINT id;

			switch ((StringId)i) {
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

			case StringId::ErrorDataLoadFailed:
				id = IDS_ERROR_DATA_LOAD_FAILED;
				break;

			case StringId::ErrorWebView2InitFailed:
				id = IDS_ERROR_WEBVIEW2_INIT_FAILED;
				break;

			case StringId::ErrorCommCtrlInitFailed:
				id = IDS_ERROR_COMMCTRL_INIT_FAILED;
				break;

			case StringId::ErrorUILoadFailed:
				id = IDS_ERROR_UI_LOAD_FAILED;
				break;

			case StringId::ErrorPresetCreateFailed:
				id = IDS_ERROR_PRESET_CREATE_FAILED;
				break;

			case StringId::ErrorCollectionImportFailed:
				id = IDS_ERROR_COLLECTION_IMPORT_FAILED;
				break;

			case StringId::WarningResetCurve:
				id = IDS_WARNING_RESET_CURVE;
				break;

			case StringId::WarningResetAllCurves:
				id = IDS_WARNING_RESET_ALL_CURVES;
				break;

			case StringId::WarningResetPreferences:
				id = IDS_WARNING_RESET_PREFERENCES;
				break;

			case StringId::WarningDeleteId:
				id = IDS_WARNING_DELETE_ID;
				break;

			case StringId::WarningDeleteAllIds:
				id = IDS_WARNING_DELETE_ALL_IDS;
				break;

			case StringId::WarningRemoveModifier:
				id = IDS_WARNING_REMOVE_MODIFIER;
				break;

			case StringId::WarningRemovePreset:
				id = IDS_WARNING_REMOVE_PRESET;
				break;

			case StringId::WarningRemoveCollection:
				id = IDS_WARNING_REMOVE_COLLECTION;
				break;

			case StringId::InfoRestartAviutl:
				id = IDS_INFO_RESTART_AVIUTL;
				break;

			case StringId::CurveTypeNormal:
				id = IDS_CURVE_TYPE_NORMAL;
				break;

			case StringId::CurveTypeValue:
				id = IDS_CURVE_TYPE_VALUE;
				break;

			case StringId::CurveTypeBezier:
				id = IDS_CURVE_TYPE_BEZIER;
				break;

			case StringId::CurveTypeElastic:
				id = IDS_CURVE_TYPE_ELASTIC;
				break;

			case StringId::CurveTypeBounce:
				id = IDS_CURVE_TYPE_BOUNCE;
				break;

			case StringId::CurveTypeScript:
				id = IDS_CURVE_TYPE_SCRIPT;
				break;

			case StringId::CurveTypeLinear:
				id = IDS_CURVE_TYPE_LINEAR;
				break;

			case StringId::ApplyModeNormal:
				id = IDS_APPLY_MODE_NORMAL;
				break;

			case StringId::ApplyModeIgnoreMidPoint:
				id = IDS_APPLY_MODE_IGNORE_MID_POINT;
				break;

			case StringId::ApplyModeInterpolate:
				id = IDS_APPLY_MODE_INTERPOLATE;
				break;

			case StringId::TooltipButtonApply:
				id = IDS_TOOLTIP_BUTTON_APPLY;
				break;

			case StringId::TooltipButtonCopy:
				id = IDS_TOOLTIP_BUTTON_COPY;
				break;

			case StringId::TooltipButtonRead:
				id = IDS_TOOLTIP_BUTTON_READ;
				break;

			case StringId::TooltipButtonSave:
				id = IDS_TOOLTIP_BUTTON_SAVE;
				break;

			case StringId::TooltipButtonUnlocked:
				id = IDS_TOOLTIP_BUTTON_UNLOCKED;
				break;

			case StringId::TooltipButtonLocked:
				id = IDS_TOOLTIP_BUTTON_LOCKED;
				break;

			case StringId::TooltipButtonReset:
				id = IDS_TOOLTIP_BUTTON_RESET;
				break;

			case StringId::TooltipButtonFit:
				id = IDS_TOOLTIP_BUTTON_FIT;
				break;

			case StringId::TooltipButtonOthers:
				id = IDS_TOOLTIP_BUTTON_OTHERS;
				break;

			case StringId::TooltipButtonIdBack:
				id = IDS_TOOLTIP_BUTTON_ID_BACK;
				break;

			case StringId::TooltipButtonIdNext:
				id = IDS_TOOLTIP_BUTTON_ID_NEXT;
				break;

			case StringId::TooltipButtonIdNew:
				id = IDS_TOOLTIP_BUTTON_ID_NEW;
				break;

			case StringId::TooltipButtonCurrentId:
				id = IDS_TOOLTIP_BUTTON_CURRENT_ID;
				break;

			case StringId::TooltipButtonParam:
				id = IDS_TOOLTIP_BUTTON_PARAM;
				break;

			case StringId::TooltipButtonListConfig:
				id = IDS_TOOLTIP_BUTTON_LIST_CONFIG;
				break;

			case StringId::TooltipButtonCollectionAdd:
				id = IDS_TOOLTIP_BUTTON_COLLECTION_ADD;
				break;

			case StringId::TooltipButtonCollectionEdit:
				id = IDS_TOOLTIP_BUTTON_COLLECTION_EDIT;
				break;

			case StringId::TooltipButtonShowHandle:
				id = IDS_TOOLTIP_BUTTON_SHOW_HANDLE;
				break;

			case StringId::TooltipCreatedAt:
				id = IDS_TOOLTIP_CREATED_AT;
				break;

			case StringId::ThemeSystem:
				id = IDS_THEME_SYSTEM;
				break;

			case StringId::ThemeDark:
				id = IDS_THEME_DARK;
				break;

			case StringId::ThemeLight:
				id = IDS_THEME_LIGHT;
				break;

			case StringId::PreferencesCategoryGeneral:
				id = IDS_PREFERENCES_CATEGORY_GENERAL;
				break;

			case StringId::PreferencesCategoryAppearance:
				id = IDS_PREFERENCES_CATEGORY_APPEARANCE;
				break;

			case StringId::PreferencesCategoryBehavior:
				id = IDS_PREFERENCES_CATEGORY_BEHAVIOR;
				break;

			case StringId::PreferencesCategoryEditing:
				id = IDS_PREFERENCES_CATEGORY_EDITING;
				break;

			case StringId::CaptionSelectBackgroundImage:
				id = IDS_CAPTION_SELECT_BG_IMAGE;
				break;

			case StringId::CaptionSelectCurve:
				id = IDS_CAPTION_SELECT_CURVE;
				break;

			case StringId::ModifierTypeDiscretization:
				id = IDS_MODIFIER_TYPE_DISCRETIZATION;
				break;

			case StringId::ModifierTypeNoise:
				id = IDS_MODIFIER_TYPE_NOISE;
				break;

			case StringId::ModifierTypeSineWave:
				id = IDS_MODIFIER_TYPE_SINE_WAVE;
				break;

			case StringId::ModifierTypeSquareWave:
				id = IDS_MODIFIER_TYPE_SQUARE_WAVE;
				break;

			case StringId::SortByNone:
				id = IDS_SORT_BY_NONE;
				break;

			case StringId::SortByName:
				id = IDS_SORT_BY_NAME;
				break;

			case StringId::SortByDate:
				id = IDS_SORT_BY_DATE;
				break;

			case StringId::SortOrderAsc:
				id = IDS_SORT_ORDER_ASC;
				break;

			case StringId::SortOrderDesc:
				id = IDS_SORT_ORDER_DESC;
				break;

			case StringId::LabelCollectionOmitDate:
				id = IDS_LABEL_COLLECTION_OMIT_DATE;
				break;

			case StringId::LabelCollectionSetIndent:
				id = IDS_LABEL_COLLECTION_SET_INDENT;
				break;

			case StringId::LabelLoading:
				id = IDS_LABEL_LOADING;
				break;

			case StringId::LabelSearchPresets:
				id = IDS_LABEL_SEARCH_PRESETS;
				break;

			case StringId::LabelApplyDnD:
				id = IDS_LABEL_APPLY_DND;
				break;

			case StringId::LabelReset:
				id = IDS_LABEL_RESET;
				break;

			case StringId::LabelNoPresets:
				id = IDS_LABEL_NO_PRESETS;
				break;

			case StringId::CollectionNameAll:
				id = IDS_COLLECTION_NAME_ALL;
				break;

			case StringId::CollectionNameDefault:
				id = IDS_COLLECTION_NAME_DEFAULT;
				break;

			case StringId::CollectionNameUser:
				id = IDS_COLLECTION_NAME_USER;
				break;

			case StringId::PromptPresetName:
				id = IDS_PROMPT_PRESET_NAME;
				break;

			case StringId::PromptCurveCode:
				id = IDS_PROMPT_CURVE_CODE;
				break;

			case StringId::PromptCurveParam:
				id = IDS_PROMPT_CURVE_PARAM;
				break;

			case StringId::PromptCollectionName:
				id = IDS_PROMPT_COLLECTION_NAME;
				break;

			case StringId::CaptionPresetCreate:
				id = IDS_CAPTION_PRESET_CREATE;
				break;

			case StringId::CaptionCurveCode:
				id = IDS_CAPTION_CURVE_CODE;
				break;

			case StringId::CaptionCurveParam:
				id = IDS_CAPTION_CURVE_PARAM;
				break;

			case StringId::CaptionPresetRename:
				id = IDS_CAPTION_PRESET_RENAME;
				break;

			case StringId::CaptionCollectionCreate:
				id = IDS_CAPTION_COLLECTION_CREATE;
				break;

			case StringId::CaptionCollectionRename:
				id = IDS_CAPTION_COLLECTION_RENAME;
				break;

			case StringId::CaptionCollectionImport:
				id = IDS_CAPTION_COLLECTION_IMPORT;
				break;

			case StringId::CaptionCollectionExport:
				id = IDS_CAPTION_COLLECTION_EXPORT;
				break;

			case StringId::WordVersion:
				id = IDS_WORD_VERSION;
				break;

			case StringId::WordEditMode:
				id = IDS_WORD_EDIT_MODE;
				break;

			case StringId::WordAutomatic:
				id = IDS_WORD_AUTOMATIC;
				break;

			case StringId::WordImageFile:
				id = IDS_WORD_IMAGE_FILE;
				break;

			case StringId::WordSelect:
				id = IDS_WORD_SELECT;
				break;

			case StringId::WordCollection:
				id = IDS_WORD_COLLECTION;
				break;

			case StringId::WordCollectionFile:
				id = IDS_WORD_COLLECTION_FILE;
				break;

			case StringId::WordType:
				id = IDS_WORD_TYPE;
				break;

			case StringId::WordApply:
				id = IDS_WORD_APPLY;
				break;

			case StringId::WordOK:
				id = IDS_WORD_OK;
				break;

			case StringId::WordCancel:
				id = IDS_WORD_CANCEL;
				break;

			case StringId::MenuEditorCurveAddAnchor:
				id = IDS_MENU_EDITOR_CURVE_ADD_ANCHOR;
				break;

			case StringId::MenuEditorCurveReverse:
				id = IDS_MENU_EDITOR_CURVE_REVERSE;
				break;

			case StringId::MenuEditorCurveModifier:
				id = IDS_MENU_EDITOR_CURVE_MODIFIER;
				break;

			case StringId::MenuEditorCurveCopy:
				id = IDS_MENU_EDITOR_CURVE_COPY;
				break;

			case StringId::MenuEditorCurvePaste:
				id = IDS_MENU_EDITOR_CURVE_PASTE;
				break;

			case StringId::MenuEditorCurveAlignHandle:
				id = IDS_MENU_EDITOR_CURVE_ALIGN_HANDLE;
				break;

			case StringId::MenuEditorCurveShowScaleX:
				id = IDS_MENU_EDITOR_CURVE_SHOW_SCALE_X;
				break;

			case StringId::MenuEditorCurveShowScaleY:
				id = IDS_MENU_EDITOR_CURVE_SHOW_SCALE_Y;
				break;

			case StringId::MenuEditorCurveShowHandle:
				id = IDS_MENU_EDITOR_CURVE_SHOW_HANDLE;
				break;

			case StringId::MenuEditorCurveShowVelocityGraph:
				id = IDS_MENU_EDITOR_CURVE_SHOW_VELOCITY_GRAPH;
				break;

			case StringId::MenuEditorSegmentType:
				id = IDS_MENU_EDITOR_SEGMENT_TYPE;
				break;

			case StringId::MenuEditorSegmentReverse:
				id = IDS_MENU_EDITOR_SEGMENT_REVERSE;
				break;

			case StringId::MenuEditorSegmentModifier:
				id = IDS_MENU_EDITOR_SEGMENT_MODIFIER;
				break;

			case StringId::MenuOthersPanelLayout:
				id = IDS_MENU_OTHERS_PANEL_LAYOUT;
				break;

			case StringId::MenuOthersPanelLayoutVertical:
				id = IDS_MENU_OTHERS_PANEL_LAYOUT_VERTICAL;
				break;

			case StringId::MenuOthersPanelLayoutHorizontal:
				id = IDS_MENU_OTHERS_PANEL_LAYOUT_HORIZONTAL;
				break;

			case StringId::MenuOthersExtension:
				id = IDS_MENU_OTHERS_EXTENSION;
				break;

			case StringId::MenuOthersExtensionManage:
				id = IDS_MENU_OTHERS_EXTENSION_MANAGE;
				break;

			case StringId::MenuOthersReloadUI:
				id = IDS_MENU_OTHERS_RELOAD_UI;
				break;

			case StringId::MenuOthersPreferences:
				id = IDS_MENU_OTHERS_PREFERENCES;
				break;

			case StringId::MenuOthersHelp:
				id = IDS_MENU_OTHERS_HELP;
				break;

			case StringId::MenuOthersAbout:
				id = IDS_MENU_OTHERS_ABOUT;
				break;

			case StringId::MenuOthersNotificationUpdateAvailable:
				id = IDS_MENU_OTHERS_NOTIFICATION_UPDATE_AVAILABLE;
				break;

			case StringId::MenuPresetItemRename:
				id = IDS_MENU_PRESET_ITEM_RENAME;
				break;

			case StringId::MenuPresetItemRemove:
				id = IDS_MENU_PRESET_ITEM_REMOVE;
				break;

			case StringId::MenuPresetSimpleView:
				id = IDS_MENU_PRESET_SIMPLE_VIEW;
				break;

			case StringId::MenuCollectionAddNew:
				id = IDS_MENU_COLLECTION_ADD_NEW;
				break;

			case StringId::MenuCollectionAddImport:
				id = IDS_MENU_COLLECTION_ADD_IMPORT;
				break;

			case StringId::MenuCollectionAddImportCecl:
				id = IDS_MENU_COLLECTION_ADD_IMPORT_CECL;
				break;

			case StringId::MenuCollectionAddImportFlow:
				id = IDS_MENU_COLLECTION_ADD_IMPORT_FLOW;
				break;

			case StringId::MenuCollectionRename:
				id = IDS_MENU_COLLECTION_RENAME;
				break;

			case StringId::MenuCollectionRemove:
				id = IDS_MENU_COLLECTION_REMOVE;
				break;

			case StringId::MenuCollectionExport:
				id = IDS_MENU_COLLECTION_EXPORT;
				break;

			case StringId::MenuEditorHandleBezierRoot:
				id = IDS_MENU_EDITOR_HANDLE_BEZIER_ROOT;
				break;

			case StringId::MenuEditorHandleBezierMatchAngle:
				id = IDS_MENU_EDITOR_HANDLE_BEZIER_MATCH_ANGLE;
				break;

			case StringId::MenuIDJumpToFirst:
				id = IDS_MENU_ID_JUMP_TO_FIRST;
				break;

			case StringId::MenuIDJumpToLast:
				id = IDS_MENU_ID_JUMP_TO_LAST;
				break;

			case StringId::MenuIDDelete:
				id = IDS_MENU_ID_DELETE;
				break;

			case StringId::MenuIDDeleteAll:
				id = IDS_MENU_ID_DELETE_ALL;
				break;

			default:
				continue;
			}

			if (::LoadStringW(util::get_hinst(), id, tmp, MAX_LEN)) {
				string_data[i] = tmp;
			}
		}
	}


	const wchar_t* StringTable::operator[] (StringTable::StringId str_id) {
		if ((uint32_t)StringId::NumStringId <= (uint32_t)str_id) {
			return nullptr;
		}
		return string_data[(size_t)str_id].c_str();
	}


	nlohmann::json StringTable::create_json() const {
		nlohmann::json json;
		for (size_t i = 0; i < (size_t)StringId::NumStringId; i++) {
			const auto string_id = static_cast<StringId>(i);
			const auto key = magic_enum::enum_name(string_id);
			json[key] = ::wide_to_utf8(string_data[i]);
		}
		return json;
	}
} // namespace curve_editor::global