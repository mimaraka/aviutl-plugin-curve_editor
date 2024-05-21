#include "curve_editor.hpp"
#include "config.hpp"
#include "constants.hpp"



namespace cved {
	namespace global {
		Curve* CurveEditor::current_curve() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
			case EditMode::Value:
			case EditMode::Bezier:
			case EditMode::Elastic:
			case EditMode::Bounce:
				return editor_graph_.current_curve();

			case EditMode::Script:
			default:
				return editor_script_.curve_script();
			}
		}

		size_t CurveEditor::current_idx() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				return editor_graph_.idx_normal();

			case EditMode::Value:
				return editor_graph_.idx_value();

			case EditMode::Script:
				return editor_script_.idx();

			default:
				return 0u;
			}
		}

		bool CurveEditor::set_idx(size_t idx) noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				return editor_graph_.set_idx_normal(idx);

			case EditMode::Value:
				return editor_graph_.set_idx_value(idx);
				break;

			case EditMode::Script:
				return editor_script_.set_idx(idx);

			default:
				return false;
			}
		}

		bool CurveEditor::advance_idx(int n) noexcept {
			return set_idx((size_t)std::max((int)current_idx() + n, 0));
		}

		void CurveEditor::jump_to_last_idx() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				editor_graph_.jump_to_last_idx_normal();
				break;

			case EditMode::Value:
				editor_graph_.jump_to_last_idx_value();
				break;

			case EditMode::Script:
				editor_script_.jump_to_last_idx();
				break;

			default:
				break;
			}
		}

		bool CurveEditor::is_idx_last() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				return editor_graph_.is_idx_normal_last();

			case EditMode::Value:
				return editor_graph_.is_idx_value_last();

			case EditMode::Script:
				return editor_script_.is_idx_last();

			default:
				return false;
			}
		}

		void CurveEditor::delete_last_idx() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				editor_graph_.delete_last_idx_normal();
				break;

			case EditMode::Value:
				editor_graph_.delete_last_idx_value();
				break;

			case EditMode::Script:
				editor_script_.delete_last_idx();
				break;

			default:
				break;
			}
		}

		void CurveEditor::reset_id_curves() noexcept {
			editor_graph_.reset_id_curves();
			editor_script_.reset();
		}

		int32_t CurveEditor::track_param() noexcept {
			switch (global::config.get_edit_mode()) {
			case EditMode::Normal:
				return (int32_t)editor_graph_.idx_normal() + 1;

			case EditMode::Value:
				return -(int32_t)editor_graph_.idx_value() - 1;

			case EditMode::Script:
				return (int32_t)(IDCURVE_MAX_N + editor_script_.idx()) + 1;

			case EditMode::Bezier:
			case EditMode::Elastic:
			case EditMode::Bounce:
				return editor_graph_.numeric_curve()->encode();

			default:
				return 0;
			}
		}

		void CurveEditor::create_data(std::vector<byte>& data) const noexcept {
			constexpr size_t SIZE_N = sizeof(uint32_t) / sizeof(byte);
			
			// NormalCurveのデータを作成
			std::vector<byte> data_normal;
			editor_graph_.create_data_normal(data_normal);
			uint32_t size_normal = data_normal.size();
			auto bytes_size_normal = reinterpret_cast<byte*>(&size_normal);
			// サイズの情報(4バイト)をねじ込む
			data_normal.insert(data_normal.begin(), bytes_size_normal, bytes_size_normal + SIZE_N);
			// カーブの種類の識別子(1バイト)をねじ込む
			data_normal.insert(data_normal.begin(), (byte)IDCurveType::Normal);

			// TODO: ValueCurve、ScriptCurveのデータを作成

			// すべてのデータを連結
			data.clear();
			std::copy(data_normal.begin(), data_normal.end(), std::back_inserter(data));

			// バージョン情報("CEV2"の文字列, 4バイト)をねじ込む
			auto bytes_prefix = reinterpret_cast<const byte*>(global::DATA_PREFIX);
			data.insert(data.begin(), bytes_prefix, bytes_prefix + strlen(global::DATA_PREFIX));
		}

		// カーブのデータを読み取り
		bool CurveEditor::load_data(const byte* data, size_t size) noexcept {
			constexpr size_t SIZE_N = sizeof(uint32_t) / sizeof(byte);

			for (size_t idx = 0u; idx < size;) {
				// IDカーブのタイプの識別子を確認
				switch (*(data + idx)) {
				case (byte)IDCurveType::Normal:
				{
					// idxからsizeまでのバイト数が5バイト未満(タイプ・サイズ情報が取得できない)の場合
					if (size < idx + SIZE_N + 1u) return false;
					idx++;
					auto p_size = reinterpret_cast<const uint32_t*>(data + idx);
					// データが記述されている部分までインデックスを進める
					idx += SIZE_N;
					// idxからsizeまでのバイト数がサイズ情報のサイズより小さい(データが無効)場合
					if (size < idx + *p_size) return false;
					if (!editor_graph_.load_data_normal(data + idx, *p_size)) return false;
					// 読み込みに成功したらインデックスを進める
					idx += *p_size;
					break;
				}

				// TODO: Value、Scriptの読み込み処理
				case (byte)IDCurveType::Value:
				case (byte)IDCurveType::Script:
				default:
					return false;
				}
			}
			return true;
		}
	}
}