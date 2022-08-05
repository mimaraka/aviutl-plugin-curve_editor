//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (IDモードでのカーブの関数)
//		VC++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		制御点群を追加
//---------------------------------------------------------------------
void ce::Curve_ID::addPoint(POINT gr_pt)
{
	int index = 0;
	Point_Address tmp;
	if (control_points.size() >= CE_POINT_MAX) return;
	for (int i = 0; i < control_points.size(); i++) {
		if (control_points[i].pt_center.x == gr_pt.x)
			return;
		else if (control_points[i].pt_center.x > gr_pt.x) {
			index = i;
			break;
		}
	}
	Points_ID control_point_add;
	control_point_add.type = 2;
	control_point_add.pt_center = gr_pt;
	//ビューの縮尺に合わせてハンドルのデフォルトの長さを変更
	control_point_add.pt_left = { gr_pt.x - (int)(CE_HANDLE_DEF_L / g_disp_info.scale.x), gr_pt.y};
	control_point_add.pt_right = { gr_pt.x + (int)(CE_HANDLE_DEF_L / g_disp_info.scale.x), gr_pt.y};
	control_points.insert(control_points.begin() + index, control_point_add);

	//左右の点が両隣の中央の点より左/右に出ていたら修正
	if (control_points[index].pt_left.x < control_points[index - 1].pt_center.x)
		control_points[index].pt_left.x = control_points[index - 1].pt_center.x;
	if (control_points[index].pt_right.x > control_points[index + 1].pt_center.x)
		control_points[index].pt_right.x = control_points[index + 1].pt_center.x;

	//両隣の左右の点が中央の点より右/左に出ていたら修正
	
	tmp = { index - 1, CONTROLPOINT_RIGHT };
	CorrectHandle(tmp, GetHandleAngle(tmp));
	
	tmp = { index + 1, CONTROLPOINT_LEFT };
	CorrectHandle(tmp, GetHandleAngle(tmp));
	
}


//---------------------------------------------------------------------
//		制御点群を削除
//---------------------------------------------------------------------
void ce::Curve_ID::DeletePoint(POINT cl_pt)
{
	Point_Address address = PtIncontrol_points(cl_pt);
	if (!address.position) return;
	for (int i = 1; i < control_points.size() - 1; i++) {
		if (address.index == i) {
			control_points.erase(control_points.begin() + i);
			break;
		}
	}
}


POINT ce::Curve_ID::GetPoint(Point_Address address)
{
	POINT result = { -1, -1 };
		switch (address.position) {
		case 1://中央
			result = control_points[address.index].pt_center;
			break;

		case 2://左
			result = control_points[address.index].pt_left;
			break;

		case 3://右
			result = control_points[address.index].pt_right;
			break;

		default:
			break;
	}
		return result;
}


//---------------------------------------------------------------------
//		制御点(群)を移動
//---------------------------------------------------------------------
void ce::Curve_ID::MovePoint(Point_Address address, POINT gr_pt, BOOL bReset)
{
	static POINT prevright, nextleft, hdleft, hdright;
	Point_Address tmp;
	double agl_tmp;
	static double agl_prev, agl_next,
		agl_left, agl_right,
		len_left, len_right;

	//ハンドル位置&角度を記憶
	if (bReset && control_points[address.index].type > 1) {
		prevright = control_points[address.index - 1].pt_right;
		nextleft = control_points[address.index + 1].pt_left;
		hdleft = {
		control_points[address.index].pt_left.x - control_points[address.index].pt_center.x,
		control_points[address.index].pt_left.y - control_points[address.index].pt_center.y,
		};
		hdright = {
			control_points[address.index].pt_right.x - control_points[address.index].pt_center.x,
			control_points[address.index].pt_right.y - control_points[address.index].pt_center.y,
		};
		tmp = { address.index - 1, CONTROLPOINT_RIGHT };
		agl_prev = GetHandleAngle(tmp);
		tmp = { address.index + 1, CONTROLPOINT_LEFT };
		agl_next = GetHandleAngle(tmp);
		tmp = { address.index, CONTROLPOINT_LEFT };
		agl_left = GetHandleAngle(tmp);
		tmp = { address.index, CONTROLPOINT_RIGHT };
		agl_right = GetHandleAngle(tmp);

		len_left = DISTANCE(control_points[address.index].pt_center, control_points[address.index].pt_left);
		len_right = DISTANCE(control_points[address.index].pt_center, control_points[address.index].pt_right);
	}
	switch (address.position) {
		//中央 ---[]---
	case 1:
		if (control_points[address.index].type < 2) return;

		//中央 ---[]---
		control_points[address.index].pt_center.x = MINMAXLIM(gr_pt.x,
			control_points[address.index - 1].pt_center.x + 1,
			control_points[address.index + 1].pt_center.x - 1);
		control_points[address.index].pt_center.y = gr_pt.y;

		//左   O-----[]
		control_points[address.index].pt_left.x = control_points[address.index].pt_center.x + hdleft.x;
		control_points[address.index].pt_left.y = gr_pt.y + hdleft.y;

		//右  []-----O
		control_points[address.index].pt_right.x = control_points[address.index].pt_center.x + hdright.x;
		control_points[address.index].pt_right.y = gr_pt.y + hdright.y;


		//左右両端のハンドル補正
		if (control_points[address.index].type > 1) {//拡張制御点だった場合
			tmp = { address.index, CONTROLPOINT_LEFT };//左  O-----[]
			CorrectHandle(tmp, agl_left);
			tmp = { address.index, CONTROLPOINT_RIGHT };//右 []-----O
			CorrectHandle(tmp, agl_right);

			control_points[address.index - 1].pt_right = prevright;
			control_points[address.index + 1].pt_left = nextleft;

			tmp = { address.index - 1, CONTROLPOINT_RIGHT };//右 []-----O (前の制御点群)
			CorrectHandle(tmp, agl_prev);
			tmp = { address.index + 1, CONTROLPOINT_LEFT };//左  O-----[] (次の制御点群)
			CorrectHandle(tmp, agl_next);
		}
		break;

	case 2://左 O-----[]
		if (control_points[address.index].type == 0) return;

		control_points[address.index].pt_left.x = MINMAXLIM(gr_pt.x,
			control_points[address.index - 1].pt_center.x,
			control_points[address.index].pt_center.x);
		control_points[address.index].pt_left.y = gr_pt.y;

		//整列(角度)
		if (g_config.align_mode == 1) {
			agl_tmp = GetHandleAngle(address);
			tmp = { address.index, CONTROLPOINT_RIGHT };
			SetHandleAngle(tmp, agl_tmp + MATH_PI, TRUE, len_right);
		}
		break;

	case 3://Right  o-----O
		if (control_points[address.index].type == 1) return;

		control_points[address.index].pt_right.x = MINMAXLIM(gr_pt.x,
			control_points[address.index].pt_center.x,
			control_points[address.index + 1].pt_center.x);
		control_points[address.index].pt_right.y = gr_pt.y;

		//整列(角度)
		if (g_config.align_mode == 1) {
			agl_tmp = GetHandleAngle(address);
			tmp = { address.index, CONTROLPOINT_LEFT };
			SetHandleAngle(tmp, agl_tmp + MATH_PI, TRUE, len_left);
		}
		break;
	}
}


//---------------------------------------------------------------------
//		指定した座標が制御点群の内部に存在しているか
//---------------------------------------------------------------------
ce::Point_Address ce::Curve_ID::PtIncontrol_points(POINT cl_pt)
{
	RECT rcCenter, rcLeft, rcRight;
	for (int i = 0; i < control_points.size(); i++) {
		rcCenter = {
			(LONG)ToClient(control_points[i].pt_center).x - CE_POINT_RANGE,
			(LONG)ToClient(control_points[i].pt_center).y - CE_POINT_RANGE,
			(LONG)ToClient(control_points[i].pt_center).x + CE_POINT_RANGE,
			(LONG)ToClient(control_points[i].pt_center).y + CE_POINT_RANGE
		};
		rcLeft = {
			(LONG)ToClient(control_points[i].pt_left).x - CE_POINT_RANGE,
			(LONG)ToClient(control_points[i].pt_left).y - CE_POINT_RANGE,
			(LONG)ToClient(control_points[i].pt_left).x + CE_POINT_RANGE,
			(LONG)ToClient(control_points[i].pt_left).y + CE_POINT_RANGE
		};
		rcRight = {
			(LONG)ToClient(control_points[i].pt_right).x - CE_POINT_RANGE,
			(LONG)ToClient(control_points[i].pt_right).y - CE_POINT_RANGE,
			(LONG)ToClient(control_points[i].pt_right).x + CE_POINT_RANGE,
			(LONG)ToClient(control_points[i].pt_right).y + CE_POINT_RANGE
		};

		if (PtInRect(&rcLeft, cl_pt) && control_points[i].type != 0)
			return { i, CONTROLPOINT_LEFT };
		else if (PtInRect(&rcRight, cl_pt) && control_points[i].type != 1)
			return { i, CONTROLPOINT_RIGHT };
		else if (PtInRect(&rcCenter, cl_pt) && control_points[i].type > 1)
			return { i, CONTROLPOINT_CENTER };
	}
	ce::Point_Address control_point_null = { 0, CONTROLPOINT_NULL };
	return control_point_null;
}


//---------------------------------------------------------------------
//		ハンドルの角度を取得
//---------------------------------------------------------------------
double ce::Curve_ID::GetHandleAngle(Point_Address address)
{
	double angle;
	int dstx, dsty;
	switch (address.position) {
	case 2:
		dstx = control_points[address.index].pt_left.x - control_points[address.index].pt_center.x;
		dsty = control_points[address.index].pt_left.y - control_points[address.index].pt_center.y;
		break;
	case 3:
		dstx = control_points[address.index].pt_right.x - control_points[address.index].pt_center.x;
		dsty = control_points[address.index].pt_right.y - control_points[address.index].pt_center.y;
		break;
	default:
		return 0;
	}
	//x, yがともに0のとき
	if (dstx == 0 && dsty == 0) {
		//左
		if (address.position == CONTROLPOINT_LEFT)
			return MATH_PI;
		//右
		else if (address.position == CONTROLPOINT_RIGHT)
			return 0;
	}
	angle = std::atan2(dsty, dstx);
	return angle;
}


//---------------------------------------------------------------------
//		ハンドルの角度を設定
//---------------------------------------------------------------------
void ce::Curve_ID::SetHandleAngle(Point_Address address, double angle, BOOL bLength, double lgth)
{
	double length;
	//左-left
	if (address.position == 2 && control_points[address.index].type != 0) {
		length = bLength ? lgth : DISTANCE(
			control_points[address.index].pt_center,
			control_points[address.index].pt_left);

		control_points[address.index].pt_left.x = control_points[address.index].pt_center.x + std::cos(angle) * length;
		control_points[address.index].pt_left.y = control_points[address.index].pt_center.y + std::sin(angle) * length;
		CorrectHandle(address, angle);
	}
	//右-right
	else if (address.position == CONTROLPOINT_RIGHT &&
			control_points[address.index].type != 1) {
		length = bLength ? lgth : DISTANCE(
			control_points[address.index].pt_center,
			control_points[address.index].pt_right);

		control_points[address.index].pt_right.x = control_points[address.index].pt_center.x + std::cos(angle) * length;
		control_points[address.index].pt_right.y = control_points[address.index].pt_center.y + std::sin(angle) * length;
		CorrectHandle(address, angle);
	}
	else return;
}


//---------------------------------------------------------------------
//		指定したアドレスのハンドル(右か左)が，前または次の制御点群の中央の点を超えている場合に，
//		ハンドルの角度を保ちながら修正する関数
//		address:	指定する制御点のアドレス
//		x:			指定するX座標
//		bAngle:		角度を設定するかどうか(FALSEなら指定したアドレスのハンドルの角度を取得する)
//		agl:		設定する角度
//---------------------------------------------------------------------
void ce::Curve_ID::CorrectHandle(Point_Address address, double angle)
{
	switch (address.position) {
	case 2://左   O-----[]
		//左の制御点が前の制御点群の中央の点より左側にあったとき
		if (GetPoint(address).x < control_points[address.index - 1].pt_center.x) {
			control_points[address.index].pt_left.x = control_points[address.index - 1].pt_center.x;
			//角度を保つ
			control_points[address.index].pt_left.y =
				control_points[address.index].pt_center.y + std::tan(angle) *
				(control_points[address.index].pt_left.x - control_points[address.index].pt_center.x);
		}
		break;
	case 3://右  []-----O
		if (GetPoint(address).x > control_points[address.index + 1].pt_center.x) {
			control_points[address.index].pt_right.x = control_points[address.index + 1].pt_center.x;
			//角度を保つ
			control_points[address.index].pt_right.y =
				control_points[address.index].pt_center.y + std::tan(angle) *
				(control_points[address.index].pt_right.x - control_points[address.index].pt_center.x);
		}
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
	std::vector<Points_ID> control_points_old;
	for (int i = 0; i < control_points.size() / 2.0; i++) {
		control_points_old.emplace_back(control_points[i]);
		control_points[i].pt_center.x = CE_GR_RES - control_points[control_points.size() - i - 1].pt_center.x;
		control_points[i].pt_center.y = CE_GR_RES - control_points[control_points.size() - i - 1].pt_center.y;
		control_points[i].pt_left.x = CE_GR_RES - control_points[control_points.size() - i - 1].pt_right.x;
		control_points[i].pt_left.y = CE_GR_RES - control_points[control_points.size() - i - 1].pt_right.y;
		control_points[i].pt_right.x = CE_GR_RES - control_points[control_points.size() - i - 1].pt_left.x;
		control_points[i].pt_right.y = CE_GR_RES - control_points[control_points.size() - i - 1].pt_left.y;

		control_points[control_points.size() - i - 1].pt_center.x = CE_GR_RES - control_points_old[i].pt_center.x;
		control_points[control_points.size() - i - 1].pt_center.y = CE_GR_RES - control_points_old[i].pt_center.y;
		control_points[control_points.size() - i - 1].pt_left.x = CE_GR_RES - control_points_old[i].pt_right.x;
		control_points[control_points.size() - i - 1].pt_left.y = CE_GR_RES - control_points_old[i].pt_right.y;
		control_points[control_points.size() - i - 1].pt_right.x = CE_GR_RES - control_points_old[i].pt_left.x;
		control_points[control_points.size() - i - 1].pt_right.y = CE_GR_RES - control_points_old[i].pt_left.y;
	}

}


//---------------------------------------------------------------------
//		Get Value
//---------------------------------------------------------------------
double ce::Curve_ID::GetValue(double ratio, double st, double ed)
{
	if (!ISINRANGE(ratio, 0, 1)) return 0;
	for (int i = 0; i < control_points.size() - 1; i++) {
		if (ISINRANGE(ratio, control_points[i].pt_center.x / (double)CE_GR_RES, control_points[i + 1].pt_center.x / (double)CE_GR_RES)) {
			double range = (control_points[i + 1].pt_center.x - control_points[i].pt_center.x) / (double)CE_GR_RES;
			double x1 = (control_points[i].pt_right.x - control_points[i].pt_center.x) / (control_points[i + 1].pt_center.x - control_points[i].pt_center.x);
			double y1 = (control_points[i].pt_right.y - control_points[i].pt_center.y) / (control_points[i + 1].pt_center.y - control_points[i].pt_center.y);
			double x2 = (control_points[i + 1].pt_center.x - control_points[i + 1].pt_left.x) / (control_points[i + 1].pt_center.x - control_points[i].pt_center.x);
			double y2 = (control_points[i + 1].pt_center.y - control_points[i + 1].pt_left.y) / (control_points[i + 1].pt_center.y - control_points[i].pt_center.y);
			double st2 = st + control_points[i].pt_center.y * (ed - st) / (double)CE_GR_RES;
			double ed2 = st + control_points[i + 1].pt_center.y * (ed - st) / (double)CE_GR_RES;
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