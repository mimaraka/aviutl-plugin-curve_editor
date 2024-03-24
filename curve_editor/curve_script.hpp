#pragma once

#include "curve.hpp"



namespace cved {
	// カーブ(スクリプト)
	class ScriptCurve : public Curve {
	public:
		double get_value(double progress, double start, double end) const noexcept override { return start; }
		void clear() noexcept override {}

		void create_data(std::vector<byte>& data) const noexcept override;
		bool load_data(const byte* data, size_t size) noexcept override;
	};
}