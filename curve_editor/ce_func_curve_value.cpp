//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (Valueモードのカーブの関数)
//		VC++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"


//---------------------------------------------------------------------
//		PtInCtpt
//---------------------------------------------------------------------
int ce::Curve_Value::PtInCtpt(POINT cl_pt)
{
	RECT rcCtpt1 = {
		ToClient(ctpt[0]).x - CE_POINT_RANGE,
		ToClient(ctpt[0]).y - CE_POINT_RANGE,
		ToClient(ctpt[0]).x + CE_POINT_RANGE,
		ToClient(ctpt[0]).y + CE_POINT_RANGE
	};
	RECT rcCtpt2 = {
		ToClient(ctpt[1]).x - CE_POINT_RANGE,
		ToClient(ctpt[1]).y - CE_POINT_RANGE,
		ToClient(ctpt[1]).x + CE_POINT_RANGE,
		ToClient(ctpt[1]).y + CE_POINT_RANGE
	};
	if (PtInRect(&rcCtpt2, cl_pt))
		return 2;
	else if (PtInRect(&rcCtpt1, cl_pt))
		return 1;
	else return 0;
}


//---------------------------------------------------------------------
//		Move Point
//---------------------------------------------------------------------
void ce::Curve_Value::MovePoint(int index, POINT gr_pt)
{
	constexpr double MATH_PI = 3.14159265;
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
		g_curve_value.ctpt[index].x = MINMAXLIM(gr_pt.x, 0, CE_GR_RES);
		//Y
		//Anti-ZeroDivisionError
		if (!index) {
			if (ptLock.x < 0)
				g_curve_value.ctpt[0].y = gr_pt.y;
			else
				g_curve_value.ctpt[0].y = (int)(g_curve_value.ctpt[0].x * ptLock.y / (double)ptLock.x);
		}
		else {
			if (ptLock.x > CE_GR_RES)
				g_curve_value.ctpt[1].y = gr_pt.y;
			else
				g_curve_value.ctpt[1].y
				= CE_GR_RES - (int)((CE_GR_RES - (int)g_curve_value.ctpt[1].x) * (CE_GR_RES - ptLock.y) / (double)(CE_GR_RES - ptLock.x));
		}
	}
	//同時に動かす
	else if (GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_CONTROL) < 0) {
		//X
		g_curve_value.ctpt[index].x = gr_pt.x;
		g_curve_value.ctpt[!index].x = MINMAXLIM(CE_GR_RES - g_curve_value.ctpt[index].x, 0, CE_GR_RES);
		//Y
		g_curve_value.ctpt[index].y = gr_pt.y;
		g_curve_value.ctpt[!index].y = CE_GR_RES - g_curve_value.ctpt[index].y;
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
		if (ptLock.y < CE_GR_RES / 2)
			g_curve_value.ctpt[index].y = 0;
		//if Y is less than 500
		else if (ptLock.y >= CE_GR_RES / 2)
			g_curve_value.ctpt[index].y = CE_GR_RES;
	}
	//The moment the Control Key is pressed
	else if (GetAsyncKeyState(VK_CONTROL) < 0 && !bCtrlKey) {
		ptLock.x = gr_pt.x; ptLock.y = gr_pt.y;
		bCtrlKey = TRUE;
		bAltKey = FALSE;
		bShiftKey = FALSE;
	}
	//While the Control Key is being pressed
	else if (GetAsyncKeyState(VK_CONTROL) < 0) {
		float theta;
		int intResult_x, intResult_y;
		if (!index) {
			theta = std::atan2(gr_pt.y, gr_pt.x);
			intResult_x = (int)(DISTANCE1(ptLock) * std::cos(theta));
			intResult_y = (int)(DISTANCE1(ptLock) * std::sin(theta));
		}
		else {
			theta = std::atan2(CE_GR_RES - gr_pt.y, CE_GR_RES - gr_pt.x);
			intResult_x = CE_GR_RES - (int)(DISTANCE2(ptLock, CE_GR_RES, CE_GR_RES) * std::cos(theta));
			intResult_y = CE_GR_RES - (int)(DISTANCE2(ptLock, CE_GR_RES, CE_GR_RES) * std::sin(theta));
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
				g_curve_value.ctpt[1].y = CE_GR_RES - (int)(DISTANCE2(ptLock, CE_GR_RES, CE_GR_RES));

			else if (theta < -MATH_PI * 0.5)
				g_curve_value.ctpt[1].y = CE_GR_RES + (int)(DISTANCE2(ptLock, CE_GR_RES, CE_GR_RES));

			else g_curve_value.ctpt[1].y = intResult_y;
		}
	}
	//Nomal Mode
	else {
		g_curve_value.ctpt[index] = gr_pt;
		bCtrlKey = FALSE;
		bAltKey = FALSE;
		bShiftKey = FALSE;
	}

	g_curve_value.ctpt[index].x = MINMAXLIM(g_curve_value.ctpt[index].x, 0, CE_GR_RES);
}