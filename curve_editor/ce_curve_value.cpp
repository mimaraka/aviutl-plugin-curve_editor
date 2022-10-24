//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (Valueモードのカーブの関数)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void ce::Curve_Value::init()
{
	ctpt[0] = {
		(int)(CE_GR_RESOLUTION * CE_CURVE_DEF_1),
		(int)(CE_GR_RESOLUTION * CE_CURVE_DEF_1)
	};
	ctpt[1] = {
		(int)(CE_GR_RESOLUTION * CE_CURVE_DEF_2),
		(int)(CE_GR_RESOLUTION * CE_CURVE_DEF_2)
	};
}



//---------------------------------------------------------------------
//		指定した座標が制御点の内部に存在するか
//---------------------------------------------------------------------
int ce::Curve_Value::point_in_ctpts(POINT cl_pt)
{
	RECT rcCtpt1 = {
		(LONG)to_client(ctpt[0]).x - CE_POINT_RANGE,
		(LONG)to_client(ctpt[0]).y - CE_POINT_RANGE,
		(LONG)to_client(ctpt[0]).x + CE_POINT_RANGE,
		(LONG)to_client(ctpt[0]).y + CE_POINT_RANGE
	};
	RECT rcCtpt2 = {
		(LONG)to_client(ctpt[1]).x - CE_POINT_RANGE,
		(LONG)to_client(ctpt[1]).y - CE_POINT_RANGE,
		(LONG)to_client(ctpt[1]).x + CE_POINT_RANGE,
		(LONG)to_client(ctpt[1]).y + CE_POINT_RANGE
	};
	if (PtInRect(&rcCtpt2, cl_pt))
		return 2;
	else if (PtInRect(&rcCtpt1, cl_pt))
		return 1;
	else return 0;
}



//---------------------------------------------------------------------
//		制御点を移動させる
//---------------------------------------------------------------------
void ce::Curve_Value::move_point(int index, POINT gr_pt)
{
	static POINT ptLock;
	static BOOL bShiftKey, bCtrlKey, bAltKey;

	// Altキーが押された瞬間
	if (::GetAsyncKeyState(VK_MENU) < 0 && !bAltKey) {
		ptLock.x = gr_pt.x;
		ptLock.y = gr_pt.y;
		bAltKey = TRUE;
		bCtrlKey = FALSE;
		bShiftKey = FALSE;
	}
	// Altキーが押されている間
	else if (::GetAsyncKeyState(VK_MENU) < 0) {
		//X
		g_curve_value.ctpt[index].x = MINMAXLIM(gr_pt.x, 0, CE_GR_RESOLUTION);
		//Y
		// ゼロ除算対策
		if (!index) {
			if (ptLock.x < 0)
				g_curve_value.ctpt[0].y = gr_pt.y;
			else
				g_curve_value.ctpt[0].y = (int)(g_curve_value.ctpt[0].x * ptLock.y / (double)ptLock.x);
		}
		else {
			if (ptLock.x > CE_GR_RESOLUTION)
				g_curve_value.ctpt[1].y = gr_pt.y;
			else
				g_curve_value.ctpt[1].y =
					CE_GR_RESOLUTION - (int)((CE_GR_RESOLUTION - (int)g_curve_value.ctpt[1].x) *
					(CE_GR_RESOLUTION - ptLock.y) / (double)(CE_GR_RESOLUTION - ptLock.x));
		}
	}
	// 同時に動かす
	else if (::GetAsyncKeyState(VK_SHIFT) < 0 && ::GetAsyncKeyState(VK_CONTROL) < 0) {
		//X
		g_curve_value.ctpt[index].x = gr_pt.x;
		g_curve_value.ctpt[!index].x = MINMAXLIM(CE_GR_RESOLUTION - g_curve_value.ctpt[index].x, 0, CE_GR_RESOLUTION);
		//Y
		g_curve_value.ctpt[index].y = gr_pt.y;
		g_curve_value.ctpt[!index].y = CE_GR_RESOLUTION - g_curve_value.ctpt[index].y;
		bCtrlKey = FALSE;
		bAltKey = FALSE;
		bShiftKey = FALSE;
	}
	// Shiftキーが押された瞬間
	else if (::GetAsyncKeyState(VK_SHIFT) < 0 && !bShiftKey) {
		ptLock.y = gr_pt.y;
		bShiftKey = TRUE;
		bCtrlKey = FALSE;
		bAltKey = FALSE;
	}
	// Shiftキーが押されている間
	else if (::GetAsyncKeyState(VK_SHIFT) < 0) {
		g_curve_value.ctpt[index].x = gr_pt.x;
		//if Y is larger than 500
		if (ptLock.y < CE_GR_RESOLUTION / 2)
			g_curve_value.ctpt[index].y = 0;
		//if Y is less than 500
		else if (ptLock.y >= CE_GR_RESOLUTION / 2)
			g_curve_value.ctpt[index].y = CE_GR_RESOLUTION;
	}
	// Ctrlキーが押された瞬間
	else if (::GetAsyncKeyState(VK_CONTROL) < 0 && !bCtrlKey) {
		ptLock.x = gr_pt.x; ptLock.y = gr_pt.y;
		bCtrlKey = TRUE;
		bAltKey = FALSE;
		bShiftKey = FALSE;
	}
	// Ctrlキーが押されている間
	else if (::GetAsyncKeyState(VK_CONTROL) < 0) {
		float theta;
		int intResult_x, intResult_y;
		if (!index) {
			theta = (float)std::atan2(gr_pt.y, gr_pt.x);
			intResult_x = (int)(DISTANCE1(ptLock) * std::cos(theta));
			intResult_y = (int)(DISTANCE1(ptLock) * std::sin(theta));
		}
		else {
			theta = (float)std::atan2(CE_GR_RESOLUTION - gr_pt.y, CE_GR_RESOLUTION - gr_pt.x);
			intResult_x = CE_GR_RESOLUTION - (int)(DISTANCE2(ptLock, CE_GR_RESOLUTION, CE_GR_RESOLUTION) * std::cos(theta));
			intResult_y = CE_GR_RESOLUTION - (int)(DISTANCE2(ptLock, CE_GR_RESOLUTION, CE_GR_RESOLUTION) * std::sin(theta));
		}
		g_curve_value.ctpt[index].x = intResult_x;
		//Y
		if (!index) {
			if (theta > MATH_PI * 0.5)
				g_curve_value.ctpt[0].y = (int)(DISTANCE1(ptLock));

			else if (theta < -MATH_PI * 0.5)
				g_curve_value.ctpt[0].y = (int)-(DISTANCE1(ptLock));

			else g_curve_value.ctpt[0].y = intResult_y;
		}
		else {
			if (theta > MATH_PI * 0.5)
				g_curve_value.ctpt[1].y = CE_GR_RESOLUTION - (int)(DISTANCE2(ptLock, CE_GR_RESOLUTION, CE_GR_RESOLUTION));

			else if (theta < -MATH_PI * 0.5)
				g_curve_value.ctpt[1].y = CE_GR_RESOLUTION + (int)(DISTANCE2(ptLock, CE_GR_RESOLUTION, CE_GR_RESOLUTION));

			else g_curve_value.ctpt[1].y = intResult_y;
		}
	}
	//通常
	else {
		g_curve_value.ctpt[index] = gr_pt;
		bCtrlKey = FALSE;
		bAltKey = FALSE;
		bShiftKey = FALSE;
	}

	// 範囲制限
	g_curve_value.ctpt[index].x = MINMAXLIM(g_curve_value.ctpt[index].x, 0, CE_GR_RESOLUTION);
	g_curve_value.ctpt[index].y = MINMAXLIM(
		g_curve_value.ctpt[index].y,
		(LONG)(CE_CURVE_VALUE_MIN_Y * CE_GR_RESOLUTION),
		(LONG)(CE_CURVE_VALUE_MAX_Y * CE_GR_RESOLUTION)
	);
}



//---------------------------------------------------------------------
//		値を生成(1次元)
//---------------------------------------------------------------------
int ce::Curve_Value::create_value_1d()
{
	int result;
	int x1, y1, x2, y2;
	x1 = (int)std::round(ctpt[0].x * 100 / (double)CE_GR_RESOLUTION);
	y1 = MINMAXLIM((int)std::round(ctpt[0].y * 100 / (double)CE_GR_RESOLUTION), -273, 373);
	x2 = (int)std::round(ctpt[1].x * 100 / (double)CE_GR_RESOLUTION);
	y2 = MINMAXLIM((int)std::round(ctpt[1].y * 100 / (double)CE_GR_RESOLUTION), -273, 373);
	// 計算
	result = 6600047 * (y2 + 273) + 65347 * x2 + 101 * (y1 + 273) + x1 - 2147483647;
	return result;
}



//---------------------------------------------------------------------
//		値を生成(4次元)
//---------------------------------------------------------------------
std::string ce::Curve_Value::create_value_4d()
{
	float ptx, pty;
	std::string strx, stry, result;
	for (int i = 0; i < 2; i++) {
		ptx = (float)(std::round(ctpt[i].x * 100 / (double)CE_GR_RESOLUTION) * 0.01);
		pty = (float)(std::round(ctpt[i].y * 100 / (double)CE_GR_RESOLUTION) * 0.01);
		strx = std::to_string(ptx);
		stry = std::to_string(pty);
		strx.erase(4);
		if (ctpt[i].y < 0) stry.erase(5);
		else stry.erase(4);
		result += strx + ", " + stry + ", ";
	}
	result.erase(result.size() - 2, 2);
	return result;
}



//---------------------------------------------------------------------
//		1次元値を読み取る
//---------------------------------------------------------------------
void ce::Curve_Value::read_value_1d(int value)
{
	int64_t int64;
	int64 = (int64_t)value + (int64_t)2147483647;
	ctpt[1].y = (LONG)(int64 / 6600047);
	ctpt[1].x = (LONG)((int64 - (int64_t)ctpt[1].y * 6600047) / 65347);
	ctpt[0].y = (LONG)((int64 - ((int64_t)ctpt[1].y * 6600047 + (int64_t)ctpt[1].x * 65347)) / 101);
	ctpt[0].x = (LONG)((int64 - ((int64_t)ctpt[1].y * 6600047 + (int64_t)ctpt[1].x * 65347)) % 101);
	ctpt[0].x *= CE_GR_RESOLUTION / 100;
	ctpt[0].y *= CE_GR_RESOLUTION / 100;
	ctpt[1].x *= CE_GR_RESOLUTION / 100;
	ctpt[1].y *= CE_GR_RESOLUTION / 100;
	ctpt[0].y -= (LONG)(2.73 * CE_GR_RESOLUTION);
	ctpt[1].y -= (LONG)(2.73 * CE_GR_RESOLUTION);
}