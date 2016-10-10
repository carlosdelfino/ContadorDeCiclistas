#include<opencv2/opencv.hpp>

#include "CycloTracker.hpp"
#include "CycloConfig.hpp"
#include "ImageProcessor.hpp"
#include "InteractionHandler.hpp"
#include "ObjectTracker.hpp"
#include "VideoOutput.hpp"
#include "Sensors.hpp"
#include "TrackedObject.hpp"
#include "Utils.hpp"

CycloTracker::CycloTracker(CycloConfig *config, cv::VideoCapture *cap,
		cv::VideoWriter *outputDevice, VideoOutput *outputFile,
		SensorData *sd) {
	this->config = config;
	this->cap = cap;
	this->outputDevice = outputDevice;
	this->outputFile = outputFile;
	this->sd = sd;
}

CycloTracker::~CycloTracker(){}

void CycloTracker::processFrames() {

		std::cout << "** Criando rastreador de objetos " << std::endl;
	ObjectTracker ot(30, 50, (*config).getInterestArea());

	cv::Mat frame;

	ImageProcessor ip;

	cv::Mat fore;
	char key;
	do {

		cv::Point2f p0((*config).getX(0), (*config).getY(0));
		cv::Point2f p1((*config).getX(1), (*config).getY(1));
		cv::Point2f p2((*config).getX(2), (*config).getY(2));
		cv::Point2f p3((*config).getX(3), (*config).getY(3));

		// @TODO verificar se pode evitar a redefinição da area de
		// interesse aqui em caso de não ser reconfiguração
		cv::Rect interestArea = (*config).getInterestArea();
		ot.SetInterestArea(interestArea);

		*cap >> frame;		// captura um novo frame e processa imediatamente

		cv::Mat full = frame.clone();

		ip.PrepareFrame(frame, (*config).getCropArea(), p0, p1, p2, p3);
		fore = ip.AcquireForeground(frame);
		ip.InsertInterestArea(frame, interestArea);
		ot.IterateTracker(frame, fore);

		ot.PrintTotal(full);

		ot.PrintLeftPartial(full, (*config).GetCounterPos(0));
		ot.PrintRightPartial(full, (*config).GetCounterPos(1));

		ProvideOsd(full, sd, ot);
		ProvidePip(frame, full);

		cv::imshow((*config).getAddress(), frame);
		cv::imshow("Full Frame", full);

		if (outputFile)
			outputFile->write(frame);		//Write avi file
		if (outputDevice)
			outputDevice->write(full);

		if (IsMidnight()) {
			ot.ZeroCounters();
		}

		key = cv::waitKey(30);
		if (key == 'R' || key == 'r') {
			// buscar InterationHandler da janela em questão e zerar ação para reinicia-la.
		} else if (key == 'Z' || 'z') {
			(*config).SetLeftCounter(0);
			(*config).SetRightCounter(0);
		}

		(*config).countInteraction();
	} while (char(key) != char(27));
}
