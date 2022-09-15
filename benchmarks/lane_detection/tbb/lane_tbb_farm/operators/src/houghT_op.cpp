#include <../include/houghT_op.hpp>

inline void spb::HoughT::houghT_op(spb::item_data &item){

	//Hough tranform for line detection with feedback
	//Increase by 25 for the next frame if we found some lines.  
	//This is so we don't miss other lines that may crop up in the next frame
	//but at the same time we don't want to start the feed back loop from scratch. 

	int houghVote = 200;

	//we lost all lines. reset 
	if (houghVote < 1 || item.lines.size() > 2) 
	{ 
		houghVote = 200; 
	}
	else
	{ 
		houghVote += 25;
	} 

	while(item.lines.size() < 5 && houghVote > 0)
	{
		cv::HoughLines(item.contours, item.lines,1,PI/180, houghVote);
		houghVote -= 5;  
	}

	cv::Mat result(item.imgROI.size(), CV_8U, cv::Scalar(255));
	item.imgROI.copyTo(result);

	//draw the limes
	std::vector<cv::Vec2f>::const_iterator it;
	cv::Mat hough(item.imgROI.size(), CV_8U, cv::Scalar(0));
	it = item.lines.begin();

	while(it!=item.lines.end()) 
	{
		//first element is distance rho
		float rho= (*it)[0];
		//second element is angle theta	   
		float theta= (*it)[1]; 			
		if( (theta > 0.09 && theta < 1.48) || (theta < 3.14 && theta > 1.66) ) 
		{ 
			//filter to remove vertical and horizontal lines
			//point of intersection of the line with first row
			cv::Point pt1(rho/cos(theta),0);
			//point of intersection of the line with last row
			cv::Point pt2((rho-result.rows*sin(theta))/cos(theta), result.rows);
			//draw a white line
			cv::line(result, pt1, pt2, cv::Scalar(255), 8); 
			cv::line(hough, pt1, pt2, cv::Scalar(255), 8);
		}
		++it;
	}
	item.hough = hough;
}
