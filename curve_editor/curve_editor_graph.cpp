#include "curve_editor_graph.hpp"
#include "config.hpp"
#include "enum.hpp"



namespace cved {
	namespace global {
		GraphCurveEditor::GraphCurveEditor() :
			curves_normal_{},
			curves_value_{},
			curve_bezier_{ mkaul::Point{0., 0.}, mkaul::Point{1., 1.}, 0u, 0u, true},
			curve_elastic_{ mkaul::Point{0., 0.}, mkaul::Point{1., 0.5}, 0u, 0u, true },
			curve_bounce_{ mkaul::Point{0., 0.}, mkaul::Point{1., 1.}, 0u, 0u, true }
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

		GraphCurve* GraphCurveEditor::current_curve() noexcept {

			switch (global::config.get_edit_mode()) {
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

		// NormalCurveのvectorのデータを作成
		void GraphCurveEditor::create_data_normal(std::vector<byte>& data) const noexcept {
			data.clear();
			for (const auto& curve : curves_normal_) {
				std::vector<byte> tmp;
				curve.create_data(tmp);
				// データのサイズ
				uint32_t size = tmp.size();
				auto bytes_size = reinterpret_cast<byte*>(&size);
				// サイズの情報(4バイト)をねじ込む
				tmp.insert(tmp.begin(), bytes_size, bytes_size + sizeof(uint32_t) / sizeof(byte));
				// データの連結
				std::copy(tmp.begin(), tmp.end(), std::back_inserter(data));
			}
		}

		// NormalCurveのデータを読み込み
		bool GraphCurveEditor::load_data_normal(const byte* data, size_t size) noexcept {
			constexpr size_t SIZE_N = sizeof(uint32_t) / sizeof(byte);

			std::vector<NormalCurve> vec_tmp;
			for (size_t idx = 0u; idx < size;) {
				// idxからsizeまでのバイト数が4バイト未満(サイズ情報が取得できない)の場合
				if (size < idx + SIZE_N) return false;
				// サイズ情報の取得
				auto p_size = reinterpret_cast<const uint32_t*>(data + idx);
				// データが記述されている部分までインデックスを進める
				idx += SIZE_N;
				// idxからsizeまでのバイト数がサイズ情報のサイズより小さい(データが無効)場合
				if (size < idx + *p_size) return false;
				// NormalCurveのデータの読み込み
				NormalCurve curve;
				if (!curve.load_data(data + idx, *p_size)) return false;
				// 読み込みに成功したらインデックスを進め、カーブをアペンド
				idx += *p_size;
				vec_tmp.emplace_back(std::move(curve));
			}
			curves_normal_ = std::move(vec_tmp);
			// インデックスをリセット
			idx_normal_ = 0;
			return true;
		}

		// v1.xのカーブのデータを読み取り
		bool GraphCurveEditor::load_data_v1(const byte* data) noexcept {
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

			for (size_t i = 0u; i < CURVE_N; i++) {
				// sizeの値が不正でないことをチェック
				if (POINT_N < curve_data[i].size) return false;
				NormalCurve curve;
				if (!curve.load_data_v1(reinterpret_cast<const byte*>(&curve_data[i]), curve_data[i].size)) {
					return false;
				}
				// 読み込みに成功したらカーブをアペンド
				vec_tmp.emplace_back(std::move(curve));
			}
			curves_normal_ = std::move(vec_tmp);
			// インデックスをリセット
			idx_normal_ = 0;
			return true;
		}
	}
}