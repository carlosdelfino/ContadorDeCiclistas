#ifndef OBJECT_TRACKER_HPP
#define OBJECT_TRACKER_HPP

#include <opencv2/opencv.hpp>

#include "CycloConfig.hpp"
#include "TrackedObject.hpp"
#include "ObjectCounter.hpp"
#include "ObjectLocator.hpp"
#include "PointTracker.hpp"

class ObjectTracker {
private:
	CycloConfig *config;
	RectangleTracker *old_tracker = nullptr;
	RectangleTracker *new_tracker = nullptr;
	ObjectLocator *object_locator = nullptr;
	ObjectCounter *object_counter = nullptr;

	unsigned int distance_threshold;

	unsigned int image_counter = 0;

	std::string filePath = "";
	std::string filePrefix = "";
	std::string fileExtension = "";

public:
	ObjectTracker(							//.
			CycloConfig *config				//.
			);
	ObjectTracker(							//.
			CycloConfig *config,			//.
			cv::Rect interest_area			//.
			);
	ObjectTracker(							//.
			CycloConfig *config, 			//.
			unsigned int distanceThreshold, //.
			double contour_threshold, 		//.
			cv::Rect interest_area			//.
			);

	void IterateTracker(cv::Mat &frame, cv::Mat &fore);
	void SetInterestArea(cv::Rect interest_area);
	void PrintTotal(cv::Mat &frame);
	void PrintLeftPartial(cv::Mat &frame, cv::Point pt);
	void PrintRightPartial(cv::Mat &frame, cv::Point pt);
	void PrintLeftPartial(cv::Mat &frame);
	void PrintRightPartial(cv::Mat &frame);
	unsigned int GetTotal();
	unsigned int GetRightPartial();
	unsigned int GetLeftPartial();
	void ZeroCounters();

private:
	void CreateNewTracker();
	void AddRectanglesToTracker(cv::Mat &frame, cv::Mat &fore);
	void AddDate(cv::Mat &frame);
	void PersistImage(cv::Mat &frame);
	void AccountNewObjects(cv::Mat &frame);
	void RenewTrackers();
	void PrintCounters(cv::Mat &frame);
};

#endif
