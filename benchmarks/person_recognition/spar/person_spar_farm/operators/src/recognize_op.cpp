#include <../include/recognize_op.hpp>

using namespace spb;
using namespace std;
using namespace cv;

inline void Recognize::recognize_op(item_data &item){

	SPBench spbench;

	//analyze each detected face:
	bool has_match = false;
	double match_conf = 0;
	int index = 0;
	for (vector<Rect>::const_iterator face = item.faces.begin() ; face != item.faces.end() ; face++, index++){
		Scalar color = NO_MATCH_COLOR;

		double confidence = 0;
		bool face_match = false;

		//try to recognize the face:
		Ptr<FaceRecognizer> _model = model;

		Mat gray, aux;
		if(spbench.memory_source_is_enabled()){
			Mat tmp = *(item.image_p);
			Mat face_img = tmp(*face);
			aux = tmp(item.faces[index]);
		} else {
			Mat face_img = item.image(*face);
			aux = item.image(item.faces[index]);
		}	
		
		int label;
		cvtColor(aux, gray, CV_BGR2GRAY);
		resize(gray, gray, _faceSize);
		_model->predict(gray, label, confidence);

		bool verify;
		label == 10 ? verify = true : verify = false;

		if (verify){
			color = MATCH_COLOR;
			has_match = true;
			face_match = true;
			match_conf = confidence;
		}

		Point center(face->x + face->width * 0.5, face->y + face->height * 0.5);
		if(spbench.memory_source_is_enabled()){
			circle(*(item.image_p), center, FACE_RADIUS_RATIO * face->width, color, CIRCLE_THICKNESS, LINE_TYPE, 0);
		} else {
			circle(item.image, center, FACE_RADIUS_RATIO * face->width, color, CIRCLE_THICKNESS, LINE_TYPE, 0);
		}
	}  
	if(spbench.memory_source_is_enabled()){
		putText(*(item.image_p), format("Frame: %d", (item.index +1)), cvPoint(10, item.image_p->rows - 105),
				FONT, 2, FONT_COLOR, 1, LINE_TYPE);
		putText(*(item.image_p), format("FPS: %d", 15), cvPoint(10, item.image_p->rows - 80),
				FONT, 2, FONT_COLOR, 1, LINE_TYPE);
		putText(*(item.image_p), format("Faces: %d", item.faces.size()), cvPoint(10, item.image_p->rows - 55),
				FONT, 2, FONT_COLOR, 1, LINE_TYPE);
		putText(*(item.image_p), format("Match: %s", has_match ? "True" : "False"), cvPoint(10, item.image_p->rows - 30),
				FONT, 2, FONT_COLOR, 1, LINE_TYPE);
		putText(*(item.image_p), format("Confidence: %f", has_match ? match_conf : 0), cvPoint(10, item.image_p->rows - 5),
				FONT, 2, FONT_COLOR, 1, LINE_TYPE);
	} else {
		putText(item.image, format("Frame: %d", (item.index +1)), cvPoint(10, item.image.rows - 105),
				FONT, 2, FONT_COLOR, 1, LINE_TYPE);
		putText(item.image, format("FPS: %d", 15), cvPoint(10, item.image.rows - 80),
				FONT, 2, FONT_COLOR, 1, LINE_TYPE);
		putText(item.image, format("Faces: %d", item.faces.size()), cvPoint(10, item.image.rows - 55),
				FONT, 2, FONT_COLOR, 1, LINE_TYPE);
		putText(item.image, format("Match: %s", has_match ? "True" : "False"), cvPoint(10, item.image.rows - 30),
				FONT, 2, FONT_COLOR, 1, LINE_TYPE);
		putText(item.image, format("Confidence: %f", has_match ? match_conf : 0), cvPoint(10, item.image.rows - 5),
				FONT, 2, FONT_COLOR, 1, LINE_TYPE);
	}
}