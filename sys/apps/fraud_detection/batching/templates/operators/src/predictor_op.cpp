#include <../include/predictor_op.hpp>

inline void spb::Predictor::predictor_op(spb::item_data &item){
	Prediction prediction_object = predictor.execute(item.key, item.record, ",");
	if (prediction_object.is_outlier()) {
		item.score = prediction_object.get_score();
		item.isOutlier = true;
	}
}