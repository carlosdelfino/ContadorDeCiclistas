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

CycloTracker::CycloTracker(CycloConfig *config) {
	std::cout << "** CycloTracker: Criando" << std::endl;

	this->config = config;
	this->cap = nullptr;
	this->outputDevice = nullptr;
	this->outputFile = nullptr;
	this->sd = nullptr;

	std::cout << "** CycloTracker: Criado" << std::endl;
}

CycloTracker::CycloTracker(				//.
		CycloConfig *config,			//.
		cv::VideoCapture *cap,			//.
		cv::VideoWriter *outputFile,	//.
		VideoOutput *outputDevice,		//.
		SensorData *sd					//.
		) {
	std::cout << "** CycloTracker: Criando" << std::endl;

	this->config = config;
	this->cap = cap;
	this->outputDevice = outputDevice;
	this->outputFile = outputFile;
	this->sd = sd;

	std::cout << "** CycloTracker: Criado" << std::endl;
}

CycloTracker::~CycloTracker() {
}

void CycloTracker::processFrames(unsigned int delay) {

	std::cout << "** CycloTracker: Criando rastreador de objetos " << std::endl;

	cv::Mat frame;

	cv::Point2f p0(config->getX(0), config->getY(0));
	cv::Point2f p1(config->getX(1), config->getY(1));
	cv::Point2f p2(config->getX(2), config->getY(2));
	cv::Point2f p3(config->getX(3), config->getY(3));

	cv::Rect interestArea = config->getInterestArea();
	cv::Rect cropArea = config->getCropArea();
	cv::Mat full;
	cv::Mat fore;
	char key;

	ImageProcessor ip(config);
	ObjectTracker ot(config);

	if (delay == INT_MAX)
		delay = config->getCaptureFrameDelay();
	do {

		*cap >> frame;		// captura um novo frame e processa imediatamente

		full = frame.clone();

		ip.PrepareFrame(frame);

		fore = ip.AcquireForeground(frame);

		if (preCallBack)
			preCallBack(frame, full, fore);

		ip.InsertInterestArea(frame);

		ot.IterateTracker(frame, fore);

		ot.PrintTotal(full);
		ot.PrintLeftPartial(full);
		ot.PrintRightPartial(full);

		ProvideOsd(full, sd, ot);
		ProvidePip(frame, full);

		cv::imshow(config->getAddress(), frame);
		cv::imshow("Full Frame", full);

		if (outputFile)
			outputFile->write(frame);
		if (outputDevice)
			outputDevice->write(full);

		if (IsMidnight()) {
			ot.ZeroCounters();
		}

		if (posCallBack)
			posCallBack(frame, full, fore);

		key = cv::waitKey(30);
		if (key == 'R' || key == 'r') {
			// buscar InterationHandler da janela em questão e zerar ação para reinicia-la.
		} else if (key == 'Z' || 'z') {
			ot.ZeroCounters();
		}

		if (config->reconfigure()) {
			p0.x = config->getX(0);
			p0.y = config->getY(0);
			p1.x = config->getX(1);
			p1.y = config->getY(1);
			p2.x = config->getX(2);
			p2.y = config->getY(2);
			p3.x = config->getX(3);
			p3.y = config->getY(3);

			interestArea = config->getInterestArea();
			cropArea = config->getCropArea();

			ip.setCropArea(cropArea);
			ip.setPerspective(p0, p1, p2, p3);
			ip.setInterestArea(interestArea);

			ot.SetInterestArea(interestArea);
		}

		config->countInteraction();
	} while (char(key) != char(27));
}

void CycloTracker::ProvideOsd(cv::Mat &frame, SensorData *sd,
		ObjectTracker &ot) {
	static cv::Mat logo = cv::imread(config->getPathPrincipalLogo(),
			CV_LOAD_IMAGE_COLOR);
	static cv::Mat cyclist = cv::imread(config->getPathSecondLogo(),
			CV_LOAD_IMAGE_COLOR);

	cv::Size sz = frame.size();
	cv::Point basePoint(0, sz.height - 80);
	int textVerticalSep = 20;
	int textTop = basePoint.y + 30;
	char mesgBuf[20];

	//Draw OSD
	cv::rectangle(frame, basePoint, cv::Point(sz.width, sz.height),
			cv::Scalar(255, 255, 255), CV_FILLED, 8, 0);

	//Paste logo
	cv::Rect logoRect(
			cv::Point(10, (sz.height + basePoint.y - logo.size().height) / 2),
			logo.size());
	logo.copyTo(frame(logoRect));

	//Paste cyclist
	cv::Rect cyclistRect(cv::Point(frame.size().width - 225, textTop - 25),
			cyclist.size());
	cyclist.copyTo(frame(cyclistRect));

	//Put data
	if (sd) {
		//sprintf(mesgBuf, "CO: %s", sd.co.c_str());
		//Print(mesgBuf, cv::Point(frame.size().width-170, textTop), frame);

		sprintf(mesgBuf, "Pressao: %dPa", sd->pressure);
		Print(mesgBuf,
				cv::Point(frame.size().width - 160,
						textTop + textVerticalSep * 2), frame);

		sprintf(mesgBuf, "Umidade: %d%%", sd->umidity);
		Print(mesgBuf,
				cv::Point(frame.size().width - 160, textTop + textVerticalSep),
				frame);

		sprintf(mesgBuf, "%d C", sd->temperature);
		Print(mesgBuf, cv::Point(frame.size().width - 125, textTop), frame, 2);

		sprintf(mesgBuf, "%d", sd->temperature);
		sz = cv::getTextSize(mesgBuf, CV_FONT_HERSHEY_PLAIN, 2, 2, nullptr);
		sprintf(mesgBuf, "o");
		Print(mesgBuf,
				cv::Point(frame.size().width - 127 + sz.width,
						textTop - (sz.height) + 2), frame, 1);
	}

	sprintf(mesgBuf, "%d", ot.GetTotal());
	Print(mesgBuf, cv::Point(frame.size().width - 300, textTop + 7), frame, 2);

	sprintf(mesgBuf, "Hoje");
	Print(mesgBuf,
			cv::Point(frame.size().width - 275, textTop + textVerticalSep * 2),
			frame, 2);

	time_t time_now = time(nullptr);
	char buff[50];

	sprintf(buff, config->getAddress().c_str());
	Print(buff, cv::Point(frame.size().width - 410, textTop), frame, 1);

	strftime(buff, sizeof(buff), "%d/%m/%y", localtime(&time_now));
	Print(buff, cv::Point(frame.size().width - 410, textTop + textVerticalSep),
			frame);

	strftime(buff, sizeof(buff), "%X", localtime(&time_now));
	Print(buff,
			cv::Point(frame.size().width - 410, textTop + textVerticalSep * 2),
			frame);
}

void CycloTracker::ProvidePip(cv::Mat &frame, cv::Mat &dst) {
	const int pip_x_limit = config->getCropWindowPosX();
	const int pip_y_limit = config->getCropWindowPosX();

	cv::Size size = frame.size();

	cv::Size dstSize = dst.size();

	cv::Size pip1Size(
			std::min(config->getCropWindowMaxWidth(),
					(unsigned int) (size.width / 2)),
			std::min(config->getCropWindowMaxHeight(),
					(unsigned int) (size.height / 2)));

	int pip_x = dstSize.width - pip1Size.width - pip_x_limit;
	cv::Rect pip1Rect(cv::Point(pip_x, pip_y_limit), pip1Size);

	cv::Mat pip1;
	cv::resize(frame, pip1, pip1Size);

	cv::rectangle(dst, pip1Rect, cv::Scalar(0, 0, 255), 5);

	cv::Mat dst0 = dst(pip1Rect);
	pip1.copyTo(dst0);
}

cv::VideoCapture* CycloTracker::openVideoCapture() {
	if (config->getSourceType() == REG_FILE) {
		std::cout
				<< "**Abrindo arquivo fonte de video.                        **"
				<< std::endl;
		cap = new cv::VideoCapture(config->getSourceFileName());

	} else {
		std::cout << "** Abrindo dispositivo fonte de video.                 **"
				<< std::endl;

		cap = new cv::VideoCapture(config->getSourceDeviceId());

	}
	if (!cap) {
		std::cerr << "Error: could not create a VideoCapture object"
				<< std::endl;
		exit(EXIT_FAILURE);
	} else if (!cap->isOpened()) {
		std::cout << "Verifique o arquivo fonte do vídeo." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "** Fonte de video está pronto.                        **"
			<< std::endl;

	return cap;
}

VideoOutput* CycloTracker::openVideoOutput() {
	if (config->hasOutputDeviceName()) {
		std::cout << "**Abrindo dispositivo de saida de video "
				<< config->getOutputDeviceName() << std::endl;
		outputDevice = new VideoOutput(config->getOutputDeviceName().c_str());

		if (!outputDevice->isOpened()) {
			std::cerr << "Erro ao abrir o dispostivo para saída do video"
					<< std::endl;
			exit(EXIT_FAILURE);
		}
		std::cout
				<< "**Dispositivo de saida preparado.                        **"
				<< std::endl;
	}
	return outputDevice;
}

cv::VideoWriter* CycloTracker::openVideoWriter() {
	if (config->hasRecordFileName()) {
		std::cout << "** Gravando video em: " << config->getRecordFileName()
				<< std::endl;
		outputFile = new cv::VideoWriter(config->getRecordFileName(),
				CV_FOURCC('M', 'P', 'E', 'G'), 30, config->getFullFrameSize());
		if (!outputFile->isOpened()) {
			std::cerr << "Erro ao abrir o arquivo para saída do video"
					<< std::endl;
			exit(EXIT_FAILURE);
		}
		std::cout
				<< "**Dispositivo de saida preparado.                        **"
				<< std::endl;
	}
	return outputFile;
}

void CycloTracker::setPreCallBack(TrackerCallback tcb) {
	CycloTracker::preCallBack = tcb;
}
void CycloTracker::setPosCallBack(TrackerCallback tcb) {
	CycloTracker::posCallBack = tcb;
}
