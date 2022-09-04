//----------------------------------------------------------------------------------
//		curve editor
//		header file (wndproc)
//		(Visual C++ 2019)
//----------------------------------------------------------------------------------


#include "ce_header.hpp"

#define CE_CT_POS_MODE_VALUE(rc)	CE_MRG, CE_MRG, (rc.right - 2 * CE_MRG) * 0.5, CE_CT_SIDE_H
#define CE_CT_POS_MODE_ID(rc)		rc.right * 0.5, CE_MRG, (rc.right - 2 * CE_MRG) * 0.5, CE_CT_SIDE_H
#define CE_CT_POS_APPLY(rc)			0, CE_CT_SIDE_H, rc.right, CE_CT_APPLY_H
#define CE_CT_POS_SAVE(rc)			CE_MRG, CE_MRG + CE_CT_SIDE_H + CE_CT_APPLY_H, (rc.right - 2 * CE_MRG) * 0.25, CE_CT_SIDE_H
#define CE_CT_POS_READ(rc)			CE_MRG + (rc.right - 2 * CE_MRG) * 0.25, CE_MRG + CE_CT_SIDE_H + CE_CT_APPLY_H, (rc.right - 2 * CE_MRG) * 0.25, CE_CT_SIDE_H
#define CE_CT_POS_ALIGN(rc)			CE_MRG + (rc.right - 2 * CE_MRG) * 0.5, CE_MRG + CE_CT_SIDE_H + CE_CT_APPLY_H, (rc.right - 2 * CE_MRG) * 0.25, CE_CT_SIDE_H
#define CE_CT_POS_PREF(rc)			CE_MRG + (rc.right - 2 * CE_MRG) * 0.75, CE_MRG + CE_CT_SIDE_H + CE_CT_APPLY_H, (rc.right - 2 * CE_MRG) * 0.25, CE_CT_SIDE_H
#define CE_CT_POS_PREV(rc)			CE_MRG, rc.bottom - CE_CT_EDT_H - CE_MRG, (rc.right - 4 * CE_MRG) / 6.5, CE_CT_EDT_H
#define CE_CT_POS_PREV_DUR(rc)		CE_MRG + (rc.right - 4 * CE_MRG) / 6.5, rc.bottom - CE_CT_EDT_H - CE_MRG, CE_MRG + (rc.right - 4 * CE_MRG) * 1.5 / 6.5, CE_CT_EDT_H
#define CE_CT_POS_FIT(rc)			2 * CE_MRG + (rc.right - 4 * CE_MRG) * 2.5 / 6.5, rc.bottom - CE_CT_EDT_H - CE_MRG, (rc.right - 4 * CE_MRG) / 6.5, CE_CT_EDT_H
#define CE_CT_POS_ID_BACK(rc)		3 * CE_MRG + (rc.right - 4 * CE_MRG) * 3.5 / 6.5, rc.bottom - CE_CT_EDT_H - CE_MRG, (rc.right - 4 * CE_MRG) * 0.5 / 6.5, CE_CT_EDT_H
#define CE_CT_POS_ID_NEXT(rc)		3 * CE_MRG + (rc.right - 4 * CE_MRG) * 5 / 6.5, rc.bottom - CE_CT_EDT_H - CE_MRG, (rc.right - 4 * CE_MRG) * 0.5 / 6.5, CE_CT_EDT_H
#define CE_CT_POS_ID(rc)			3 * CE_MRG + (rc.right - 4 * CE_MRG) * 4 / 6.5, rc.bottom - CE_CT_EDT_H - CE_MRG, (rc.right - 4 * CE_MRG) / 6.5, CE_CT_EDT_H
#define CE_CT_POS_DELETE(rc)		3 * CE_MRG + (rc.right - 4 * CE_MRG) * 5.5 / 6.5, rc.bottom - CE_CT_EDT_H - CE_MRG, (rc.right - 4 * CE_MRG) / 6.5, CE_CT_EDT_H
#define CE_CT_POS_SEARCH(rc)		CE_MRG, CE_MRG, rc.right - 2 * CE_MRG, CE_CT_SEARCH_H

#define CE_WD_POS_SIDE(rc)			0, 0, g_config.separator - CE_SEPR_W, rc.bottom
#define CE_WD_POS_EDT(rc)			g_config.separator + CE_SEPR_W, 0, MINLIM(rc.right - g_config.separator - CE_SEPR_W, CE_WD_EDT_MINW), rc.bottom
#define CE_WD_POS_PREV(rc)			CE_MRG, CE_MRG, CE_WD_PREV_W, rc.bottom - CE_CT_EDT_H - 3 * CE_MRG
#define CE_WD_POS_GRAPH(rc)			CE_MRG * 2 + CE_WD_PREV_W, CE_MRG, rc.right - 3 * CE_MRG - CE_WD_PREV_W, rc.bottom - 3 * CE_MRG - CE_CT_EDT_H
#define CE_WD_POS_LIB(rc)			CE_MRG, 2 * (CE_MRG + CE_CT_SIDE_H) + CE_CT_APPLY_H, rc.right - 2 * CE_MRG, rc.bottom - 2 * (CE_MRG + CE_CT_SIDE_H) - CE_CT_APPLY_H - CE_MRG