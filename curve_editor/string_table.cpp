#include "constants.hpp"
#include "resource.h"
#include "string_table.hpp"



namespace cved::global {
		// 文字列テーブルの読み込み
		StringTable::StringTable() noexcept {
			// TODO: ロケールの設定
			// ここに書く

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

				case StringId::ErrorDataLoadFailed:
					id = IDS_ERROR_DATA_LOAD_FAILED;
					break;

				case StringId::ErrorWebView2InitFailed:
					id = IDS_ERROR_WEBVIEW2_INIT_FAILED;
					break;

				case StringId::ErrorCommCtrlInitFailed:
					id = IDS_ERROR_COMMCTRL_INIT_FAILED;
					break;

				case StringId::ErrorPageLoadFailed:
					id = IDS_ERROR_PAGE_LOAD_FAILED;
					break;

				case StringId::ErrorPresetCreateFailed:
					id = IDS_ERROR_PRESET_CREATE_FAILED;
					break;

				case StringId::WarningDeleteCurve:
					id = IDS_WARNING_DELETE_CURVE;
					break;

				case StringId::WarningDeleteAllCurves:
					id = IDS_WARNING_DELETE_ALL_CURVES;
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

				case StringId::LabelEditModeScript:
					id = IDS_LABEL_EDIT_MODE_SCRIPT;
					break;

				case StringId::LabelCurveSegmentTypeLinear:
					id = IDS_LABEL_CURVE_SEGMENT_TYPE_LINEAR;
					break;

				case StringId::LabelApplyModeNormal:
					id = IDS_LABEL_APPLY_MODE_NORMAL;
					break;

				case StringId::LabelApplyModeIgnoreMidPoint:
					id = IDS_LABEL_APPLY_MODE_IGNORE_MIDPOINT;
					break;

				case StringId::LabelApplyModeInterpolate:
					id = IDS_LABEL_APPLY_MODE_INTERPOLATE;
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

				case StringId::LabelThemeNameSystem:
					id = IDS_LABEL_THEME_NAME_SYSTEM;
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

				case StringId::LabelSelectBackgroundImage:
					id = IDS_LABEL_SELECT_BG_IMAGE;
					break;

				case StringId::LabelSelectCurve:
					id = IDS_LABEL_SELECT_CURVE;
					break;

				case StringId::LabelModifierDiscretization:
					id = IDS_LABEL_MODIFIER_DISCRETIZATION;
					break;

				case StringId::LabelModifierNoise:
					id = IDS_LABEL_MODIFIER_NOISE;
					break;

				case StringId::LabelModifierSineWave:
					id = IDS_LABEL_MODIFIER_SINE_WAVE;
					break;

				case StringId::LabelModifierSquareWave:
					id = IDS_LABEL_MODIFIER_SQUARE_WAVE;
					break;

				case StringId::LabelCollectionNameAll:
					id = IDS_LABEL_COLLECTION_NAME_ALL;
					break;

				case StringId::LabelCollectionNameDefault:
					id = IDS_LABEL_COLLECTION_NAME_DEFAULT;
					break;

				case StringId::LabelCollectionNameRoot:
					id = IDS_LABEL_COLLECTION_NAME_ROOT;
					break;

				case StringId::PromptCreatePreset:
					id = IDS_PROMPT_CREATE_PRESET;
					break;

				case StringId::PromptCurveCode:
					id = IDS_PROMPT_CURVE_CODE;
					break;

				case StringId::PromptCurveParam:
					id = IDS_PROMPT_CURVE_PARAM;
					break;

				case StringId::PromptRenamePreset:
					id = IDS_PROMPT_RENAME_PRESET;
					break;

				case StringId::PromptCreateCollection:
					id = IDS_PROMPT_CREATE_COLLECTION;
					break;

				case StringId::PromptRenameCollection:
					id = IDS_PROMPT_RENAME_COLLECTION;
					break;

				case StringId::CaptionCreatePreset:
					id = IDS_CAPTION_CREATE_PRESET;
					break;

				case StringId::CaptionCurveCode:
					id = IDS_CAPTION_CURVE_CODE;
					break;

				case StringId::CaptionCurveParam:
					id = IDS_CAPTION_CURVE_PARAM;
					break;

				case StringId::CaptionRenamePreset:
					id = IDS_CAPTION_RENAME_PRESET;
					break;

				case StringId::CaptionCreateCollection:
					id = IDS_CAPTION_CREATE_COLLECTION;
					break;

				case StringId::CaptionRenameCollection:
					id = IDS_CAPTION_RENAME_COLLECTION;
					break;

				case StringId::CaptionImportCollection:
					id = IDS_CAPTION_IMPORT_COLLECTION;
					break;

				case StringId::CaptionExportCollection:
					id = IDS_CAPTION_EXPORT_COLLECTION;
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

				case StringId::WordImageFiles:
					id = IDS_WORD_IMAGE_FILES;
					break;

				case StringId::WordSelect:
					id = IDS_WORD_SELECT;
					break;

				case StringId::WordCollectionFile:
					id = IDS_WORD_COLLECTION_FILE;
					break;

				case StringId::MenuGraphApplyMode:
					id = IDS_MENU_GRAPH_APPLY_MODE;
					break;

				case StringId::MenuGraphAddAnchor:
					id = IDS_MENU_GRAPH_ADD_ANCHOR;
					break;

				case StringId::MenuGraphReverseCurve:
					id = IDS_MENU_GRAPH_REVERSE_CURVE;
					break;

				case StringId::MenuGraphModifier:
					id = IDS_MENU_GRAPH_MODIFIER;
					break;

				case StringId::MenuGraphAlignHandle:
					id = IDS_MENU_GRAPH_ALIGN_HANDLE;
					break;

				case StringId::MenuGraphShowXLabel:
					id = IDS_MENU_GRAPH_SHOW_X_LABEL;
					break;

				case StringId::MenuGraphShowYLabel:
					id = IDS_MENU_GRAPH_SHOW_Y_LABEL;
					break;

				case StringId::MenuGraphShowHandle:
					id = IDS_MENU_GRAPH_SHOW_HANDLE;
					break;

				case StringId::MenuGraphShowVelocityGraph:
					id = IDS_MENU_GRAPH_SHOW_VELOCITY_GRAPH;
					break;

				case StringId::MenuCurveSegmentType:
					id = IDS_MENU_CURVE_SEGMENT_TYPE;
					break;

				case StringId::MenuCurveSegmentReverse:
					id = IDS_MENU_CURVE_SEGMENT_REVERSE;
					break;

				case StringId::MenuCurveSegmentModifier:
					id = IDS_MENU_CURVE_SEGMENT_MODIFIER;
					break;

				case StringId::MenuOthersWindowLayout:
					id = IDS_MENU_OTHERS_WINDOW_LAYOUT;
					break;

				case StringId::MenuOthersWindowLayoutVertical:
					id = IDS_MENU_OTHERS_WINDOW_LAYOUT_VERTICAL;
					break;

				case StringId::MenuOthersWindowLayoutHorizontal:
					id = IDS_MENU_OTHERS_WINDOW_LAYOUT_HORIZONTAL;
					break;

				case StringId::MenuOthersExtension:
					id = IDS_MENU_OTHERS_EXTENSION;
					break;

				case StringId::MenuOthersExtensionInstall:
					id = IDS_MENU_OTHERS_EXTENSION_INSTALL;
					break;

				case StringId::MenuOthersReloadWindow:
					id = IDS_MENU_OTHERS_RELOAD_WINDOW;
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

				case StringId::MenuOthersUpdateAvailable:
					id = IDS_MENU_OTHERS_UPDATE_AVAILABLE;
					break;

				case StringId::MenuPresetItemRename:
					id = IDS_MENU_PRESET_ITEM_RENAME;
					break;

				case StringId::MenuPresetItemRemove:
					id = IDS_MENU_PRESET_ITEM_REMOVE;
					break;

				case StringId::MenuCollectionAddNew:
					id = IDS_MENU_COLLECTION_ADD_NEW;
					break;

				case StringId::MenuCollectionAddImport:
					id = IDS_MENU_COLLECTION_ADD_IMPORT;
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

				case StringId::MenuBezierHandleRoot:
					id = IDS_MENU_BEZIER_HANDLE_ROOT;
					break;

				case StringId::MenuBezierHandleAdjustAngle:
					id = IDS_MENU_BEZIER_HANDLE_ADJUST_ANGLE;
					break;

				case StringId::MenuIdxJumpToFirst:
					id = IDS_MENU_IDX_JUMP_TO_FIRST;
					break;

				case StringId::MenuIdxJumpToLast:
					id = IDS_MENU_IDX_JUMP_TO_LAST;
					break;

				case StringId::MenuIdxDelete:
					id = IDS_MENU_IDX_DELETE;
					break;

				case StringId::MenuIdxDeleteAll:
					id = IDS_MENU_IDX_DELETE_ALL;
					break;

				default:
					continue;
				}

				if (::LoadStringA(::GetModuleHandleA(global::PLUGIN_DLL_NAME), id, tmp, MAX_LEN)) {
					string_data[i] = tmp;
				}
			}
		}


		const char* StringTable::operator[] (StringTable::StringId str_id)
		{
			if ((uint32_t)StringId::NumStringId <= (uint32_t)str_id) {
				return nullptr;
			}
			return string_data[(size_t)str_id].c_str();
		}
} // namespace cved::global