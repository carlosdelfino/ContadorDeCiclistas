/*
 * CycloTracker.hpp
 *
 *  Created on: 8 de out de 2016
 *      Author: consu_000
 */

#ifndef CYCLOTRACKER_HPP_
#define CYCLOTRACKER_HPP_

#include<opencv2/opencv.hpp>

#include "CycloConfig.hpp"
#include "VideoOutput.hpp"
#include "Sensors.hpp"

class CycloTracker {
private:
	CycloConfig *config = nullptr;
	cv::VideoCapture *cap = nullptr;
	cv::VideoWriter *outputDevice = nullptr;
	VideoOutput *outputFile = nullptr;
	SensorData *sd = nullptr;
public:
	CycloTracker(CycloConfig *config, cv::VideoCapture *cap,
			cv::VideoWriter *outputDevice, VideoOutput *outputFile,
			SensorData *sd);

	void processFrames();

	virtual ~CycloTracker();

};

#endif /* CYCLOTRACKER_HPP_ */
