//----------------------------------------------------------------------------------
//		Flow for AviUtl
//		Source File (Functions for IDCurve class)
//		(Visual C++ 2019)
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		制御点群を追加
//---------------------------------------------------------------------
void ce::Curve_ID::addPoint(POINT gr_pt)
{
	int index = 0;
	Point_Address tmp;
	if (ctpts.size() >= CE_POINT_MAX) return;
	for (int i = 0; i < ctpts.size(); i++) {
		if (ctpts[i].pt_center.x == gr_pt.x)
			return;
		else if (ctpts[i].pt_center.x > gr_pt.x) {
			index = i;
			break;
		}
	}
	Points_ID ctpt_add;
	ctpt_add.type = 2;
	ctpt_add.pt_center = gr_pt;
	ctpt_add.pt_left = { gr_pt.x - CE_HANDLE_DEF_L, gr_pt.y };
	ctpt_add.pt_right = { gr_pt.x + CE_HANDLE_DEF_L, gr_pt.y };
	ctpts.insert(ctpts.begin() + index, ctpt_add);

	//左右の点が両隣の中央の点より左/右に出ていたら修正
	if (ctpts[index].pt_left.x < ctpts[index - 1].pt_center.x)
		ctpts[index].pt_left.x = ctpts[index - 1].pt_center.x;
	if (ctpts[index].pt_right.x > ctpts[index + 1].pt_center.x)
		ctpts[index].pt_right.x = ctpts[index + 1].pt_center.x;

	//両隣の左右の点が中央の点より右/左に出ていたら修正
	if (ctpts[index - 1].pt_right.x > ctpts[index].pt_center.x) {
		tmp = { index - 1, 3 };
		CorrectHandle(tmp, ctpts[index].pt_center.x, FALSE, NULL);
	}
	if (ctpts[index + 1].pt_left.x < ctpts[index].pt_center.x) {
		tmp = { index + 1, 2 };
		CorrectHandle(tmp, ctpts[index].pt_center.x, FALSE, NULL);
	}
}


//---------------------------------------------------------------------
//		制御点群を削除
//---------------------------------------------------------------------
void ce::Curve_ID::DeletePoint(POINT cl_pt)
{
	Point_Address address = PtInCtpts(cl_pt);
	if (!address.CLR) return;
	for (int i = 1; i < ctpts.size() - 1; i++) {
		if (address.index == i) {
			ctpts.erase(ctpts.begin() + i);
			break;
		}
	}
}


//---------------------------------------------------------------------
//		制御点(群)を移動
//---------------------------------------------------------------------
void ce::Curve_ID::MovePoint(Point_Address address, POINT gr_pt, BOOL bReset)
{
	constexpr double MATH_PI = 3.14159265;
	static POINT prevright, nextleft, hdleft, hdright;
	Point_Address tmp;
	double agl_tmp;
	static double agl_prev, agl_next,
		agl_left, agl_right,
		lgth_left, lgth_right;

	//ハンドル位置&角度を記憶
	if (bReset && ctpts[address.index].type > 1) {
		prevright = ctpts[address.index - 1].pt_right;
		nextleft = ctpts[address.index + 1].pt_left;
		hdleft = {
		ctpts[address.index].pt_left.x - ctpts[address.index].pt_center.x,
		ctpts[address.index].pt_left.y - ctpts[address.index].pt_center.y,
		};
		hdright = {
			ctpts[address.index].pt_right.x - ctpts[address.index].pt_center.x,
			ctpts[address.index].pt_right.y - ctpts[address.index].pt_center.y,
		};
		tmp = { address.index - 1, 3 };
		agl_prev = GetHandleAngle(tmp);
		tmp = { address.index + 1, 2 };
		agl_next = GetHandleAngle(tmp);
		tmp = { address.index, 2 };
		agl_left = GetHandleAngle(tmp);
		tmp = { address.index, 3 };
		agl_right = GetHandleAngle(tmp);

		lgth_left = DISTANCE(ctpts[address.index].pt_center, ctpts[address.index].pt_left);
		lgth_right = DISTANCE(ctpts[address.index].pt_center, ctpts[address.index].pt_right);
	}
	switch (address.CLR) {
		//Center ---o---
	case 1:
		if (ctpts[address.index].type < 2) return;

		//中央 ---o---
		ctpts[address.index].pt_center.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index - 1].pt_center.x + 1,
			ctpts[address.index + 1].pt_center.x - 1);
		ctpts[address.index].pt_center.y = gr_pt.y;

		//左   O-----o
		ctpts[address.index].pt_left.x = ctpts[address.index].pt_center.x + hdleft.x;
		ctpts[address.index].pt_left.y = gr_pt.y + hdleft.y;

		//右  o-----O
		ctpts[address.index].pt_right.x = ctpts[address.index].pt_center.x + hdright.x;
		ctpts[address.index].pt_right.y = gr_pt.y + hdright.y;


		//左右両端のハンドル補正
		if (ctpts[address.index].type > 1) {
			tmp = { address.index, 2 };//Left  o-----O
			if (ctpts[address.index].pt_left.x < ctpts[address.index - 1].pt_center.x)
				CorrectHandle(tmp, ctpts[address.index - 1].pt_center.x, TRUE, agl_left);
			tmp = { address.index, 3 };//Right O-----o
			if (ctpts[address.index].pt_right.x > ctpts[address.index + 1].pt_center.x)
				CorrectHandle(tmp, ctpts[address.index + 1].pt_center.x, TRUE, agl_right);

			tmp = { address.index - 1, 3 };//Right o-----O
			if (prevright.x > ctpts[address.index].pt_center.x)
				CorrectHandle(tmp, ctpts[address.index].pt_center.x, TRUE, agl_prev);
			tmp = { address.index + 1, 2 };//Left  O-----o
			if (nextleft.x < ctpts[address.index].pt_center.x)
				CorrectHandle(tmp, ctpts[address.index].pt_center.x, TRUE, agl_next);
		}

		break;
	case 2://Left   O-----o
		if (ctpts[address.index].type == 0) return;

		ctpts[address.index].pt_left.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index - 1].pt_center.x,
			ctpts[address.index].pt_center.x);
		ctpts[address.index].pt_left.y = gr_pt.y;

		//整列(角度)
		if (g_cfg.align_mode == 1) {
			tmp = { address.index, 2 };
			agl_tmp = GetHandleAngle(tmp);
			tmp = { address.index, 3 };
			SetHandleAngle(tmp, agl_tmp + MATH_PI, TRUE, lgth_right);
		}
		break;

	case 3://Right  o-----O
		if (ctpts[address.index].type == 1) return;

		ctpts[address.index].pt_right.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index].pt_center.x,
			ctpts[address.index + 1].pt_center.x);
		ctpts[address.index].pt_right.y = gr_pt.y;

		//整列(角度)
		if (g_cfg.align_mode == 1) {
			tmp = { address.index, 3 };
			agl_tmp = GetHandleAngle(tmp);
			tmp = { address.index, 2 };
			SetHandleAngle(tmp, agl_tmp + MATH_PI, TRUE, lgth_left);
		}
		break;
	}
}


//---------------------------------------------------------------------
//		指定した座標が制御点群の内部に存在しているか
//---------------------------------------------------------------------
ce::Point_Address ce::Curve_ID::PtInCtpts(POINT cl_pt)
{
	RECT rcCenter, rcLeft, rcRight;
	for (int i = 0; i < ctpts.size(); i++) {
		rcCenter = {
			(LONG)ToClient(ctpts[i].pt_center).x - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_center).y - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_center).x + CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_center).y + CE_POINT_RANGE
		};
		rcLeft = {
			(LONG)ToClient(ctpts[i].pt_left).x - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_left).y - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_left).x + CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_left).y + CE_POINT_RANGE
		};
		rcRight = {
			(LONG)ToClient(ctpts[i].pt_right).x - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_right).y - CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_right).x + CE_POINT_RANGE,
			(LONG)ToClient(ctpts[i].pt_right).y + CE_POINT_RANGE
		};

		if (PtInRect(&rcLeft, cl_pt) && ctpts[i].type != 0)
			return { i, 2 };
		else if (PtInRect(&rcRight, cl_pt) && ctpts[i].type != 1)
			return { i, 3 };
		else if (PtInRect(&rcCenter, cl_pt) && ctpts[i].type > 1)
			return { i, 1 };
	}
	return { 0, 0 };
}


//---------------------------------------------------------------------
//		ハンドルの角度を取得
//---------------------------------------------------------------------
double ce::Curve_ID::GetHandleAngle(Point_Address address)
{
	double angle;
	int dstx, dsty;
	switch (address.CLR) {
	case 2:
		dstx = ctpts[address.index].pt_left.x - ctpts[address.index].pt_center.x;
		dsty = ctpts[address.index].pt_left.y - ctpts[address.index].pt_center.y;
		break;
	case 3:
		dstx = ctpts[address.index].pt_right.x - ctpts[address.index].pt_center.x;
		dsty = ctpts[address.index].pt_right.y - ctpts[address.index].pt_center.y;
		break;
	default:
		return 0;
	}
	if (dstx == 0 && dsty == 0) return 0;
	angle = std::atan2(dsty, dstx);
	return angle;
}


//---------------------------------------------------------------------
//		ハンドルの角度を設定
//---------------------------------------------------------------------
void ce::Curve_ID::SetHandleAngle(Point_Address address, double angle, BOOL bLength, double lgth)
{
	double length;
	if (address.CLR == 2 && ctpts[address.index].type != 0) {
		length = bLength ? lgth : DISTANCE(
			ctpts[address.index].pt_center,
			ctpts[address.index].pt_left);

		ctpts[address.index].pt_left.x = MINMAXLIM(
			ctpts[address.index].pt_center.x + std::cos(angle) * length,
			ctpts[address.index - 1].pt_center.x,
			ctpts[address.index].pt_center.x);
		ctpts[address.index].pt_left.y = ctpts[address.index].pt_center.y + std::sin(angle) * length;
	}
	else if (address.CLR == 3 && ctpts[address.index].type != 1) {
		length = bLength ? lgth : DISTANCE(
			ctpts[address.index].pt_center,
			ctpts[address.index].pt_right);

		ctpts[address.index].pt_right.x = MINMAXLIM(
			ctpts[address.index].pt_center.x + std::cos(angle) * length,
			ctpts[address.index].pt_center.x,
			ctpts[address.index + 1].pt_center.x);
		ctpts[address.index].pt_right.y = ctpts[address.index].pt_center.y + std::sin(angle) * length;
	}
	else return;
}


//---------------------------------------------------------------------
//		ハンドルを連動
//---------------------------------------------------------------------
void ce::Curve_ID::CorrectHandle(Point_Address address, int x, BOOL bAngle, double agl)
{
	double angle = bAngle ? agl : GetHandleAngle(address);
	int x_dst;
	switch (address.CLR) {
	case 2://Left   O-----o
		ctpts[address.index].pt_left.x = MAXLIM(x, ctpts[address.index].pt_center.x);
		ctpts[address.index].pt_left.y =
			ctpts[address.index].pt_center.y + std::tan(angle) *
			(ctpts[address.index].pt_left.x - ctpts[address.index].pt_center.x);
		break;
	case 3://Right  o-----O
		ctpts[address.index].pt_right.x = MINLIM(x, ctpts[address.index].pt_center.x);
		ctpts[address.index].pt_right.y =
			ctpts[address.index].pt_center.y + std::tan(angle) *
			(ctpts[address.index].pt_right.x - ctpts[address.index].pt_center.x);
		break;
	default:
		return;
	}
}


//---------------------------------------------------------------------
//		グラフを反転
//---------------------------------------------------------------------
void ce::Curve_ID::ReversePoints()
{
	std::vector<Points_ID> ctpts_old;
	for (int i = 0; i < ctpts.size() / 2.0; i++) {
		ctpts_old.emplace_back(ctpts[i]);
		ctpts[i].pt_center.x = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_center.x;
		ctpts[i].pt_center.y = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_center.y;
		ctpts[i].pt_left.x = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_right.x;
		ctpts[i].pt_left.y = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_right.y;
		ctpts[i].pt_right.x = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_left.x;
		ctpts[i].pt_right.y = CE_GR_RES - ctpts[ctpts.size() - i - 1].pt_left.y;

		ctpts[ctpts.size() - i - 1].pt_center.x = CE_GR_RES - ctpts_old[i].pt_center.x;
		ctpts[ctpts.size() - i - 1].pt_center.y = CE_GR_RES - ctpts_old[i].pt_center.y;
		ctpts[ctpts.size() - i - 1].pt_left.x = CE_GR_RES - ctpts_old[i].pt_right.x;
		ctpts[ctpts.size() - i - 1].pt_left.y = CE_GR_RES - ctpts_old[i].pt_right.y;
		ctpts[ctpts.size() - i - 1].pt_right.x = CE_GR_RES - ctpts_old[i].pt_left.x;
		ctpts[ctpts.size() - i - 1].pt_right.y = CE_GR_RES - ctpts_old[i].pt_left.y;
	}

}


//---------------------------------------------------------------------
//		Get Value
//---------------------------------------------------------------------
double ce::Curve_ID::GetValue(double ratio, double st, double ed)
{
	if (!ISINRANGE(ratio, 0, 1)) return 0;
	for (int i = 0; i < ctpts.size() - 1; i++) {
		if (ISINRANGE(ratio, ctpts[i].pt_center.x / (double)CE_GR_RES, ctpts[i + 1].pt_center.x / (double)CE_GR_RES)) {
			double range = (ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x) / (double)CE_GR_RES;
			double x1 = (ctpts[i].pt_right.x - ctpts[i].pt_center.x) / (ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x);
			double y1 = (ctpts[i].pt_right.y - ctpts[i].pt_center.y) / (ctpts[i + 1].pt_center.y - ctpts[i].pt_center.y);
			double x2 = (ctpts[i + 1].pt_center.x - ctpts[i + 1].pt_left.x) / (ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x);
			double y2 = (ctpts[i + 1].pt_center.y - ctpts[i + 1].pt_left.y) / (ctpts[i + 1].pt_center.y - ctpts[i].pt_center.y);
			double st2 = st + ctpts[i].pt_center.y * (ed - st) / (double)CE_GR_RES;
			double ed2 = st + ctpts[i + 1].pt_center.y * (ed - st) / (double)CE_GR_RES;
			double tl = 0;
			double tr = 1;
			double ta = 0.5 * (tl + tr);
			double xta;
			for (int j = 1; j < 10; j++) {
				xta = (1 - 3 * x2 + 3 * x1) * std::pow(ta, 3) + (x2 - 2 * x1) * 3 * std::pow(ta, 2) + 3 * x1 * ta;
				if (ratio < xta) tr = ta;
				else tl = ta;
				ta = 0.5 * (tl + tr);
			}
			return std::pow(ta, 3) * (ed2 - st2 - 3 * y2 + 3 * y1) + 3 * std::pow(ta, 2) * (y2 - 2 * y1 + st2) + 3 * ta * (y1 - st2) + st2;
		}
	}
}