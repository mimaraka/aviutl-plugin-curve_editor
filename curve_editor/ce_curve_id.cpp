//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (IDモードでのカーブの関数)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void ce::Curve_ID::init()
{
	Points_ID pt_add[2];
	pt_add[0].type = 0;
	pt_add[0].pt_center = { 0, 0 };
	pt_add[0].pt_right = { (int)(CE_GR_RESOLUTION * 0.4), (int)(CE_GR_RESOLUTION * 0.4) };
	pt_add[0].pt_left = { 0, 0 };
	ctpts.push_back(pt_add[0]);

	pt_add[1].type = 1;
	pt_add[1].pt_center = { CE_GR_RESOLUTION, CE_GR_RESOLUTION };
	pt_add[1].pt_left = { (int)(CE_GR_RESOLUTION * 0.6), (int)(CE_GR_RESOLUTION * 0.6) };
	pt_add[1].pt_right = { CE_GR_RESOLUTION, CE_GR_RESOLUTION };
	ctpts.push_back(pt_add[1]);
}



//---------------------------------------------------------------------
//		ポイント・ハンドルを追加
//---------------------------------------------------------------------
void ce::Curve_ID::add_point(POINT gr_pt)
{
	int index = 0;
	Point_Address tmp;
	if (ctpts.size >= CE_POINT_MAX) return;
	if (!ISINRANGE(gr_pt.x, 0, CE_GR_RESOLUTION)) return;
	for (int i = 0; i < (int)ctpts.size; i++) {
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
	//ビューの縮尺に合わせてハンドルのデフォルトの長さを変更
	ctpt_add.pt_left = { gr_pt.x - (int)(CE_HANDLE_DEF_L / g_view_info.scale.x), gr_pt.y};
	ctpt_add.pt_right = { gr_pt.x + (int)(CE_HANDLE_DEF_L / g_view_info.scale.x), gr_pt.y};
	ctpts.insert(index, ctpt_add);

	//左右の点が両隣の中央の点より左/右に出ていたら修正
	if (ctpts[index].pt_left.x < ctpts[index - 1].pt_center.x)
		ctpts[index].pt_left.x = ctpts[index - 1].pt_center.x;
	if (ctpts[index].pt_right.x > ctpts[index + 1].pt_center.x)
		ctpts[index].pt_right.x = ctpts[index + 1].pt_center.x;

	//両隣の左右の点が中央の点より右/左に出ていたら修正
	
	tmp = { index - 1, CTPT_RIGHT };
	correct_handle(tmp, get_handle_angle(tmp));
	
	tmp = { index + 1, CTPT_LEFT };
	correct_handle(tmp, get_handle_angle(tmp));
	
}


//---------------------------------------------------------------------
//		ポイントを削除
//---------------------------------------------------------------------
void ce::Curve_ID::delete_point(POINT cl_pt)
{
	Point_Address address = pt_in_ctpt(cl_pt);
	if (!address.position) return;
	for (int i = 1; i < (int)ctpts.size - 1; i++) {
		if (address.index == i) {
			ctpts.erase(i);
			break;
		}
	}
}



//---------------------------------------------------------------------
//		ポイントを取得
//---------------------------------------------------------------------
POINT ce::Curve_ID::get_point(Point_Address address)
{
	POINT result = { -1, -1 };
		switch (address.position) {
		case 1://中央
			result = ctpts[address.index].pt_center;
			break;

		case 2://左
			result = ctpts[address.index].pt_left;
			break;

		case 3://右
			result = ctpts[address.index].pt_right;
			break;

		default:
			break;
	}
		return result;
}



//---------------------------------------------------------------------
//		ポイントをクリア
//---------------------------------------------------------------------
void ce::Curve_ID::clear()
{
	ctpts.clear();
	init();
}



//---------------------------------------------------------------------
//		ポイント・ハンドルを移動
//---------------------------------------------------------------------
void ce::Curve_ID::move_point(Point_Address address, POINT gr_pt, BOOL bReset)
{
	static POINT prevright, nextleft, hdleft, hdright;
	Point_Address tmp;
	double agl_tmp;
	static double agl_prev, agl_next,
		agl_left, agl_right,
		len_left, len_right;

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
		tmp = { address.index - 1, CTPT_RIGHT };
		agl_prev = get_handle_angle(tmp);
		tmp = { address.index + 1, CTPT_LEFT };
		agl_next = get_handle_angle(tmp);
		tmp = { address.index, CTPT_LEFT };
		agl_left = get_handle_angle(tmp);
		tmp = { address.index, CTPT_RIGHT };
		agl_right = get_handle_angle(tmp);

		len_left = DISTANCE(ctpts[address.index].pt_center, ctpts[address.index].pt_left);
		len_right = DISTANCE(ctpts[address.index].pt_center, ctpts[address.index].pt_right);
	}
	switch (address.position) {
		//中央 ---[]---
	case 1:
		if (ctpts[address.index].type < 2) return;

		//中央 ---[]---
		ctpts[address.index].pt_center.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index - 1].pt_center.x + 1,
			ctpts[address.index + 1].pt_center.x - 1);
		ctpts[address.index].pt_center.y = gr_pt.y;

		//左   O-----[]
		ctpts[address.index].pt_left.x = ctpts[address.index].pt_center.x + hdleft.x;
		ctpts[address.index].pt_left.y = gr_pt.y + hdleft.y;

		//右  []-----O
		ctpts[address.index].pt_right.x = ctpts[address.index].pt_center.x + hdright.x;
		ctpts[address.index].pt_right.y = gr_pt.y + hdright.y;


		//左右両端のハンドル補正
		if (ctpts[address.index].type > 1) {//拡張制御点だった場合
			tmp = { address.index, CTPT_LEFT };//左  O-----[]
			correct_handle(tmp, agl_left);
			tmp = { address.index, CTPT_RIGHT };//右 []-----O
			correct_handle(tmp, agl_right);

			ctpts[address.index - 1].pt_right = prevright;
			ctpts[address.index + 1].pt_left = nextleft;

			tmp = { address.index - 1, CTPT_RIGHT };//右 []-----O (前の制御点群)
			correct_handle(tmp, agl_prev);
			tmp = { address.index + 1, CTPT_LEFT };//左  O-----[] (次の制御点群)
			correct_handle(tmp, agl_next);
		}
		break;

	case 2://左 O-----[]
		if (ctpts[address.index].type == 0) return;

		ctpts[address.index].pt_left.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index - 1].pt_center.x,
			ctpts[address.index].pt_center.x);
		ctpts[address.index].pt_left.y = gr_pt.y;

		//整列(角度)
		if (g_config.align_handle) {
			agl_tmp = get_handle_angle(address);
			tmp = { address.index, CTPT_RIGHT };
			set_handle_angle(tmp, agl_tmp + MATH_PI, TRUE, len_right);
		}
		break;

	case 3://Right  origin-----O
		if (ctpts[address.index].type == 1) return;

		ctpts[address.index].pt_right.x = MINMAXLIM(gr_pt.x,
			ctpts[address.index].pt_center.x,
			ctpts[address.index + 1].pt_center.x);
		ctpts[address.index].pt_right.y = gr_pt.y;

		//整列(角度)
		if (g_config.align_handle) {
			agl_tmp = get_handle_angle(address);
			tmp = { address.index, CTPT_LEFT };
			set_handle_angle(tmp, agl_tmp + MATH_PI, TRUE, len_left);
		}
		break;
	}
}



//---------------------------------------------------------------------
//		指定した座標がポイント・ハンドルの内部に存在しているか
//---------------------------------------------------------------------
ce::Point_Address ce::Curve_ID::pt_in_ctpt(POINT cl_pt)
{
	RECT rcCenter, rcLeft, rcRight;
	for (int i = 0; i < (int)ctpts.size; i++) {
		rcCenter = {
			(LONG)to_client(ctpts[i].pt_center).x - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_center).y - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_center).x + CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_center).y + CE_POINT_RANGE
		};
		rcLeft = {
			(LONG)to_client(ctpts[i].pt_left).x - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_left).y - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_left).x + CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_left).y + CE_POINT_RANGE
		};
		rcRight = {
			(LONG)to_client(ctpts[i].pt_right).x - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_right).y - CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_right).x + CE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_right).y + CE_POINT_RANGE
		};

		if (PtInRect(&rcLeft, cl_pt) && ctpts[i].type != 0)
			return { i, CTPT_LEFT };
		else if (PtInRect(&rcRight, cl_pt) && ctpts[i].type != 1)
			return { i, CTPT_RIGHT };
		else if (PtInRect(&rcCenter, cl_pt) && ctpts[i].type > 1)
			return { i, CTPT_CENTER };
	}
	ce::Point_Address ctpt_null = { 0, CTPT_NULL };
	return ctpt_null;
}



//---------------------------------------------------------------------
//		ハンドルの角度を取得
//---------------------------------------------------------------------
double ce::Curve_ID::get_handle_angle(Point_Address address)
{
	double angle;
	int dstx, dsty;
	switch (address.position) {
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
	//x, yがともに0のとき
	if (dstx == 0 && dsty == 0) {
		//左
		if (address.position == CTPT_LEFT)
			return MATH_PI;
		//右
		else if (address.position == CTPT_RIGHT)
			return 0;
	}
	angle = std::atan2(dsty, dstx);
	return angle;
}



//---------------------------------------------------------------------
//		ハンドルの角度を設定
//---------------------------------------------------------------------
void ce::Curve_ID::set_handle_angle(Point_Address address, double angle, BOOL bLength, double lgth)
{
	double length;
	//左-left
	if (address.position == 2 && ctpts[address.index].type != 0) {
		length = bLength ? lgth : DISTANCE(
			ctpts[address.index].pt_center,
			ctpts[address.index].pt_left);

		ctpts[address.index].pt_left.x = (LONG)(ctpts[address.index].pt_center.x + std::cos(angle) * length);
		ctpts[address.index].pt_left.y = (LONG)(ctpts[address.index].pt_center.y + std::sin(angle) * length);
		correct_handle(address, angle);
	}
	//右-right
	else if (address.position == CTPT_RIGHT &&
			ctpts[address.index].type != 1) {
		length = bLength ? lgth : DISTANCE(
			ctpts[address.index].pt_center,
			ctpts[address.index].pt_right);

		ctpts[address.index].pt_right.x = (LONG)(ctpts[address.index].pt_center.x + std::cos(angle) * length);
		ctpts[address.index].pt_right.y = (LONG)(ctpts[address.index].pt_center.y + std::sin(angle) * length);
		correct_handle(address, angle);
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
void ce::Curve_ID::correct_handle(Point_Address address, double angle)
{
	switch (address.position) {
	case 2://左   O-----[]
		//左の制御点が前の制御点群の中央の点より左側にあったとき
		if (get_point(address).x < ctpts[address.index - 1].pt_center.x) {
			ctpts[address.index].pt_left.x = ctpts[address.index - 1].pt_center.x;
			//角度を保つ
			ctpts[address.index].pt_left.y = (LONG)
				(ctpts[address.index].pt_center.y + std::tan(angle) *
					(ctpts[address.index].pt_left.x - ctpts[address.index].pt_center.x));
		}
		break;
	case 3://右  []-----O
		if (get_point(address).x > ctpts[address.index + 1].pt_center.x) {
			ctpts[address.index].pt_right.x = ctpts[address.index + 1].pt_center.x;
			//角度を保つ
			ctpts[address.index].pt_right.y = (LONG)
				(ctpts[address.index].pt_center.y + std::tan(angle) *
					(ctpts[address.index].pt_right.x - ctpts[address.index].pt_center.x));
		}
		break;
	default:
		return;
	}
}



//---------------------------------------------------------------------
//		カーブを反転
//---------------------------------------------------------------------
void ce::Curve_ID::reverse_curve()
{
	std::vector<Points_ID> ctpts_old;
	for (int i = 0; i < ctpts.size / 2.0; i++) {
		ctpts_old.emplace_back(ctpts[i]);
		ctpts[i].pt_center.x = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_center.x;
		ctpts[i].pt_center.y = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_center.y;
		ctpts[i].pt_left.x = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_right.x;
		ctpts[i].pt_left.y = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_right.y;
		ctpts[i].pt_right.x = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_left.x;
		ctpts[i].pt_right.y = CE_GR_RESOLUTION - ctpts[ctpts.size - i - 1].pt_left.y;

		ctpts[ctpts.size - i - 1].pt_center.x = CE_GR_RESOLUTION - ctpts_old[i].pt_center.x;
		ctpts[ctpts.size - i - 1].pt_center.y = CE_GR_RESOLUTION - ctpts_old[i].pt_center.y;
		ctpts[ctpts.size - i - 1].pt_left.x = CE_GR_RESOLUTION - ctpts_old[i].pt_right.x;
		ctpts[ctpts.size - i - 1].pt_left.y = CE_GR_RESOLUTION - ctpts_old[i].pt_right.y;
		ctpts[ctpts.size - i - 1].pt_right.x = CE_GR_RESOLUTION - ctpts_old[i].pt_left.x;
		ctpts[ctpts.size - i - 1].pt_right.y = CE_GR_RESOLUTION - ctpts_old[i].pt_left.y;
	}

}



//---------------------------------------------------------------------
//		Get Value
//---------------------------------------------------------------------
double ce::Curve_ID::get_value(double ratio, double st, double ed)
{
	// 進捗が0~1の範囲外であった場合
	if (!ISINRANGEEQ(ratio, 0, 1))
		return 0;
	// 進捗に相当する区間を調べる
	for (int i = 0; i < (int)ctpts.size - 1; i++) {
		if (ISINRANGEEQ(ratio, ctpts[i].pt_center.x / (double)CE_GR_RESOLUTION, ctpts[i + 1].pt_center.x / (double)CE_GR_RESOLUTION)) {
			double range = (ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x) / (double)CE_GR_RESOLUTION;
			// 区間ごとの進捗の相対値(0~1)
			double ratio2 = (ratio - ctpts[i].pt_center.x / (double)CE_GR_RESOLUTION) / range;
			// 区間ごとの制御点1のX座標(相対値、0~1)
			double x1 = (ctpts[i].pt_right.x - ctpts[i].pt_center.x) / (double)(ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x);
			// 区間ごとの制御点1のY座標(相対値)
			double y1 = (ctpts[i].pt_right.y - ctpts[i].pt_center.y) / (double)(ctpts[i + 1].pt_center.y - ctpts[i].pt_center.y);
			// 区間ごとの制御点2のX座標(相対値、0~1)
			double x2 = (ctpts[i + 1].pt_left.x - ctpts[i].pt_center.x) / (double)(ctpts[i + 1].pt_center.x - ctpts[i].pt_center.x);
			// 区間ごとの制御点2のY座標(相対値)
			double y2 = (ctpts[i + 1].pt_left.y - ctpts[i].pt_center.y) / (double)(ctpts[i + 1].pt_center.y - ctpts[i].pt_center.y);

			// 区間ごとの始値、終値(相対値ではなく、実際の値)
			double st2 = st + ctpts[i].pt_center.y / (double)CE_GR_RESOLUTION * (ed - st);
			double ed2 = st + ctpts[i + 1].pt_center.y / (double)CE_GR_RESOLUTION * (ed - st);
			// y1,y2を相対値から実際の値に修正
			y1 = st2 + (ed2 - st2) * y1;
			y2 = st2 + (ed2 - st2) * y2;

			// ベジェの計算
			double tl = 0;
			double tr = 1;
			double ta = 0.5 * (tl + tr);
			double xta;
			for (int j = 0; j < 10; j++) {
				xta = (1 - 3 * x2 + 3 * x1) * std::pow(ta, 3) + (x2 - 2 * x1) * 3 * std::pow(ta, 2) + 3 * x1 * ta;
				if (ratio2 < xta) tr = ta;
				else tl = ta;
				ta = 0.5 * (tl + tr);
			}
			return std::pow(ta, 3) * (ed2 - st2 - 3 * y2 + 3 * y1) + 3 * std::pow(ta, 2) * (y2 - 2 * y1 + st2) + 3 * ta * (y1 - st2) + st2;
		}
	}
	return 0;
}