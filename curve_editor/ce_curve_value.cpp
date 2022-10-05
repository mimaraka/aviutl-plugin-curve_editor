//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (Valueモードのカーブの関数)
//		VC++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void ce::Curve_Value::init()
{
	ctpt[0] = { (int)(CE_GR_RESOLUTION * 0.4), (int)(CE_GR_RESOLUTION * 0.4) };
	ctpt[1] = { (int)(CE_GR_RESOLUTION * 0.6), (int)(CE_GR_RESOLUTION * 0.6) };
}



//---------------------------------------------------------------------
//		指定した座標が制御点の内部に存在するか
//---------------------------------------------------------------------
int ce::Curve_Value::point_in_ctpts(POINT cl_pt)
{
	RECT rcCtpt1 = {
		to_client(ctpt[0]).x - CE_POINT_RANGE,
		to_client(ctpt[0]).y - CE_POINT_RANGE,
		to_client(ctpt[0]).x + CE_POINT_RANGE,
		to_client(ctpt[0]).y + CE_POINT_RANGE
	};
	RECT rcCtpt2 = {
		to_client(ctpt[1]).x - CE_POINT_RANGE,
		to_client(ctpt[1]).y - CE_POINT_RANGE,
		to_client(ctpt[1]).x + CE_POINT_RANGE,
		to_client(ctpt[1]).y + CE_POINT_RANGE
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
	//Altキーが押された直後
	if (GetAsyncKeyState(VK_MENU) < 0 && !bAltKey) {
		ptLock.x = gr_pt.x;
		ptLock.y = gr_pt.y;
		bAltKey = TRUE;
		bCtrlKey = FALSE;
		bShiftKey = FALSE;
	}
	//Altキーが押されている間
	else if (GetAsyncKeyState(VK_MENU) < 0) {
		//X
		g_curve_value.ctpt[index].x = MINMAXLIM(gr_pt.x, 0, CE_GR_RESOLUTION);
		//Y
		//Anti-ZeroDivisionError
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
				g_curve_value.ctpt[1].y
				= CE_GR_RESOLUTION - (int)((CE_GR_RESOLUTION - (int)g_curve_value.ctpt[1].x) * (CE_GR_RESOLUTION - ptLock.y) / (double)(CE_GR_RESOLUTION - ptLock.x));
		}
	}
	//同時に動かす
	else if (GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_CONTROL) < 0) {
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

	//The moment the Shift Key is pressed
	else if (GetAsyncKeyState(VK_SHIFT) < 0 && !bShiftKey) {
		ptLock.y = gr_pt.y;
		bShiftKey = TRUE;
		bCtrlKey = FALSE;
		bAltKey = FALSE;
	}
	//While the Shift Key is being pressed
	else if (GetAsyncKeyState(VK_SHIFT) < 0) {
		g_curve_value.ctpt[index].x = gr_pt.x;
		//if Y is larger than 500
		if (ptLock.y < CE_GR_RESOLUTION / 2)
			g_curve_value.ctpt[index].y = 0;
		//if Y is less than 500
		else if (ptLock.y >= CE_GR_RESOLUTION / 2)
			g_curve_value.ctpt[index].y = CE_GR_RESOLUTION;
	}
	//The moment the Control Key is pressed
	else if (GetAsyncKeyState(VK_CONTROL) < 0 && !bCtrlKey) {
		ptLock.x = gr_pt.x; ptLock.y = gr_pt.y;
		bCtrlKey = TRUE;
		bAltKey = FALSE;
		bShiftKey = FALSE;
	}
	//Ctrlキーが押されている間
	else if (GetAsyncKeyState(VK_CONTROL) < 0) {
		float theta;
		int intResult_x, intResult_y;
		if (!index) {
			theta = std::atan2(gr_pt.y, gr_pt.x);
			intResult_x = (int)(DISTANCE1(ptLock) * std::cos(theta));
			intResult_y = (int)(DISTANCE1(ptLock) * std::sin(theta));
		}
		else {
			theta = std::atan2(CE_GR_RESOLUTION - gr_pt.y, CE_GR_RESOLUTION - gr_pt.x);
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

	g_curve_value.ctpt[index].x = MINMAXLIM(g_curve_value.ctpt[index].x, 0, CE_GR_RESOLUTION);
	g_curve_value.ctpt[index].y = MINMAXLIM(g_curve_value.ctpt[index].y, -2.73 *CE_GR_RESOLUTION, 3.73 * CE_GR_RESOLUTION);
}