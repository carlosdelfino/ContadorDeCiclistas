/*
 * CycloTracker.hpp
 *
 *  Created on: 8 de out de 2016
 *      Author: consu_000
 */

#ifndef CYCLOTRACKER_HPP_
#define CYCLOTRACKER_HPP_

#include<opencv2/opencv.hpp>

#include<stdint.h>

#include "CycloConfig.hpp"
#include "VideoOutput.hpp"
#include "ObjectTracker.hpp"
#include "Sensors.hpp"

void PreCallBack(	//.
		cv::Mat frame,				//.
		cv::Mat full,				//.
		cv::Mat fore				//.
		);
void PosCallBack(	//.
		cv::Mat frame,				//.
		cv::Mat full,				//.
		cv::Mat fore				//.
		);

typedef void (*TrackerCallback)(	//.
		cv::Mat frame,				//.
		cv::Mat full,				//.
		cv::Mat fore				//.
		);

class CycloTracker {
private:
	CycloConfig *config = nullptr;
	cv::VideoCapture *cap = nullptr;
	VideoOutput *outputDevice = nullptr;
	cv::VideoWriter *outputFile = nullptr;
	SensorData *sd = nullptr;

	void ProvideOsd(cv::Mat &frame, SensorData *sd, ObjectTracker &ot);
	void ProvidePip(cv::Mat &frame, cv::Mat &dst);

	TrackerCallback posCallBack = nullptr;
	TrackerCallback preCallBack = nullptr;

public:

	CycloTracker(CycloConfig *config);
	CycloTracker(CycloConfig *config, cv::VideoCapture *cap,
			cv::VideoWriter *outputDevice, VideoOutput *outputFile,
			SensorData *sd);

	cv::VideoCapture* openVideoCapture();

	VideoOutput* openVideoOutput();

	cv::VideoWriter* openVideoWriter();

	void processFrames(unsigned int delay = INT_MAX);

	virtual ~CycloTracker();

	void setPreCallBack(TrackerCallback);
	void setPosCallBack(TrackerCallback);
};

#endif /* CYCLOTRACKER_HPP_ */
