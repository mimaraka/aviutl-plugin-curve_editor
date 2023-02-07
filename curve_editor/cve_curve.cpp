//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (カーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void cve::Curve::init(Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	// 追加する制御点
	Curve_Points pt_add[2];
	
	pt_add[0].type = Curve_Points::Default_Left;
	pt_add[0].pt_center = { 0, 0 };
	pt_add[0].pt_right = {
		(int)(CVE_GRAPH_RESOLUTION * HANDLE_DEFAULT_1),
		(int)(CVE_GRAPH_RESOLUTION * HANDLE_DEFAULT_1)
	};
	pt_add[0].pt_left = { 0, 0 };

	points.PushBack(pt_add[0]);


	pt_add[1].type = Curve_Points::Default_Right;

	pt_add[1].pt_center = {
		CVE_GRAPH_RESOLUTION,
		CVE_GRAPH_RESOLUTION
	};

	pt_add[1].pt_left = {
		(int)(CVE_GRAPH_RESOLUTION * HANDLE_DEFAULT_2),
		(int)(CVE_GRAPH_RESOLUTION * HANDLE_DEFAULT_2)
	};

	pt_add[1].pt_right = {
		CVE_GRAPH_RESOLUTION,
		CVE_GRAPH_RESOLUTION
	};

	points.PushBack(pt_add[1]);
}



//---------------------------------------------------------------------
//		ポイントを移動
//---------------------------------------------------------------------
void cve::Curve_Type_ID::move_point(int index, const POINT& pt_graph, bool init)
{
	static POINT		handle_prev_right,
						handle_next_left,
						handle_left,
						handle_right;

	Point_Address		tmp;

	static double		agl_prev, agl_next,
						agl_left, agl_right;


	// 拡張制御点でない場合弾く
	if (ctpts[index].type != Curve_Points::Extended)
		return;
						

	// ハンドル位置&角度を記憶
	if (init) {
		handle_prev_right = ctpts[index - 1].pt_right;
		handle_next_left = ctpts[index + 1].pt_left;

		// 左右のハンドルの相対座標
		handle_left = {
			ctpts[index].pt_left.x - ctpts[index].pt_center.x,
			ctpts[index].pt_left.y - ctpts[index].pt_center.y,
		};
		handle_right = {
			ctpts[index].pt_right.x - ctpts[index].pt_center.x,
			ctpts[index].pt_right.y - ctpts[index].pt_center.y,
		};

		// 左隣のポイントの右ハンドルの角度を取得
		tmp.index = index - 1;
		tmp.position = Point_Address::Right;

		agl_prev = get_handle_angle(tmp);

		// 右隣のポイントの左ハンドルの角度を取得
		tmp.index = index + 1;
		tmp.position = Point_Address::Left;

		agl_next = get_handle_angle(tmp);

		// 左のハンドルの角度を取得
		tmp.index = index;
		tmp.position = Point_Address::Left;

		agl_left = get_handle_angle(tmp);

		// 右のハンドルの角度を取得
		tmp.index = index;
		tmp.position = Point_Address::Right;

		agl_right = get_handle_angle(tmp);
	}


	// 中央    ---o---
	ctpts[index].pt_center.x = std::clamp(
		pt_graph.x,
		ctpts[index - 1].pt_center.x + 1,
		ctpts[index + 1].pt_center.x - 1
	);

	ctpts[index].pt_center.y = pt_graph.y;

	// 左   O-----o--
	ctpts[index].pt_left.x = ctpts[index].pt_center.x + handle_left.x;
	ctpts[index].pt_left.y = pt_graph.y + handle_left.y;

	// 右       --o-----O
	ctpts[index].pt_right.x = ctpts[index].pt_center.x + handle_right.x;
	ctpts[index].pt_right.y = pt_graph.y + handle_right.y;


	// 左右両端のハンドル補正
	//左   O-----o--
	tmp.index = index;
	tmp.position = Point_Address::Left;
	correct_handle(tmp);

	// 右       --o-----O
	tmp.index = index;
	tmp.position = Point_Address::Right;
	correct_handle(tmp);

	ctpts[index - 1].pt_right = handle_prev_right;
	ctpts[index + 1].pt_left = handle_next_left;

	// 右       --o-----O
	// (前の制御点群)
	tmp.index = index - 1;
	tmp.position = Point_Address::Right;

	correct_handle(tmp);

	// 左   O-----o--
	// (次の制御点群)
	tmp.index = index + 1;
	tmp.position = Point_Address::Left;

	correct_handle(tmp);
}



//---------------------------------------------------------------------
//		ハンドルを移動
//---------------------------------------------------------------------
void cve::Curve::move_handle(const Point_Address& pt_address, const POINT& pt_graph, bool init)
{
	static bool shift_key, alt_key, ctrl_key, ctrl_shift_key;
	static POINT pt_lock;
	static double length, length_opposite;
	POINT pt_result;
	Point_Address tmp;
	int left_side, right_side, top, bottom;

	// キーの押下状態
	const bool move_symmetrically = ::GetAsyncKeyState(VK_SHIFT) < 0 && ::GetAsyncKeyState(VK_CONTROL) < 0;
	const bool is_angle_locked = ::GetAsyncKeyState(VK_MENU) < 0;
	const bool snapped = ::GetAsyncKeyState(VK_SHIFT) < 0 && ::GetAsyncKeyState(VK_CONTROL) >= 0;
	const bool is_length_locked = ::GetAsyncKeyState(VK_SHIFT) >= 0 && ::GetAsyncKeyState(VK_CONTROL) < 0;


	// 左右の点以外なら弾く
	if (pt_address.position != Point_Address::Left && pt_address.position != Point_Address::Right)
		return;


	// 記録
	if (init) {
		if (pt_address.position == Point_Address::Left)
			length = aului::distance(
				ctpts[pt_address.index].pt_center,
				ctpts[pt_address.index].pt_right
			);
		else if (pt_address.position == Point_Address::Right)
			length = aului::distance(
				ctpts[pt_address.index].pt_center,
				ctpts[pt_address.index].pt_left
			);
	}

	if (pt_address.position == Point_Address::Left) {
		left_side = ctpts[pt_address.index - 1].pt_center.x;
		right_side = ctpts[pt_address.index].pt_center.x;
		top = ctpts[pt_address.index].pt_center.y;
		bottom = ctpts[pt_address.index - 1].pt_center.y;
	}
	else if (pt_address.position == Point_Address::Right) {
		left_side = ctpts[pt_address.index].pt_center.x;
		right_side = ctpts[pt_address.index + 1].pt_center.x;
		top = ctpts[pt_address.index + 1].pt_center.y;
		bottom = ctpts[pt_address.index].pt_center.y;
	}


	// [Alt]キー
	if (is_angle_locked) {
		// 記録等の処理
		if (!alt_key) {
			alt_key = true;
			ctrl_key = false;
			shift_key = false;

			pt_lock = pt_graph;
		}

		pt_result.x = std::clamp(
			(int)pt_graph.x,
			left_side,
			right_side
		);

		if (pt_address.position == Point_Address::Left) {
			if (pt_lock.x <= left_side)
				pt_result.y = pt_graph.y;
			else
				pt_result.y = top - (int)((right_side - pt_result.x) *
					(top - pt_lock.y) / (double)(right_side - pt_lock.x));
		}
		else if (pt_address.position == Point_Address::Right) {
			if (pt_lock.x >= right_side)
				pt_result.y = pt_graph.y;
			else
				pt_result.y = bottom + (int)((pt_result.x - left_side) * (pt_lock.y - bottom) / (double)(pt_lock.x - left_side));
		}
	}
	// [Ctrl]キー
	else if (is_length_locked) {
		if (!ctrl_key) {
			ctrl_key = true;
			alt_key = false;
			shift_key = false;

			pt_lock = pt_graph;
		}

		float angle;
		int result_y;
		const double handle_len = aului::distance(pt_lock, ctpts[pt_address.index].pt_center);

		if (pt_address.position == Point_Address::Left) {
			angle = (float)std::atan2(top - pt_graph.y, right_side - pt_graph.x);

			pt_result.x = right_side - (int)(handle_len * std::cos(angle));

			result_y = top - (int)(handle_len * std::sin(angle));

			if (angle > std::numbers::pi * 0.5)
				pt_result.y = top - (int)(handle_len);

			else if (angle < -std::numbers::pi * 0.5)
				pt_result.y = top + (int)(handle_len);

			else
				pt_result.y = result_y;
		}
		else if (pt_address.position == Point_Address::Right) {
			angle = (float)std::atan2(pt_graph.y - bottom, pt_graph.x - left_side);

			pt_result.x = left_side + (int)(handle_len * std::cos(angle));

			result_y = bottom + (int)(handle_len * std::sin(angle));

			if (angle > std::numbers::pi * 0.5)
				pt_result.y = bottom + (int)(handle_len);

			else if (angle < -std::numbers::pi * 0.5)
				pt_result.y = bottom - (int)(handle_len);

			else
				pt_result.y = result_y;
		}
	}
	// [Shift]キー
	else if (snapped) {
		if (!shift_key) {
			shift_key = true;
			alt_key = false;
			ctrl_key = false;

			pt_lock = pt_graph;
		}

		pt_result.x = pt_graph.x;

		if (pt_lock.y >= (top + bottom) / 2)
			pt_result.y = top;
		else
			pt_result.y = bottom;
	}
	else {
		alt_key = false;
		ctrl_key = false;
		shift_key = false;

		pt_result = pt_graph;
	}


	// ハンドルの位置を設定
	set_handle_position(pt_address, pt_result, length, false, NULL);


	// 向かい側のハンドルと同時に動くとき
	if (move_symmetrically) {
		// 向かい側のハンドルの座標
		POINT pt_opposite;

		// 左   O-----o--
		if (pt_address.position == Point_Address::Left) {
			pt_opposite = {
				ctpts[pt_address.index - 1].pt_center.x + ctpts[pt_address.index].pt_center.x - pt_result.x,
				ctpts[pt_address.index - 1].pt_center.y + ctpts[pt_address.index].pt_center.y - pt_result.y,
			};

			// CtrlキーとShiftキーを押した瞬間
			if (!ctrl_shift_key) {
				ctrl_shift_key = true;

				// 向かいのハンドルのペアのハンドルの長さを記録
				length_opposite = aului::distance(
					ctpts[pt_address.index - 1].pt_center,
					ctpts[pt_address.index - 1].pt_left
				);
			};

			tmp.index = pt_address.index - 1;
			tmp.position = Point_Address::Right;
		}
		// 右       --o-----O
		else if (pt_address.position == Point_Address::Right) {
			pt_opposite = {
				ctpts[pt_address.index + 1].pt_center.x - (pt_result.x - ctpts[pt_address.index].pt_center.x),
				ctpts[pt_address.index + 1].pt_center.y - (pt_result.y - ctpts[pt_address.index].pt_center.y),
			};

			// CtrlキーとShiftキーを押した瞬間
			if (!ctrl_shift_key) {
				ctrl_shift_key = true;

				// 向かいのハンドルのペアのハンドルの長さを記録
				length_opposite = aului::distance(
					ctpts[pt_address.index + 1].pt_center,
					ctpts[pt_address.index + 1].pt_right
				);
			};

			tmp.index = pt_address.index + 1;
			tmp.position = Point_Address::Left;
		}

		// 向かい側のハンドルの位置を設定
		set_handle_position(tmp, pt_opposite, length_opposite, false, NULL);
	}
	else
		ctrl_shift_key = false;
}



//---------------------------------------------------------------------
//		ハンドルを実際に動かしているところ
//---------------------------------------------------------------------
void cve::Curve::set_handle_position(const Point_Address& pt_address, const POINT& pt_graph, double length, bool use_angle, double angle)
{
	Point_Address		tmp;
	double				agl_tmp;

	// 中央の点なら弾く
	if (pt_address.position == Point_Address::Center)
		return;


	switch (pt_address.position) {
		//左   O-----o--
	case Point_Address::Left:
		if (ctpts[pt_address.index].type == Curve_Points::Default_Left)
			return;

		// ハンドルを動かす
		ctpts[pt_address.index].pt_left.x = std::clamp(
			pt_graph.x,
			ctpts[pt_address.index - 1].pt_center.x,
			ctpts[pt_address.index].pt_center.x
		);
		ctpts[pt_address.index].pt_left.y = pt_graph.y;

		//整列(角度)
		if (g_config.align_handle && ctpts[pt_address.index].type == Curve_Points::Extended) {
			tmp.index = pt_address.index;
			tmp.position = Point_Address::Right;
			agl_tmp = use_angle ? angle : get_handle_angle(pt_address);

			set_handle_angle(tmp, agl_tmp + std::numbers::pi, TRUE, length);
		}
		break;

		//右       --o-----O
	case Point_Address::Right:
		if (ctpts[pt_address.index].type == Curve_Points::Default_Right)
			return;

		// ハンドルを動かす
		ctpts[pt_address.index].pt_right.x = std::clamp(
			pt_graph.x,
			ctpts[pt_address.index].pt_center.x,
			ctpts[pt_address.index + 1].pt_center.x
		);
		ctpts[pt_address.index].pt_right.y = pt_graph.y;

		//整列(角度)
		if (g_config.align_handle && ctpts[pt_address.index].type == Curve_Points::Extended) {
			tmp.index = pt_address.index;
			tmp.position = Point_Address::Left;
			agl_tmp = use_angle ? angle : get_handle_angle(pt_address);

			set_handle_angle(tmp, agl_tmp + std::numbers::pi, TRUE, length);
		}
		break;
	}
}



//---------------------------------------------------------------------
//		ポイント・ハンドルを追加
//---------------------------------------------------------------------
void cve::Curve_Type_ID::add_point(const POINT& pt_graph)
{
	int index = 0;

	// ・ポイントの個数が最大
	// ・追加するポイントのX座標が範囲外
	// のいずれかの場合
	if (
		ctpts.size >= CVE_POINT_MAX ||
		!aului::in_range((int)pt_graph.x, 0, CVE_GRAPH_RESOLUTION, false)
	)
		return;

	// 追加するポイントが左から何番目になるか
	for (int i = 0; i < (int)ctpts.size; i++) {
		// 既存のポイントとX座標が被っていた場合
		if (ctpts[i].pt_center.x == pt_graph.x)
			return;

		else if (ctpts[i].pt_center.x > pt_graph.x) {
			index = i;
			break;
		}
	}

	Curve_Points pt_add;
	pt_add.type = Curve_Points::Extended;
	pt_add.pt_center = pt_graph;

	//ビューの縮尺に合わせてハンドルのデフォルトの長さを変更
	pt_add.pt_left = {
		pt_graph.x - (int)(HANDLE_LENGTH_DEFAULT / g_view_info.scale.x),
		pt_graph.y
	};
	pt_add.pt_right = {
		pt_graph.x + (int)(HANDLE_LENGTH_DEFAULT / g_view_info.scale.x),
		pt_graph.y
	};

	// 制御点挿入
	ctpts.Insert(pt_add, index);

	//左右の点が両隣の中央の点より左/右に出ていたら修正
	if (ctpts[index].pt_left.x < ctpts[index - 1].pt_center.x)
		ctpts[index].pt_left.x = ctpts[index - 1].pt_center.x;

	if (ctpts[index].pt_right.x > ctpts[index + 1].pt_center.x)
		ctpts[index].pt_right.x = ctpts[index + 1].pt_center.x;

	Point_Address tmp;
	//両隣の左右の点が中央の点より右/左に出ていたら修正
	tmp.index = index - 1;
	tmp.position = Point_Address::Right;

	correct_handle(tmp);

	tmp.index = index + 1;
	tmp.position = Point_Address::Left;

	correct_handle(tmp);

}



//---------------------------------------------------------------------
//		ポイントを削除
//---------------------------------------------------------------------
void cve::Curve_Type_ID::delete_point(const POINT& pt_client)
{
	Point_Address pt_address;
	pt_on_ctpt(pt_client, &pt_address);
	if (pt_address.position == Point_Address::Null)
		return;
	for (int i = 1; i < (int)ctpts.size - 1; i++) {
		if (pt_address.index == i) {
			ctpts.Erase(i);
			break;
		}
	}
}



//---------------------------------------------------------------------
//		ポイントをクリア
//---------------------------------------------------------------------
void cve::Curve::clear(Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	points.Clear();
	init(points);
}



//---------------------------------------------------------------------
//		指定した座標がポイント・ハンドルの内部に存在しているか
//---------------------------------------------------------------------
void cve::Curve::pt_on_ctpt(const POINT& pt_client, Point_Address* pt_address, bool prioritize_point)
{
	RECT center, left, right;
	Point_Address result = {
		NULL,
		Point_Address::Null
	};

	for (int i = 0; i < (int)ctpts.size; i++) {
		center = {
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_center)).x - POINT_BOX_WIDTH,
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_center)).y - POINT_BOX_WIDTH,
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_center)).x + POINT_BOX_WIDTH,
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_center)).y + POINT_BOX_WIDTH
		};
		left = {
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_left)).x - POINT_BOX_WIDTH,
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_left)).y - POINT_BOX_WIDTH,
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_left)).x + POINT_BOX_WIDTH,
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_left)).y + POINT_BOX_WIDTH
		};
		right = {
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_right)).x - POINT_BOX_WIDTH,
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_right)).y - POINT_BOX_WIDTH,
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_right)).x + POINT_BOX_WIDTH,
			(LONG)to_client(aului::Point<LONG>(ctpts[i].pt_right)).y + POINT_BOX_WIDTH
		};

		bool is_on_point = ::PtInRect(&center, pt_client) && ctpts[i].type == Curve_Points::Extended;
		bool is_on_lhandle = ::PtInRect(&left, pt_client) && ctpts[i].type != Curve_Points::Default_Left;
		bool is_on_rhandle = ::PtInRect(&right, pt_client) && ctpts[i].type != Curve_Points::Default_Right;

		// ポイントとハンドルの両方の上にあったとき
		
		if (is_on_point || is_on_lhandle || is_on_rhandle) {
			if (is_on_point && (is_on_lhandle || is_on_rhandle)) {
				if (prioritize_point)
					result.position = Point_Address::Center;
				else {
					if (is_on_lhandle)
						result.position = Point_Address::Left;
					else
						result.position = Point_Address::Right;
				}
			}
			else if (is_on_point)
				result.position = Point_Address::Center;
			else if (is_on_lhandle)
				result.position = Point_Address::Left;
			else if (is_on_rhandle)
				result.position = Point_Address::Right;
			result.index = i;
			break;
		}
	}

	*pt_address = result;
}



//---------------------------------------------------------------------
//		ハンドルの角度を取得
//---------------------------------------------------------------------
double cve::Curve::get_handle_angle(const Point_Address& pt_address)
{
	double angle;
	int dstx, dsty;

	switch (pt_address.position) {
		// 左
	case Point_Address::Left:
		dstx = ctpts[pt_address.index].pt_left.x - ctpts[pt_address.index].pt_center.x;
		dsty = ctpts[pt_address.index].pt_left.y - ctpts[pt_address.index].pt_center.y;
		break;

		// 右
	case Point_Address::Right:
		dstx = ctpts[pt_address.index].pt_right.x - ctpts[pt_address.index].pt_center.x;
		dsty = ctpts[pt_address.index].pt_right.y - ctpts[pt_address.index].pt_center.y;
		break;

	default:
		return 0;
	}

	// x, yがともに0のとき
	if (dstx == 0 && dsty == 0) {
		// 左
		if (pt_address.position == Point_Address::Left)
			return std::numbers::pi;
		// 右
		else if (pt_address.position == Point_Address::Right)
			return 0;
	}

	angle = std::atan2(dsty, dstx);
	return angle;
}



//---------------------------------------------------------------------
//		ハンドルの角度を設定
//---------------------------------------------------------------------
void cve::Curve::set_handle_angle(const Point_Address& pt_address, double angle, bool use_length, double lgth)
{
	double length;

	//左
	if (pt_address.position == Point_Address::Left &&
		ctpts[pt_address.index].type != 0) {
		length = use_length ? lgth : aului::distance(
			ctpts[pt_address.index].pt_center,
			ctpts[pt_address.index].pt_left
		);

		ctpts[pt_address.index].pt_left.x =
			(LONG)(ctpts[pt_address.index].pt_center.x + std::cos(angle) * length);
		ctpts[pt_address.index].pt_left.y =
			(LONG)(ctpts[pt_address.index].pt_center.y + std::sin(angle) * length);

		correct_handle(pt_address);
	}
	//右
	else if (pt_address.position == Point_Address::Right &&
		ctpts[pt_address.index].type != 1) {
		length = use_length ? lgth : aului::distance(
			ctpts[pt_address.index].pt_center,
			ctpts[pt_address.index].pt_right
		);

		ctpts[pt_address.index].pt_right.x =
			(LONG)(ctpts[pt_address.index].pt_center.x + std::cos(angle) * length);
		ctpts[pt_address.index].pt_right.y =
			(LONG)(ctpts[pt_address.index].pt_center.y + std::sin(angle) * length);

		correct_handle(pt_address);
	}
	else return;
}



//---------------------------------------------------------------------
//		指定したアドレスのハンドル(右か左)が，前または次のポイントを超えている場合に，
//		ハンドルの角度を保ちながら修正する関数
//		pt_address:	指定する制御点のアドレス
//		angle:		設定する角度
//---------------------------------------------------------------------
void cve::Curve::correct_handle(const Point_Address& pt_address)
{
	double angle = get_handle_angle(pt_address);

	if (pt_address.position == Point_Address::Left) {
		//左の制御点が前の制御点群の中央の点より左側にあったとき
		if (ctpts[pt_address.index].pt_left.x < ctpts[pt_address.index - 1].pt_center.x) {
			ctpts[pt_address.index].pt_left.x = ctpts[pt_address.index - 1].pt_center.x;
			

			//角度を保つ
			ctpts[pt_address.index].pt_left.y = (LONG)
				(ctpts[pt_address.index].pt_center.y + std::tan(angle) *
					(ctpts[pt_address.index].pt_left.x - ctpts[pt_address.index].pt_center.x));
		}
	}
	else if (pt_address.position == Point_Address::Right) {
		if (ctpts[pt_address.index].pt_right.x > ctpts[pt_address.index + 1].pt_center.x) {
			ctpts[pt_address.index].pt_right.x = ctpts[pt_address.index + 1].pt_center.x;

			//角度を保つ
			ctpts[pt_address.index].pt_right.y = (LONG)
				(ctpts[pt_address.index].pt_center.y + std::tan(angle) *
					(ctpts[pt_address.index].pt_right.x - ctpts[pt_address.index].pt_center.x));
		}
	}
}



//---------------------------------------------------------------------
//		カーブを反転
//---------------------------------------------------------------------
void cve::Curve::reverse_curve()
{
	Static_Array<Curve_Points, CVE_POINT_MAX> ctpts_old;
	for (int i = 0; i < ctpts.size / 2.f; i++) {
		ctpts_old.PushBack(ctpts[i]);

		ctpts[i].pt_center = {
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_center.x,
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_center.y
		};

		ctpts[i].pt_left = {
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_right.x,
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_right.y
		};

		ctpts[i].pt_right = {
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_left.x,
			CVE_GRAPH_RESOLUTION - ctpts[ctpts.size - i - 1].pt_left.y
		};


		ctpts[ctpts.size - i - 1].pt_center = {
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_center.x,
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_center.y
		};

		ctpts[ctpts.size - i - 1].pt_left = {
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_right.x,
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_right.y
		};

		ctpts[ctpts.size - i - 1].pt_right = {
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_left.x,
			CVE_GRAPH_RESOLUTION - ctpts_old[i].pt_left.y
		};
	}
}



//---------------------------------------------------------------------
//		ベジェ曲線を描画
//---------------------------------------------------------------------
void cve::Curve::draw_bezier(
	aului::Direct2d_Paint_Object* paint_object,
	const aului::Point<float>& stpt,
	const aului::Point<float>& ctpt1,
	const aului::Point<float>& ctpt2,
	const aului::Point<float>& edpt,
	float thickness
)
{
	ID2D1GeometrySink* sink = nullptr;
	ID2D1PathGeometry* bezier = nullptr;
	ID2D1StrokeStyle* style = nullptr;

	g_p_factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.f,
			D2D1_DASH_STYLE_SOLID,
			0.f),
		NULL, NULL,
		&style
	);
	

	g_p_factory->CreatePathGeometry(&bezier);
	bezier->Open(&sink);
	sink->BeginFigure(D2D1::Point2F(stpt.x, stpt.y), D2D1_FIGURE_BEGIN_HOLLOW);
	sink->AddBezier(D2D1::BezierSegment(
		D2D1::Point2F(ctpt1.x, ctpt1.y),
		D2D1::Point2F(ctpt2.x, ctpt2.y),
		D2D1::Point2F(edpt.x, edpt.y)
	));
	sink->EndFigure(D2D1_FIGURE_END_OPEN);
	sink->Close();

	if (bezier)
		paint_object->p_render_target->DrawGeometry(bezier, paint_object->brush, thickness, style);

	paint_object->release(&sink);
	paint_object->release(&bezier);
	paint_object->release(&style);
}



//---------------------------------------------------------------------
//		ハンドルを描画
//---------------------------------------------------------------------
void cve::Curve::draw_handle(
	aului::Direct2d_Paint_Object* paint_object,
	const aului::Point<float>& st,
	const aului::Point<float>& ed,
	int drawing_mode,
	int draw_option)
{
	ID2D1StrokeStyle* style = nullptr;

	aului::Point<float> st_new = st;
	aului::Point<float> ed_new = ed;

	const float handle_thickness = (drawing_mode == DRAW_CURVE_NORMAL) ? HANDLE_THICKNESS : HANDLE_THICKNESS_PRESET;
	const float point_size = (drawing_mode == DRAW_CURVE_NORMAL) ? POINT_SIZE : POINT_SIZE_PRESET;
	const float handle_size = (drawing_mode == DRAW_CURVE_NORMAL) ? HANDLE_SIZE : HANDLE_SIZE_PRESET;
	const float handle_circle_line = (drawing_mode == DRAW_CURVE_NORMAL) ? HANDLE_BORDER_THICKNESS : HANDLE_BORDER_THICKNESS;

	if (drawing_mode == DRAW_CURVE_NORMAL) {
		subtract_length(&st_new, ed, st, draw_option == DRAW_HANDLE_ONLY ? CVE_SUBTRACT_LENGTH : CVE_SUBTRACT_LENGTH_2);
		subtract_length(&ed_new, st_new, ed, CVE_SUBTRACT_LENGTH);
	}

	g_p_factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.f,
			D2D1_DASH_STYLE_SOLID,
			0.f),
		NULL, NULL,
		&style
	);

	if (drawing_mode != DRAW_CURVE_TRACE) {
		if (draw_option != DRAW_POINT_ONLY) {
			// ハンドルの直線
			paint_object->p_render_target->DrawLine(
				D2D1::Point2F(st_new.x, st_new.y),
				D2D1::Point2F(ed_new.x, ed_new.y),
				paint_object->brush, handle_thickness
			);

			// ハンドルの先端
			paint_object->p_render_target->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(ed.x, ed.y),
					handle_size, handle_size),
				paint_object->brush, HANDLE_BORDER_THICKNESS
			);
		}

		// ハンドルの根元
		if (draw_option == DRAW_HANDLE_ONLY) {
			paint_object->p_render_target->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(st.x, st.y),
					handle_size, handle_size),
				paint_object->brush, HANDLE_BORDER_THICKNESS
			);
		}
		else {
			paint_object->p_render_target->FillEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(st.x, st.y),
					point_size, point_size),
				paint_object->brush
			);
		}
	}
	paint_object->release(&style);
}



//---------------------------------------------------------------------
//		ポイントに引かれる点線を描画
//---------------------------------------------------------------------
void cve::Curve::draw_dash_line(
	aului::Direct2d_Paint_Object* paint_object,
	const RECT& rect_wnd,
	int pt_idx)
{
	constexpr double DASH_CONTRAST = 3.;
	constexpr int DASH_LENGTH = 42;
	constexpr int DASH_BLANK_LENGTH = 24;
	constexpr float LINE_THICKNESS = .2f;
	ID2D1StrokeStyle* style_dash = nullptr;
	const float dashes[] = { DASH_LENGTH, DASH_BLANK_LENGTH };
	aului::Color dash_color;

	g_p_factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_FLAT,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.f,
			D2D1_DASH_STYLE_CUSTOM,
			0.f),
		dashes,
		ARRAYSIZE(dashes),
		&style_dash
	);

	// 端点以外の制御点に引かれる点線
	dash_color.invert();
	dash_color.change_contrast(DASH_CONTRAST);
	paint_object->brush->SetColor(D2D1::ColorF(dash_color.d2dcolor()));

	if (pt_idx > 0) {
		paint_object->p_render_target->DrawLine(
			D2D1::Point2F(to_client(aului::Point<float>(ctpts[pt_idx].pt_center)).x, 0.f),
			D2D1::Point2F(to_client(aului::Point<float>(ctpts[pt_idx].pt_center)).x, (float)rect_wnd.bottom),
			paint_object->brush, LINE_THICKNESS, style_dash
		);
	}
	paint_object->release(&style_dash);
}



//---------------------------------------------------------------------
//		カーブを描画
//---------------------------------------------------------------------
void cve::Curve::draw_curve(
	aului::Direct2d_Paint_Object* paint_object,
	const RECT& rect_wnd,
	int drawing_mode)
{
	aului::Color handle_color;
	aului::Color curve_color;

	const bool is_preset = drawing_mode == DRAW_CURVE_PRESET;

	if (drawing_mode == DRAW_CURVE_NORMAL) {
		handle_color = g_theme[g_config.theme].handle;
		curve_color = g_config.curve_color;
	}
	else if (drawing_mode == DRAW_CURVE_TRACE) {
		handle_color = g_theme[g_config.theme].handle;
		curve_color = g_theme[g_config.theme].curve_trace;
	}
	else {
		handle_color = g_theme[g_config.theme].handle_preset;
		curve_color = g_theme[g_config.theme].curve_preset;
	}


	for (int i = 0; i < (int)ctpts.size - 1; i++)
	{
		// 端点以外の制御点に引かれる点線
		if (drawing_mode == DRAW_CURVE_NORMAL) {
			draw_dash_line(paint_object, rect_wnd, i);
		}

		// ベジェ曲線を描画
		paint_object->brush->SetColor(D2D1::ColorF(curve_color.d2dcolor()));
		draw_bezier(paint_object,
			is_preset ? to_preset(aului::Point<float>(ctpts[i].pt_center)) : to_client(aului::Point<float>(ctpts[i].pt_center)),
			is_preset ? to_preset(aului::Point<float>(ctpts[i].pt_right)) : to_client(aului::Point<float>(ctpts[i].pt_right)),
			is_preset ? to_preset(aului::Point<float>(ctpts[i + 1].pt_left)) : to_client(aului::Point<float>(ctpts[i + 1].pt_left)),
			is_preset ? to_preset(aului::Point<float>(ctpts[i + 1].pt_center)) : to_client(aului::Point<float>(ctpts[i + 1].pt_center)),
			CURVE_THICKNESS
		);

		//ハンドルの描画
		if (g_config.show_handle) {
			paint_object->brush->SetColor(D2D1::ColorF(handle_color.d2dcolor()));
			draw_handle(paint_object,
				is_preset ? to_preset(aului::Point<float>(ctpts[i].pt_center)) : to_client(aului::Point<float>(ctpts[i].pt_center)),
				is_preset ? to_preset(aului::Point<float>(ctpts[i].pt_right)) : to_client(aului::Point<float>(ctpts[i].pt_right)),
				drawing_mode, NULL
			);
			draw_handle(paint_object,
				is_preset ? to_preset(aului::Point<float>(ctpts[i + 1].pt_center)) : to_client(aului::Point<float>(ctpts[i + 1].pt_center)),
				is_preset ? to_preset(aului::Point<float>(ctpts[i + 1].pt_left)) : to_client(aului::Point<float>(ctpts[i + 1].pt_left)),
				drawing_mode, NULL
			);
		}
	}
}



//---------------------------------------------------------------------
//		データが有効かどうか
//---------------------------------------------------------------------
bool cve::Curve::is_data_valid()
{
	return aului::in_range((int)ctpts.size, 2, CVE_POINT_MAX, true);
}



//---------------------------------------------------------------------
//		1次元値を読み取る
//---------------------------------------------------------------------
bool cve::Curve::read_number(int number, Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	// -2147483647 ~  -12368443：ベジェが使用
	//   -12368442 ~         -1：IDが使用
	//           0             ：不使用
	//           1 ~   12368442：IDが使用
	//    12368443 ~ 2147483646：ベジェが使用
	//  2147483647             ：不使用

	int64_t num;
	if (number <= -12368443) {
		num = (int64_t)number + (int64_t)2147483647;
	}
	else if (12368443 <= number && number <= 2147483646) {
		num = (int64_t)number + (int64_t)2122746762;
	}
	else return false;

	clear(points);

	points[1].pt_left.y = (int32_t)(num / 6600047);
	points[1].pt_left.x = (int32_t)((num - (int64_t)points[1].pt_left.y * 6600047) / 65347);
	points[0].pt_right.y = (int32_t)((num - ((int64_t)points[1].pt_left.y * 6600047 + (int64_t)points[1].pt_left.x * 65347)) / 101);
	points[0].pt_right.x = (int32_t)((num - ((int64_t)points[1].pt_left.y * 6600047 + (int64_t)points[1].pt_left.x * 65347)) % 101);
	points[0].pt_right.x *= CVE_GRAPH_RESOLUTION / 100;
	points[0].pt_right.y *= CVE_GRAPH_RESOLUTION / 100;
	points[1].pt_left.x *= CVE_GRAPH_RESOLUTION / 100;
	points[1].pt_left.y *= CVE_GRAPH_RESOLUTION / 100;
	points[0].pt_right.y -= (int32_t)(2.73 * CVE_GRAPH_RESOLUTION);
	points[1].pt_left.y -= (int32_t)(2.73 * CVE_GRAPH_RESOLUTION);

	return true;
}



//---------------------------------------------------------------------
//		ベジェの値を計算
//---------------------------------------------------------------------
double cve::Curve::get_bezier_value(double ratio, Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	// 進捗が0~1の範囲外であった場合
	if (!aului::in_range(ratio, 0., 1., true))
		return 0;

	// 進捗に相当する区間を調べる
	for (int i = 0; i < (int)points.size - 1; i++) {
		if (aului::in_range(ratio, points[i].pt_center.x / (double)CVE_GRAPH_RESOLUTION, points[i + 1].pt_center.x / (double)CVE_GRAPH_RESOLUTION, true)) {
			// 区間の長さ(ratioと同じスケール)
			double range = (points[i + 1].pt_center.x - points[i].pt_center.x) / (double)CVE_GRAPH_RESOLUTION;
			// 区間ごとの進捗の相対値(0~1)
			double ratio2 = (ratio - points[i].pt_center.x / (double)CVE_GRAPH_RESOLUTION) / range;
			// 区間ごとの制御点1のX座標(相対値、0~1)
			double x1 = (points[i].pt_right.x - points[i].pt_center.x) / (double)(points[i + 1].pt_center.x - points[i].pt_center.x);
			// 区間ごとの制御点1のY座標(実際の値)
			double y1 = points[i].pt_right.y - points[i].pt_center.y / (double)(points[i + 1].pt_center.y - points[i].pt_center.y);
			// 区間ごとの制御点2のX座標(相対値、0~1)
			double x2 = (points[i + 1].pt_left.x - points[i].pt_center.x) / (double)(points[i + 1].pt_center.x - points[i].pt_center.x);
			// 区間ごとの制御点2のY座標(相対値)
			double y2 = points[i + 1].pt_left.y - points[i].pt_center.y / (double)(points[i + 1].pt_center.y - points[i].pt_center.y);

			// 区間ごとの始値、終値(相対値ではなく、グラフの値)
			double st2 = points[i].pt_center.y;
			double ed2 = points[i + 1].pt_center.y;

			// ベジェの計算
			double tl = 0;
			double tr = 1;
			double ta = .5 * (tl + tr);
			double xta;
			for (int j = 0; j < 10; j++) {
				xta = (1 - 3 * x2 + 3 * x1) * std::pow(ta, 3) + (x2 - 2 * x1) * 3 * std::pow(ta, 2) + 3 * x1 * ta;
				if (ratio2 < xta) tr = ta;
				else tl = ta;
				ta = .5 * (tl + tr);
			}
			return std::pow(ta, 3) * (ed2 - st2 - 3 * y2 + 3 * y1) + 3 * std::pow(ta, 2) * (y2 - 2 * y1 + st2) + 3 * ta * (y1 - st2) + st2;
		}
	}
	return 0;
}