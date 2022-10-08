//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル (ce_wndproc.cpp用)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



#define CE_CT_POS_MODE_VALUE(rc)	CE_MARGIN, CE_MARGIN, (rc.right - 2 * CE_MARGIN) * 0.5, CE_CT_SIDE_H
#define CE_CT_POS_MODE_ID(rc)		rc.right * 0.5, CE_MARGIN, (rc.right - 2 * CE_MARGIN) * 0.5, CE_CT_SIDE_H
#define CE_CT_POS_APPLY(rc)			0, CE_CT_SIDE_H, rc.right, CE_CT_APPLY_H
#define CE_CT_POS_SAVE(rc)			CE_MARGIN, CE_MARGIN + CE_CT_SIDE_H + CE_CT_APPLY_H, (rc.right - 2 * CE_MARGIN) * 0.25, CE_CT_SIDE_H
#define CE_CT_POS_READ(rc)			CE_MARGIN + (rc.right - 2 * CE_MARGIN) * 0.25, CE_MARGIN + CE_CT_SIDE_H + CE_CT_APPLY_H, (rc.right - 2 * CE_MARGIN) * 0.25, CE_CT_SIDE_H
#define CE_CT_POS_ALIGN(rc)			CE_MARGIN + (rc.right - 2 * CE_MARGIN) * 0.5, CE_MARGIN + CE_CT_SIDE_H + CE_CT_APPLY_H, (rc.right - 2 * CE_MARGIN) * 0.25, CE_CT_SIDE_H
#define CE_CT_POS_PREF(rc)			CE_MARGIN + (rc.right - 2 * CE_MARGIN) * 0.75, CE_MARGIN + CE_CT_SIDE_H + CE_CT_APPLY_H, (rc.right - 2 * CE_MARGIN) * 0.25, CE_CT_SIDE_H
#define CE_CT_POS_PREV(rc)			CE_MARGIN, rc.bottom - CE_CT_EDT_H - CE_MARGIN, (rc.right - 4 * CE_MARGIN) / 6.5, CE_CT_EDT_H
#define CE_CT_POS_PREV_DUR(rc)		CE_MARGIN + (rc.right - 4 * CE_MARGIN) / 6.5, rc.bottom - CE_CT_EDT_H - CE_MARGIN, CE_MARGIN + (rc.right - 4 * CE_MARGIN) * 1.5 / 6.5, CE_CT_EDT_H
#define CE_CT_POS_FIT(rc)			2 * CE_MARGIN + (rc.right - 4 * CE_MARGIN) * 2.5 / 6.5, rc.bottom - CE_CT_EDT_H - CE_MARGIN, (rc.right - 4 * CE_MARGIN) / 6.5, CE_CT_EDT_H
#define CE_CT_POS_ID_BACK(rc)		3 * CE_MARGIN + (rc.right - 4 * CE_MARGIN) * 3.5 / 6.5, rc.bottom - CE_CT_EDT_H - CE_MARGIN, (rc.right - 4 * CE_MARGIN) * 0.5 / 6.5, CE_CT_EDT_H
#define CE_CT_POS_ID_NEXT(rc)		3 * CE_MARGIN + (rc.right - 4 * CE_MARGIN) * 5 / 6.5, rc.bottom - CE_CT_EDT_H - CE_MARGIN, (rc.right - 4 * CE_MARGIN) * 0.5 / 6.5, CE_CT_EDT_H
#define CE_CT_POS_ID(rc)			3 * CE_MARGIN + (rc.right - 4 * CE_MARGIN) * 4 / 6.5, rc.bottom - CE_CT_EDT_H - CE_MARGIN, (rc.right - 4 * CE_MARGIN) / 6.5, CE_CT_EDT_H
#define CE_CT_POS_DELETE(rc)		3 * CE_MARGIN + (rc.right - 4 * CE_MARGIN) * 5.5 / 6.5, rc.bottom - CE_CT_EDT_H - CE_MARGIN, (rc.right - 4 * CE_MARGIN) / 6.5, CE_CT_EDT_H
#define CE_CT_POS_SEARCH(rc)		CE_MARGIN, CE_MARGIN, rc.right - 2 * CE_MARGIN, CE_CT_SEARCH_H