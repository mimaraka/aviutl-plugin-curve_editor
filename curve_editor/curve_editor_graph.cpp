#include "curve_editor_graph.hpp"
#include "config.hpp"
#include "enum.hpp"



namespace cved {
	namespace global {
		GraphCurveEditor::GraphCurveEditor() :
			curves_normal_{},
			curves_value_{},
			curve_bezier_{ mkaul::Point{0., 0.}, mkaul::Point{1., 1.}, true},
			curve_elastic_{ mkaul::Point{0., 0.}, mkaul::Point{1., 0.5}, true },
			curve_bounce_{ mkaul::Point{0., 0.}, mkaul::Point{1., 1.}, true }
		{
			reset_id_curves();
			curve_bezier_.clear();
			curve_elastic_.clear();
			curve_bounce_.clear();
		}

		bool GraphCurveEditor::set_idx_normal(size_t idx) noexcept {
			if (idx < curves_normal_.size()) {
				idx_normal_ = idx;
				return true;
			}
			else if (idx == curves_normal_.size()) {
				curves_normal_.emplace_back(NormalCurve{});
				idx_normal_ = idx;
				return true;
			}
			else return false;
		}

		bool GraphCurveEditor::set_idx_value(size_t idx) noexcept {
			if (idx < curves_value_.size()) {
				idx_value_ = idx;
				return true;
			}
			else if (idx == curves_value_.size()) {
				curves_value_.emplace_back(ValueCurve{});
				idx_value_ = idx;
				return true;
			}
			else return false;
		}

		void GraphCurveEditor::jump_to_last_idx_normal() noexcept {
			idx_normal_ = curves_normal_.size() - 1;
		}

		void GraphCurveEditor::jump_to_last_idx_value() noexcept {
			idx_value_ = curves_value_.size() - 1;
		}

		void GraphCurveEditor::delete_last_idx_normal() noexcept {
			if (1u < curves_normal_.size()) {
				curves_normal_.pop_back();
				if (idx_normal_ == curves_normal_.size()) {
					idx_normal_ = curves_normal_.size() - 1;
				}
			}
		}

		void GraphCurveEditor::delete_last_idx_value() noexcept {
			if (1u < curves_value_.size()) {
				curves_value_.pop_back();
				if (idx_value_ == curves_value_.size()) {
					idx_value_ = curves_value_.size() - 1;
				}
			}
		}

		void GraphCurveEditor::reset_id_curves() noexcept {
			curves_normal_.clear();
			curves_normal_.emplace_back(NormalCurve{});
			curves_value_.clear();
			curves_value_.emplace_back(ValueCurve{});
			idx_normal_ = 0u;
			idx_value_ = 0u;
		}

		bool GraphCurveEditor::load_codes(int32_t code_bezier, int32_t code_elastic, int32_t code_bounce) noexcept {
			bool result_bezier = curve_bezier_.decode(code_bezier);
			bool result_elastic = curve_elastic_.decode(code_elastic);
			bool result_bounce = curve_bounce_.decode(code_bounce);
			return result_bezier and result_elastic and result_bounce;
		}

		NormalCurve* GraphCurveEditor::curve_normal(size_t idx) noexcept {
			if (idx < curves_normal_.size()) {
				return &curves_normal_[idx];
			}
			else return nullptr;
		}

		ValueCurve* GraphCurveEditor::curve_value(size_t idx) noexcept {
			if (idx < curves_value_.size()) {
				return &curves_value_[idx];
			}
			else return nullptr;
		}

		GraphCurve* GraphCurveEditor::get_curve(EditMode mode) noexcept {
			switch (mode) {
			case EditMode::Bezier:
				return &curve_bezier_;

			case EditMode::Normal:
				return &curves_normal_[idx_normal_];

			case EditMode::Value:
				return &curves_value_[idx_value_];

			case EditMode::Elastic:
				return &curve_elastic_;

			case EditMode::Bounce:
				return &curve_bounce_;

			default:
				return nullptr;
			}
		}

		GraphCurve* GraphCurveEditor::current_curve() noexcept {
			return get_curve(global::config.get_edit_mode());
		}

		NumericGraphCurve* GraphCurveEditor::numeric_curve() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Bezier:
				return &curve_bezier_;

			case EditMode::Elastic:
				return &curve_elastic_;

			case EditMode::Bounce:
				return &curve_bounce_;

			default:
				return nullptr;
			}
		}

		// v1.xのカーブのデータを読み取り
		bool GraphCurveEditor::load_v1_data(const byte* data) noexcept {
			constexpr size_t CURVE_N = 1024u;
			constexpr size_t POINT_N = 64u;
			constexpr size_t SIZE_CURVE_POINT = 28u;

			struct CurveDataV1 {
				// 28バイト
				struct {
					uint8_t dummy[SIZE_CURVE_POINT];
				} curve_pt[POINT_N];
				uint32_t size;
			};

			auto curve_data = reinterpret_cast<const CurveDataV1*>(data);
			std::vector<NormalCurve> vec_tmp;
			bool flag_default = true;

			for (int i = CURVE_N - 1; 0 <= i; i--) {
				// sizeの値が不正でないことをチェック
				if (POINT_N < curve_data[i].size) {
					return false;
				}
				NormalCurve curve;
				if (!curve.load_v1_data(reinterpret_cast<const byte*>(&curve_data[i]), curve_data[i].size)) {
					return false;
				}
				// 末尾までデフォルトのカーブが連続している部分をスキップ
				if (flag_default and !curve.is_default()) {
					flag_default = false;
				}
				if (!flag_default) {
					vec_tmp.emplace_back(std::move(curve));
				}
			}
			vec_tmp.reserve(vec_tmp.capacity());
			curves_normal_ = std::move(vec_tmp);
			// インデックスをリセット
			idx_normal_ = 0;
			return true;
		}
	}
}