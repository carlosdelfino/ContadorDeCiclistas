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

class CycloTracker {
private:
	CycloConfig *config = nullptr;
	cv::VideoCapture *cap = nullptr;
	cv::VideoWriter *outputDevice = nullptr;
	VideoOutput *outputFile = nullptr;
	SensorData *sd = nullptr;

	void ProvideOsd(cv::Mat &frame, SensorData *sd, ObjectTracker &ot);
	void ProvidePip(cv::Mat &frame, cv::Mat &dst);
public:
	CycloTracker(CycloConfig *config, cv::VideoCapture *cap,
			cv::VideoWriter *outputDevice, VideoOutput *outputFile,
			SensorData *sd);

	void processFrames(unsigned int delay = INT_MAX);

	virtual ~CycloTracker();

};

#endif /* CYCLOTRACKER_HPP_ */
