#include<opencv2/opencv.hpp>
#include<algorithm>
#include<iostream>
#include<vector>
#include<map>
#include<getopt.h>
#include<unistd.h>
#include<fcntl.h>

#include "InteractionHandler.h"
#include "Utils.hpp"
#include "ImageProcessor.hpp"
#include "ObjectTracker.hpp"
#include "VideoOutput.hpp"
#include "Sensors.hpp"
#include "TrackedObject.hpp"

#define DEV_FILE 0x02
#define REG_FILE 0x01
#define UNK_FILE 0x00
int main(int argc, char **argv) {

	static struct option long_options[] = { { "reg_source", required_argument,
			nullptr, 's' }, { "override", required_argument, nullptr, 'O' }, {
			"dev_source", required_argument, nullptr, 'D' }, { "record",
			required_argument, nullptr, 'r' }, { "stream", required_argument,
			nullptr, 'S' }, { "address", required_argument, nullptr, 'a' }, {
			"sensor", required_argument, nullptr, 't' }, { "help", no_argument,
			nullptr, 'h' }, { nullptr, 0, nullptr, 0 } };
	int opt = 0;
	int long_index = 0;

	std::string source_file = ""; //where data comes from if source is a
								  //regular file.
	int source_device = -1; //where data comes from if source is
							//device file.
	std::string record_file = ""; //output file.
	std::string stream_device = ""; //device file used to stream data.
	std::string address = ""; //address where program is running on.
	std::string sensor_device = ""; //sensor device file

	bool pick_points = false;

	int source = UNK_FILE;

	VideoOutput *outputDevice = nullptr;

	std::thread *sensorsThread = nullptr;

	SensorData sd;

	while ((opt = getopt_long(argc, argv, "s:r:S:t:a:D:hO", long_options,
			&long_index)) != -1) {
		switch (opt) {
		case 's':
			source_file = optarg;
			source |= REG_FILE;
			break;
		case 'D':
			source_device = atoi(optarg);
			source |= DEV_FILE;
			break;
		case 'r':
			record_file = optarg;
			break;
		case 'S':

			if (!stream_device.empty()) {
				const char *c = stream_device.c_str();
				outputDevice = new VideoOutput(c);
				if (!outputDevice->isOpen()) {
					outputDevice->getError();
					exit (EXIT_FAILUREt);
				}
			}
			break;
		case 'a':
			address = optarg;
			if (address.empty()) {
				std::cout << "You must specify your address. "
						<< "Like --address faria_lima-1280." << std::endl;
				exit(EXIT_FAILURE);
			}
			break;
		case 't':
			sensor_device = optarg;
			if (!test_file(sensor_device)) {
				std::cout << "Verifique o dispositivo sensor." << std::endl;
				exit(EXIT_FAILURE);
			}
			const char *c = sensor_device.c_str();
			sensorsThread = StartSensorsThread(c, &sd);
			break;
		case 'h':
			print_usage((std::string) argv[0]);
			exit(EXIT_SUCCESS);
			break;
		case 'O':
			pick_points = true;
			break;
		default:
			exit(EXIT_FAILURE);
			break;
		}
	}

	if (source == (REG_FILE | DEV_FILE)) {
		std::cout << "You can only have one source. It has to be either a "
				<< "regular file or a device file, not both." << std::endl;
		exit(EXIT_FAILURE);
	} else {
		std::cout << "You must specify where data comes from. Use either:"
				<< std::endl << "\t--source <source_file>." << std::endl
				<< "\t--dev_source <device number>." << std::endl
				<< "\tOr --help to print usage." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (((source & REG_FILE) && (source_file == record_file))
			|| ((source & REG_FILE) && (source_file == stream_device))
			|| ((record_file == stream_device) && !record_file.empty())) {
		std::cout << "Files must not be the same." << std::endl;
		exit(EXIT_FAILURE);
	}

	cv::VideoCapture *cap = nullptr;

	//Check all files forced.
	if (source == REG_FILE) {
		if (!test_file(source_file)) {
			std::cout << "Verifique o dispositivo se está disponível."
					<< std::endl;
			exit(EXIT_FAILURE);
		}

		cap = new cv::VideoCapture(source_file);
		if (!cap->isOpened()) {
			std::cout << "Verifique o arquivo fonte do vídeo." << std::endl;
			exit(EXIT_FAILURE);
		}
	} else if (source == DEV_FILE) {
		cap = new cv::VideoCapture(source_device);
		if (!cap->isOpened()) {
			std::cout << "Verifique o dispositivo fonte do vídeo." << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	if (!cap) {
		std::cout << "Error: could not create a VideoCapture object"
				<< std::endl;
		exit(EXIT_FAILURE);
	}

	// Obtemos o primeiro Frame para calculos de tamanho da janela
	cv::Mat frame;
	*cap >> frame;

	cv::Size frame_size = frame.size();

	cv::VideoWriter *outputFile = nullptr;
	if (!record_file.empty())
		outputFile = new cv::VideoWriter(record_file,
				CV_FOURCC('M', 'P', 'E', 'G'), 30, frame_size);

	cv::imshow(address, frame);

	CycloConfig *config = CycloConfig::get();

	config->setFrameSize(frame_size);

	config->LoadData();

	if (pick_points) {
		InteractionHandler::Subscribe(address);
	}

	cv::Rect interestArea(config->getInterestX(0), config->getInterestY(0),
			config->getInterestWidth(), config->getInterestHeight());
	ObjectTracker ot(30, 50, interestArea);

	ImageProcessor ip;

	cv::Point lCounter(config->getCounterX(0), config->getCounterY(0));
	cv::Point rCounter(config->getCounterX(1), config->getCounterY(1));

	cv::Point2f p0(config->getX(0), config->getY(0));
	cv::Point2f p1(config->getX(1), config->getY(1));
	cv::Point2f p2(config->getX(2), config->getY(2));
	cv::Point2f p3(config->getX(3), config->getY(3));

	cv::Rect cropFrame(config->getCropX(0), config->getCropY(0),
			config->getCropWidth(), config->getCropHeight());

	cv::Mat fore;
	do {
		*cap >> frame; // captura um novo frame e processa imediatamente

		if (outputFile)
			outputFile->write(frame); //Write avi file

		ip.PrepareFrame(frame, cropFrame, p0, p1, p2, p3);

		fore = ip.AcquireForeground(frame);
		ip.InsertInterestArea(frame, interestArea);
		ot.IterateTracker(frame, fore);

		cv::Mat full = frame.clone();
		ot.PrintTotal(full);
		ot.PrintLeftPartial(full, lCounter);
		ot.PrintRightPartial(full, rCounter);

		ProvideOsd(full, sensor_device.empty() ? nullptr : &sd, ot);
		ProvidePip(frame, full);

		cv::imshow(address, frame);
		cv::imshow("Full Frame", full);

		if (outputDevice)
			outputDevice->write(full);

		if (IsMidnight()) {
			ot.ZeroCounters();
		}
	} while (char(cv::waitKey(30)) == char(27));

	if (!sensor_device.empty()) {
		sensorsThread->join();
	}

	delete cap;
	if (outputDevice)
		delete outputDevice;
	if (outputFile)
		delete outputFile;

	exit(0);
	return 0;
}
