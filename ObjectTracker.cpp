#include "ObjectTracker.hpp"
#include "PointTracker.hpp"
#include <time.h>

ObjectTracker::ObjectTracker(						//.
		CycloConfig *config 						//.
		) ://.
		ObjectTracker(								//.
				config,								//.
				config->getDistanceThreshold(),		//.
				config->getContourThreshold(),		//.
				config->getInterestArea()			//.
				) {
	// do nothing;
}
ObjectTracker::ObjectTracker(						//.
		CycloConfig *config, 						//.
		cv::Rect interest_area						//.
		) ://.
		ObjectTracker(								//.
				config,								//.
				config->getDistanceThreshold(),		//.
				config->getContourThreshold(),		//.
				interest_area						//.
				) {
	// do nothing;
}
ObjectTracker::ObjectTracker(						//.
		CycloConfig *config,						//.
		unsigned int distanceThreshold, 			//.
		double contour_threshold,					//.
		cv::Rect interest_area						//.
		) {

	std::cout << "** ObjectTracker: Criando " << std::endl;

	this->config = config;

	this->image_counter = config->getCounterDetectedObject();

	this->filePath = config->getFilePahtDetectedObject();
	this->filePrefix = config->getFilePrefixDetectedObject();
	this->fileExtension = config->getFileExtensionDetectedObject();

	this->distance_threshold = distanceThreshold;

	this->object_locator = new ObjectLocator(contour_threshold);
	this->object_counter = new ObjectCounter(interest_area);

	std::cout << "** ObjectTracker:  criado" << std::endl;
}

void ObjectTracker::IterateTracker(cv::Mat &frame, cv::Mat &fore) {
	this->CreateNewTracker();
	this->AddRectanglesToTracker(frame, fore);
	this->AccountNewObjects(frame);
	this->RenewTrackers();
}

void ObjectTracker::SetInterestArea(cv::Rect interest_area) {
	this->object_counter->SetReferenceBox(interest_area);
}

void ObjectTracker::CreateNewTracker() {
	if (this->old_tracker) {
		this->new_tracker = new RectangleTracker(this->distance_threshold,
				*(this->old_tracker));
	} else {
		this->new_tracker = new RectangleTracker(this->distance_threshold);
	}
}

void ObjectTracker::AddRectanglesToTracker(cv::Mat &frame, cv::Mat &fore) {
	auto rects_from_countour = object_locator->DetectObjects(fore);
	for (auto rec : rects_from_countour) {
		this->new_tracker->AddRectangle(rec);
		cv::rectangle(frame, rec, cv::Scalar(0, 0, 255), 1);
	}
}

void ObjectTracker::AddDate(cv::Mat &frame) {
	time_t time_now = time(nullptr);
	char buff[50];
	int yPos = 15;

	strftime(buff, sizeof(buff), "%d/%m/%Y", localtime(&time_now));
	cv::putText(frame, std::string(buff), cv::Point(2, yPos),
	CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255), 2, CV_AA);
	cv::putText(frame, std::string(buff), cv::Point(2, yPos),
	CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0), 1, CV_AA);

	strftime(buff, sizeof(buff), "%X", localtime(&time_now));
	cv::Size sz = cv::getTextSize(std::string(buff), CV_FONT_HERSHEY_PLAIN, 1,
			2, nullptr);
	cv::putText(frame, std::string(buff),
			cv::Point(frame.size().width - sz.width - 2, yPos),
			CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255), 2,
			CV_AA);
	cv::putText(frame, std::string(buff),
			cv::Point(frame.size().width - sz.width - 2, yPos),
			CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0), 1,
			CV_AA);
}

void ObjectTracker::PersistImage(cv::Mat &frame) {
#define DATE_SIZE 50
	char date[DATE_SIZE];
	time_t time_now = time(nullptr);
	strftime(date, sizeof(date), "%Y%m%d", localtime(&time_now));

	const unsigned int size = filePath.size() + filePrefix.size() + DATE_SIZE
			+ 5 + fileExtension.size() + 2;
	char fileName[size];
	memset(fileName, '\n', size);
	sprintf(fileName, "%s/%s%s-%05u.%s", filePath.c_str(), filePrefix.c_str(),
			date, this->image_counter++, fileExtension.c_str());
	cv::imwrite(fileName, frame);
	delete[] fileName;
}

void ObjectTracker::AccountNewObjects(cv::Mat &frame) {
	if (this->old_tracker && this->new_tracker) {
		for (auto trackedRect : this->new_tracker->points) {
			auto rectCenter = trackedRect.second;
			if (object_counter->AccountPoint(rectCenter)) {
				this->PersistImage(frame);
			}
			char id[11];
			sprintf(id, "%02d", rectCenter.id);
			cv::putText(frame, std::string(id), rectCenter.pt,
			CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 255, 0));
		}
		delete old_tracker;
		old_tracker = nullptr;
	}
}

void ObjectTracker::RenewTrackers() {
	if (new_tracker) {
		old_tracker = new_tracker;
	}
}

void ObjectTracker::PrintCounters(cv::Mat &frame) {
	char id[20];
	sprintf(id, "Total: %02d", object_counter->GetTotalPoints());
	cv::putText(frame, std::string(id), cv::Point(2, 30),
	CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255), 2, CV_AA);
	cv::putText(frame, std::string(id), cv::Point(2, 30),
	CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0), 1, CV_AA);

	sprintf(id, ">%02d", object_counter->GetLTRPoints());
	cv::putText(frame, std::string(id), cv::Point(2, 60),
	CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255), 2, CV_AA);
	cv::putText(frame, std::string(id), cv::Point(2, 60),
	CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0), 1, CV_AA);

	sprintf(id, "<%02d", object_counter->GetRTLPoints());
	cv::putText(frame, std::string(id), cv::Point(2, 45),
	CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255), 2, CV_AA);
	cv::putText(frame, std::string(id), cv::Point(2, 45),
	CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 0), 1, CV_AA);
}

void ObjectTracker::PrintTotal(cv::Mat &frame) {
	char id[20];
	cv::Point pt(10, 50);
	sprintf(id, "Total: %02d", object_counter->GetTotalPoints());
	cv::putText(frame, std::string(id), pt, CV_FONT_HERSHEY_PLAIN, 3,
			cv::Scalar(0, 0, 0), 5, CV_AA);
	cv::putText(frame, std::string(id), pt, CV_FONT_HERSHEY_PLAIN, 3,
			cv::Scalar(0, 255, 255), 3, CV_AA);
}

inline void ObjectTracker::PrintRightPartial(cv::Mat &frame, cv::Point pt) {
	char id[10];
	sprintf(id, ">%02d", object_counter->GetLTRPoints());
	cv::putText(frame, std::string(id), pt, CV_FONT_HERSHEY_PLAIN, 3,
			cv::Scalar(0, 0, 0), 5, CV_AA);
	cv::putText(frame, std::string(id), pt, CV_FONT_HERSHEY_PLAIN, 3,
			cv::Scalar(0, 255, 255), 3, CV_AA);
}

 inline void ObjectTracker::PrintLeftPartial(cv::Mat &frame, cv::Point pt) {
	char id[10];
	sprintf(id, "<%02d", object_counter->GetRTLPoints());
	cv::putText(frame, std::string(id), pt, CV_FONT_HERSHEY_PLAIN, 3,
			cv::Scalar(0, 0, 0), 5, CV_AA);
	cv::putText(frame, std::string(id), pt, CV_FONT_HERSHEY_PLAIN, 3,
			cv::Scalar(0, 255, 255), 3, CV_AA);
}

  void ObjectTracker::PrintLeftPartial(cv::Mat &frame) {
	PrintLeftPartial(frame, config->GetCounterPos(0));
}

 void ObjectTracker::PrintRightPartial(cv::Mat &frame) {
	PrintRightPartial(frame, config->GetCounterPos(1));
}

unsigned int ObjectTracker::GetTotal() {
	return object_counter->GetTotalPoints();
}

unsigned int ObjectTracker::GetRightPartial() {
	return object_counter->GetLTRPoints();
}

unsigned int ObjectTracker::GetLeftPartial() {
	return object_counter->GetRTLPoints();
}

void ObjectTracker::ZeroCounters() {
	object_counter->ZeroCounters();
}
