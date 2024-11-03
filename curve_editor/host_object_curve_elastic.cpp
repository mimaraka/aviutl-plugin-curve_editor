#include "curve_editor.hpp"
#include "curve_elastic.hpp"
#include "host_object_curve_elastic.hpp"



namespace cved {
	uint32_t ElasticCurveHostObject::get_id(bool is_select_dialog) {
		static ElasticCurve curve_tmp{
			mkaul::Point{0., 0.}, mkaul::Point{1., 0.5}
		};
		uint32_t id;
		if (is_select_dialog) {
			id = curve_tmp.get_id();
		}
		else {
			id = global::editor.editor_graph().curve_elastic()->get_id();
		}
		return id;
	}

	double ElasticCurveHostObject::get_handle_amp_left_x(uint32_t id) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_amp_left_x();
	}

	double ElasticCurveHostObject::get_handle_amp_left_y(uint32_t id) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_amp_left_y();
	}

	void ElasticCurveHostObject::set_handle_amp_left(uint32_t id, double y) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return;
		}
		curve->set_handle_amp_left(y);
	}

	double ElasticCurveHostObject::get_handle_amp_right_x(uint32_t id) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_amp_right_x();
	}

	double ElasticCurveHostObject::get_handle_amp_right_y(uint32_t id) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_amp_right_y();
	}

	void ElasticCurveHostObject::set_handle_amp_right(uint32_t id, double y) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return;
		}
		curve->set_handle_amp_right(y);
	}

	double ElasticCurveHostObject::get_handle_freq_decay_x(uint32_t id) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_freq_decay_x();
	}

	double ElasticCurveHostObject::get_handle_freq_decay_y(uint32_t id) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_freq_decay_y();
	}

	double ElasticCurveHostObject::get_handle_freq_decay_root_y(uint32_t id) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return 0.;
		}
		return curve->get_handle_freq_decay_root_y();
	}

	void ElasticCurveHostObject::set_handle_freq_decay(uint32_t id, double x, double y) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return;
		}
		curve->set_handle_freq_decay(x, y);
	}

	std::string ElasticCurveHostObject::get_param(uint32_t id) {
		auto curve = global::id_manager.get_curve<ElasticCurve>(id);
		if (!curve) {
			return "";
		}
		return curve->create_params_str();
	}
} // namespace cved