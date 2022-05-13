#include <../include/recognize_op.hpp>

inline void spb::Recognize::recognize_op(spb::item_data &item){
	//analyze each detected face:
		bool has_match = false;
		double match_conf = 0;
		int index = 0;
		for (std::vector<cv::Rect>::const_iterator face = item.faces.begin() ; face != item.faces.end() ; face++, index++){
			cv::Scalar color = cv::NO_MATCH_COLOR;

			double confidence = 0;
			bool face_match = false;

			//try to recognize the face:
			cv::Ptr<cv::FaceRecognizer> _model = model;

			cv::Mat gray, aux;
			if(SPBench::memory_source_is_enabled()){
				cv::Mat tmp = *(item.image_p);
				cv::Mat face_img = tmp(*face);
				aux = tmp(item.faces[index]);
			} else {
				cv::Mat face_img = item.image(*face);
				aux = item.image(item.faces[index]);
			}	
			
			int label;
			cvtColor(aux, gray, CV_BGR2GRAY);
			resize(gray, gray, _faceSize);
			_model->predict(gray, label, confidence);

			bool verify;
			label == 10 ? verify = true : verify = false;

			if (verify){
				color = cv::MATCH_COLOR;
				has_match = true;
				face_match = true;
				match_conf = confidence;
			}

			cv::Point center(face->x + face->width * 0.5, face->y + face->height * 0.5);
			if(SPBench::memory_source_is_enabled()){
				circle(*(item.image_p), center, FACE_RADIUS_RATIO * face->width, color, CIRCLE_THICKNESS, LINE_TYPE, 0);
			} else {
				circle(item.image, center, FACE_RADIUS_RATIO * face->width, color, CIRCLE_THICKNESS, LINE_TYPE, 0);
			}
		}  
		if(SPBench::memory_source_is_enabled()){
			putText(*(item.image_p), cv::format("Frame: %d", (item.index +1)), cvPoint(10, item.image_p->rows - 105),
					cv::FONT, 2, cv::FONT_COLOR, 1, LINE_TYPE);
			putText(*(item.image_p), cv::format("FPS: %d", 15), cvPoint(10, item.image_p->rows - 80),
					cv::FONT, 2, cv::FONT_COLOR, 1, LINE_TYPE);
			putText(*(item.image_p), cv::format("Faces: %d", item.faces.size()), cvPoint(10, item.image_p->rows - 55),
					cv::FONT, 2, cv::FONT_COLOR, 1, LINE_TYPE);
			putText(*(item.image_p), cv::format("Match: %s", has_match ? "True" : "False"), cvPoint(10, item.image_p->rows - 30),
					cv::FONT, 2, cv::FONT_COLOR, 1, LINE_TYPE);
			putText(*(item.image_p), cv::format("Confidence: %f", has_match ? match_conf : 0), cvPoint(10, item.image_p->rows - 5),
					cv::FONT, 2, cv::FONT_COLOR, 1, LINE_TYPE);
		} else {
			putText(item.image, cv::format("Frame: %d", (item.index +1)), cvPoint(10, item.image.rows - 105),
					cv::FONT, 2, cv::FONT_COLOR, 1, LINE_TYPE);
			putText(item.image, cv::format("FPS: %d", 15), cvPoint(10, item.image.rows - 80),
					cv::FONT, 2, cv::FONT_COLOR, 1, LINE_TYPE);
			putText(item.image, cv::format("Faces: %d", item.faces.size()), cvPoint(10, item.image.rows - 55),
					cv::FONT, 2, cv::FONT_COLOR, 1, LINE_TYPE);
			putText(item.image, cv::format("Match: %s", has_match ? "True" : "False"), cvPoint(10, item.image.rows - 30),
					cv::FONT, 2, cv::FONT_COLOR, 1, LINE_TYPE);
			putText(item.image, cv::format("Confidence: %f", has_match ? match_conf : 0), cvPoint(10, item.image.rows - 5),
					cv::FONT, 2, cv::FONT_COLOR, 1, LINE_TYPE);
		}
}