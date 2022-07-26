//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (カーブ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "cve_header.hpp"

#define CVE_GR_POINT_LINE_THICKNESS		0.2f
#define CVE_GRAPH_POINT_CONTRAST		3
#define CVE_GRAPH_POINT_DASH			42
#define CVE_GRAPH_POINT_DASH_BLANK		24



//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
void cve::Curve::initialize(Static_Array<Curve_Points, CVE_POINT_MAX>& points)
{
	// 追加する制御点
	Curve_Points pt_add[2];
	
	pt_add[0].type = Curve_Points::Default_Left;
	pt_add[0].pt_center = { 0, 0 };
	pt_add[0].pt_right = {
		(int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_1),
		(int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_1)
	};
	pt_add[0].pt_left = { 0, 0 };

	points.PushBack(pt_add[0]);


	pt_add[1].type = Curve_Points::Default_Right;

	pt_add[1].pt_center = {
		CVE_GRAPH_RESOLUTION,
		CVE_GRAPH_RESOLUTION
	};

	pt_add[1].pt_left = {
		(int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_2),
		(int)(CVE_GRAPH_RESOLUTION * CVE_POINT_DEFAULT_2)
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
	ctpts[index].pt_center.x = MINMAX_LIMIT(
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
	correct_handle(tmp, agl_left);

	// 右       --o-----O
	tmp.index = index;
	tmp.position = Point_Address::Right;
	correct_handle(tmp, agl_right);

	ctpts[index - 1].pt_right = handle_prev_right;
	ctpts[index + 1].pt_left = handle_next_left;

	// 右       --o-----O
	// (前の制御点群)
	tmp.index = index - 1;
	tmp.position = Point_Address::Right;

	correct_handle(tmp, agl_prev);

	// 左   O-----o--
	// (次の制御点群)
	tmp.index = index + 1;
	tmp.position = Point_Address::Left;

	correct_handle(tmp, agl_next);
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

	// その時点のキーの押下状態を調べることができる
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
			length = DISTANCE(
				ctpts[pt_address.index].pt_center,
				ctpts[pt_address.index].pt_right
			);
		else if (pt_address.position == Point_Address::Right)
			length = DISTANCE(
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


	// Altキー
	if (is_angle_locked) {
		// 記録等の処理
		if (!alt_key) {
			alt_key = true;
			ctrl_key = false;
			shift_key = false;

			pt_lock = pt_graph;
		}

		pt_result.x = MINMAX_LIMIT(
			pt_graph.x,
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
	// Ctrlキー
	else if (is_length_locked) {
		if (!ctrl_key) {
			ctrl_key = true;
			alt_key = false;
			shift_key = false;

			pt_lock = pt_graph;
		}

		float angle;
		int result_y;
		const double handle_len = DISTANCE(pt_lock, ctpts[pt_address.index].pt_center);

		if (pt_address.position == Point_Address::Left) {
			angle = (float)std::atan2(top - pt_graph.y, right_side - pt_graph.x);

			pt_result.x = right_side - (int)(handle_len * std::cos(angle));

			result_y = top - (int)(handle_len * std::sin(angle));

			if (angle > CVE_MATH_PI * 0.5)
				pt_result.y = top - (int)(handle_len);

			else if (angle < -CVE_MATH_PI * 0.5)
				pt_result.y = top + (int)(handle_len);

			else
				pt_result.y = result_y;
		}
		else if (pt_address.position == Point_Address::Right) {
			angle = (float)std::atan2(pt_graph.y - bottom, pt_graph.x - left_side);

			pt_result.x = left_side + (int)(handle_len * std::cos(angle));

			result_y = bottom + (int)(handle_len * std::sin(angle));

			if (angle > CVE_MATH_PI * 0.5)
				pt_result.y = bottom + (int)(handle_len);

			else if (angle < -CVE_MATH_PI * 0.5)
				pt_result.y = bottom - (int)(handle_len);

			else
				pt_result.y = result_y;
		}
	}
	// Shiftキー
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
				length_opposite = DISTANCE(
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
				length_opposite = DISTANCE(
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
	double				agl_tmp = use_angle ? angle : get_handle_angle(pt_address);

	// 中央の点なら弾く
	if (pt_address.position == Point_Address::Center)
		return;


	switch (pt_address.position) {
		//左   O-----o--
	case Point_Address::Left:
		if (ctpts[pt_address.index].type == Curve_Points::Default_Left)
			return;

		// ハンドルを動かす
		ctpts[pt_address.index].pt_left.x = MINMAX_LIMIT(
			pt_graph.x,
			ctpts[pt_address.index - 1].pt_center.x,
			ctpts[pt_address.index].pt_center.x
		);
		ctpts[pt_address.index].pt_left.y = pt_graph.y;

		//整列(角度)
		if (g_config.align_handle && ctpts[pt_address.index].type == Curve_Points::Extended) {
			tmp.index = pt_address.index;
			tmp.position = Point_Address::Right;

			set_handle_angle(tmp, agl_tmp + CVE_MATH_PI, TRUE, length);
		}
		break;

		//右       --o-----O
	case Point_Address::Right:
		if (ctpts[pt_address.index].type == Curve_Points::Default_Right)
			return;

		// ハンドルを動かす
		ctpts[pt_address.index].pt_right.x = MINMAX_LIMIT(
			pt_graph.x,
			ctpts[pt_address.index].pt_center.x,
			ctpts[pt_address.index + 1].pt_center.x
		);
		ctpts[pt_address.index].pt_right.y = pt_graph.y;

		//整列(角度)
		if (g_config.align_handle && ctpts[pt_address.index].type == Curve_Points::Extended) {
			tmp.index = pt_address.index;
			tmp.position = Point_Address::Left;

			set_handle_angle(tmp, agl_tmp + CVE_MATH_PI, TRUE, length);
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
	if (ctpts.size >= CVE_POINT_MAX ||
		!ISINRANGE(pt_graph.x, 0, CVE_GRAPH_RESOLUTION))
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
		pt_graph.x - (int)(CVE_HANDLE_LENGTH_DEFAULT / g_view_info.scale.x),
		pt_graph.y
	};
	pt_add.pt_right = {
		pt_graph.x + (int)(CVE_HANDLE_LENGTH_DEFAULT / g_view_info.scale.x),
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

	correct_handle(tmp, get_handle_angle(tmp));

	tmp.index = index + 1;
	tmp.position = Point_Address::Left;

	correct_handle(tmp, get_handle_angle(tmp));

}



//---------------------------------------------------------------------
//		ポイントを削除
//---------------------------------------------------------------------
void cve::Curve_Type_ID::delete_point(const POINT& pt_client)
{
	Point_Address pt_address;
	pt_in_ctpt(pt_client, &pt_address);
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
	initialize(points);
}



//---------------------------------------------------------------------
//		指定した座標がポイント・ハンドルの内部に存在しているか
//---------------------------------------------------------------------
void cve::Curve::pt_in_ctpt(const POINT& pt_client, Point_Address* pt_address)
{
	RECT center, left, right;

	for (int i = 0; i < (int)ctpts.size; i++) {
		center = {
			(LONG)to_client(ctpts[i].pt_center).x - CVE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_center).y - CVE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_center).x + CVE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_center).y + CVE_POINT_RANGE
		};
		left = {
			(LONG)to_client(ctpts[i].pt_left).x - CVE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_left).y - CVE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_left).x + CVE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_left).y + CVE_POINT_RANGE
		};
		right = {
			(LONG)to_client(ctpts[i].pt_right).x - CVE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_right).y - CVE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_right).x + CVE_POINT_RANGE,
			(LONG)to_client(ctpts[i].pt_right).y + CVE_POINT_RANGE
		};

		if (PtInRect(&left, pt_client) && ctpts[i].type != Curve_Points::Default_Left) {
			pt_address->index = i;
			pt_address->position = Point_Address::Left;
			return;
		}
		else if (PtInRect(&right, pt_client) && ctpts[i].type != Curve_Points::Default_Right) {
			pt_address->index = i;
			pt_address->position = Point_Address::Right;
			return;
		}
		else if (PtInRect(&center, pt_client) && ctpts[i].type == Curve_Points::Extended) {
			pt_address->index = i;
			pt_address->position = Point_Address::Center;
			return;
		}
	}
	pt_address->index = NULL;
	pt_address->position = Point_Address::Null;
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
			return CVE_MATH_PI;
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
		length = use_length ? lgth : DISTANCE(
			ctpts[pt_address.index].pt_center,
			ctpts[pt_address.index].pt_left
		);

		ctpts[pt_address.index].pt_left.x =
			(LONG)(ctpts[pt_address.index].pt_center.x + std::cos(angle) * length);
		ctpts[pt_address.index].pt_left.y =
			(LONG)(ctpts[pt_address.index].pt_center.y + std::sin(angle) * length);

		correct_handle(pt_address, angle);
	}
	//右
	else if (pt_address.position == Point_Address::Right &&
		ctpts[pt_address.index].type != 1) {
		length = use_length ? lgth : DISTANCE(
			ctpts[pt_address.index].pt_center,
			ctpts[pt_address.index].pt_right
		);

		ctpts[pt_address.index].pt_right.x =
			(LONG)(ctpts[pt_address.index].pt_center.x + std::cos(angle) * length);
		ctpts[pt_address.index].pt_right.y =
			(LONG)(ctpts[pt_address.index].pt_center.y + std::sin(angle) * length);

		correct_handle(pt_address, angle);
	}
	else return;
}



//---------------------------------------------------------------------
//		指定したアドレスのハンドル(右か左)が，前または次のポイントを超えている場合に，
//		ハンドルの角度を保ちながら修正する関数
//		pt_address:	指定する制御点のアドレス
//		angle:		設定する角度
//---------------------------------------------------------------------
void cve::Curve::correct_handle(const Point_Address& pt_address, double angle)
{

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
	for (int i = 0; i < ctpts.size / 2.0f; i++) {
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
	Bitmap_Buffer* bitmap_buffer,
	const Float_Point& stpt,
	const Float_Point& ctpt1,
	const Float_Point& ctpt2,
	const Float_Point& edpt,
	float thickness
)
{
	ID2D1GeometrySink* sink;
	ID2D1PathGeometry* bezier;
	static ID2D1StrokeStyle* style = nullptr;

	if (style == nullptr) {
		g_d2d1_factory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_ROUND,
				D2D1_CAP_STYLE_ROUND,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_SOLID,
				0.0f),
			NULL, NULL,
			&style
		);
	}

	g_d2d1_factory->CreatePathGeometry(&bezier);
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
		g_render_target->DrawGeometry(bezier, bitmap_buffer->brush, thickness, style);
}



//---------------------------------------------------------------------
//		ハンドルを描画
//---------------------------------------------------------------------
void cve::Curve::draw_handle(
	Bitmap_Buffer* bitmap_buffer,
	const Float_Point& st,
	const Float_Point& ed,
	int drawing_mode,
	int draw_option)
{
	static ID2D1StrokeStyle* style = nullptr;

	Float_Point st_new = st;
	Float_Point ed_new = ed;

	const float handle_thickness = (drawing_mode == CVE_DRAW_CURVE_REGULAR) ? CVE_HANDLE_THICKNESS : CVE_HANDLE_THICKNESS_PRESET;
	const float point_size = (drawing_mode == CVE_DRAW_CURVE_REGULAR) ? CVE_POINT_SIZE : CVE_PONINT_SIZE_PRESET;
	const float handle_size = (drawing_mode == CVE_DRAW_CURVE_REGULAR) ? CVE_HANDLE_SIZE : CVE_HANDLE_SIZE_PRESET;
	const float handle_circle_line = (drawing_mode == CVE_DRAW_CURVE_REGULAR) ? CVE_HANDLE_CIRCLE_LINE : CVE_HANDLE_CIRCLE_LINE_PRESET;

	if (drawing_mode == CVE_DRAW_CURVE_REGULAR) {
		subtract_length(&st_new, ed, st, draw_option == CVE_DRAW_HANDLE_ONLY ? CVE_SUBTRACT_LENGTH : CVE_SUBTRACT_LENGTH_2);
		subtract_length(&ed_new, st_new, ed, CVE_SUBTRACT_LENGTH);
	}

	g_d2d1_factory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			D2D1_DASH_STYLE_SOLID,
			0.0f),
		NULL, NULL,
		&style
	);

	if (drawing_mode != CVE_DRAW_CURVE_TRACE) {
		if (draw_option != CVE_DRAW_POINT_ONLY) {
			// ハンドルの直線
			g_render_target->DrawLine(
				D2D1::Point2F(st_new.x, st_new.y),
				D2D1::Point2F(ed_new.x, ed_new.y),
				bitmap_buffer->brush, handle_thickness
			);

			// ハンドルの先端
			g_render_target->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(ed.x, ed.y),
					handle_size, handle_size),
				bitmap_buffer->brush, CVE_HANDLE_CIRCLE_LINE
			);
		}

		// ハンドルの根元
		if (draw_option == CVE_DRAW_HANDLE_ONLY) {
			g_render_target->DrawEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(st.x, st.y),
					handle_size, handle_size),
				bitmap_buffer->brush, CVE_HANDLE_CIRCLE_LINE
			);
		}
		else {
			g_render_target->FillEllipse(
				D2D1::Ellipse(
					D2D1::Point2F(st.x, st.y),
					point_size, point_size),
				bitmap_buffer->brush
			);
		}
	}
}



//---------------------------------------------------------------------
//		ポイントに引かれる点線を描画
//---------------------------------------------------------------------
void cve::Curve::draw_dash_line(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int pt_idx)
{
	static ID2D1StrokeStyle* style_dash;
	const float dashes[] = { CVE_GRAPH_POINT_DASH, CVE_GRAPH_POINT_DASH_BLANK };

	if (style_dash == nullptr) {
		g_d2d1_factory->CreateStrokeStyle(
			D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_FLAT,
				D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_MITER,
				10.0f,
				D2D1_DASH_STYLE_CUSTOM,
				0.0f),
			dashes,
			ARRAYSIZE(dashes),
			&style_dash
		);
	}

	// 端点以外の制御点に引かれる点線
	bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(CONTRAST(INVERT(g_theme[g_config.theme].bg_graph), CVE_GRAPH_POINT_CONTRAST))));

	if (pt_idx > 0) {
		g_render_target->DrawLine(
			D2D1::Point2F(to_client(ctpts[pt_idx].pt_center).x, 0),
			D2D1::Point2F(to_client(ctpts[pt_idx].pt_center).x, (float)rect_wnd.bottom),
			bitmap_buffer->brush, CVE_GR_POINT_LINE_THICKNESS, style_dash
		);
	}

}



//---------------------------------------------------------------------
//		カーブを描画
//---------------------------------------------------------------------
void cve::Curve::draw_curve(Bitmap_Buffer* bitmap_buffer, const RECT& rect_wnd, int drawing_mode)
{
	COLORREF handle_color;
	COLORREF curve_color;

	const bool is_preset = drawing_mode == CVE_DRAW_CURVE_PRESET;


	if (drawing_mode == CVE_DRAW_CURVE_REGULAR) {
		handle_color = g_theme[g_config.theme].handle;
		curve_color = g_config.curve_color;
	}
	else if (drawing_mode == CVE_DRAW_CURVE_TRACE) {
		handle_color = g_theme[g_config.theme].handle;
		curve_color = g_theme[g_config.theme].curve_trace;
	}
	else {
		handle_color = g_theme[g_config.theme].handle_preset;
		curve_color = g_theme[g_config.theme].curve_prset;
	}


	for (int i = 0; i < (int)ctpts.size - 1; i++)
	{
		// 端点以外の制御点に引かれる点線
		if (drawing_mode == CVE_DRAW_CURVE_REGULAR) {
			draw_dash_line(bitmap_buffer, rect_wnd, i);
		}

		// ベジェ曲線を描画
		bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(curve_color)));
		draw_bezier(bitmap_buffer,
			is_preset ? to_preset(ctpts[i].pt_center) : to_client(ctpts[i].pt_center),
			is_preset ? to_preset(ctpts[i].pt_right) : to_client(ctpts[i].pt_right),
			is_preset ? to_preset(ctpts[i + 1].pt_left) : to_client(ctpts[i + 1].pt_left),
			is_preset ? to_preset(ctpts[i + 1].pt_center) : to_client(ctpts[i + 1].pt_center),
			CVE_CURVE_THICKNESS
		);

		//ハンドルの描画
		if (g_config.show_handle) {
			bitmap_buffer->brush->SetColor(D2D1::ColorF(TO_BGR(handle_color)));
			draw_handle(bitmap_buffer,
				is_preset ? to_preset(ctpts[i].pt_center) : to_client(ctpts[i].pt_center),
				is_preset ? to_preset(ctpts[i].pt_right) : to_client(ctpts[i].pt_right),
				drawing_mode, NULL
			);
			draw_handle(bitmap_buffer,
				is_preset ? to_preset(ctpts[i + 1].pt_center) : to_client(ctpts[i + 1].pt_center),
				is_preset ? to_preset(ctpts[i + 1].pt_left) : to_client(ctpts[i + 1].pt_left),
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
	return ISINRANGEEQ(ctpts.size, 2, CVE_POINT_MAX);
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
	if (!ISINRANGEEQ(ratio, 0, 1))
		return 0;

	// 進捗に相当する区間を調べる
	for (int i = 0; i < (int)points.size - 1; i++) {
		if (ISINRANGEEQ(ratio, points[i].pt_center.x / (double)CVE_GRAPH_RESOLUTION, points[i + 1].pt_center.x / (double)CVE_GRAPH_RESOLUTION)) {
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