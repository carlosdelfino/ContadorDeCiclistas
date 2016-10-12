#ifndef IMAGE_PROCESSOR_HPP
#define IMAGE_PROCESSOR_HPP

#include<opencv2/opencv.hpp>
#include<iostream>

#include "CycloConfig.hpp"

class ImageProcessor {
private:
	CycloConfig *config;
	cv::BackgroundSubtractorMOG2 bg;
	cv::Mat lambda; //used to get changes in perspective
	cv::Mat frame;
	cv::Rect cropArea;
	cv::Point2f quad[4];
	cv::Rect interestArea;
public:
	ImageProcessor();
	ImageProcessor(CycloConfig* config);
	~ImageProcessor();
	cv::Mat AcquireForeground(cv::Mat &frame);
	void PrintText(cv::Mat &frame, std::string text, cv::Point position);
	void InsertInterestArea(cv::Mat &frame);
	void InsertInterestArea(cv::Mat &frame, cv::Rect interestArea);
	void PrepareFrame(cv::Mat &frame);
	void PrepareFrame(cv::Mat &frame, cv::Rect cropArea, cv::Point2f p0,
			cv::Point2f p1, cv::Point2f p2, cv::Point2f p3);

	void RotateImage(cv::Mat &frame);

	void setCropArea(cv::Rect cropArea);
	void setInterestArea(cv::Rect interestArea);
	void setPerspective(cv::Point2f p0, cv::Point2f p1, cv::Point2f p2,
			cv::Point2f p3);
protected:
	void PerspectiveTransform(cv::Mat &frame, cv::Point2f p0, cv::Point2f p1,
			cv::Point2f p2, cv::Point2f p3);

	void CropImage(cv::Mat &frame, cv::Rect cropArea);
};
#endif
