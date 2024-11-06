#pragma once

#include <mkaul/point.hpp>



namespace cereal {
	template <class Archive>
	void save(Archive& archive, const mkaul::Point<double>& pt) {
		archive(pt.x, pt.y);
	}

	template <class Archive>
	void load(Archive& archive, mkaul::Point<double>& pt) {
		archive(pt.x, pt.y);
	}
} // namespace cereal