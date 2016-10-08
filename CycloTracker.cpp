#include<opencv2/opencv.hpp>
#include<boost/format.hpp>
#include<algorithm>
#include<iostream>
#include<vector>
#include<map>
#include<getopt.h>
#include<unistd.h>
#include<fcntl.h>

#include "CycloConfig.hpp"
#include "Utils.hpp"
#include "ImageProcessor.hpp"
#include "InteractionHandler.hpp"
#include "ObjectTracker.hpp"
#include "VideoOutput.hpp"
#include "Sensors.hpp"
#include "TrackedObject.hpp"

void MouseCallback(int event, int x, int y,
		int __attribute__((__unused__)) flags,
		void __attribute__((__unused__)) *data) {
	CycloConfig config = CycloConfig::get();
	static InteractionHandler ih(config.getAddress());
	if (ih.hasCurrentCallbackFunction()) {
		switch (event) {
		case (cv::EVENT_LBUTTONDOWN):
			ih.callCurrentCallBack(x, y);
			ih.nextStep();
			break;
		default:
			//printf("Error: MouseCallback: event = %d\n", event);
			//exit(EXIT_FAILURE);
			break;
		}
	}
}

#define DEV_FILE 0x02
#define REG_FILE 0x01
#define UNK_FILE 0x00
int main(int argc, char **argv) {

	unsigned long debug = 1;

	std::cout << "*****************************************************"
			<< std::endl;
	std::cout << "** Obtendo configurações                           **"
			<< std::endl;
	CycloConfig config = CycloConfig::get();
	std::cout << "** configurações carregadas                        **"
			<< std::endl;
	std::cout.flush();

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
	std::string sensor_device = ""; //sensor device file

	bool pick_points = false;

	int source = UNK_FILE;

	VideoOutput *outputDevice = nullptr;

	std::thread *sensorsThread = nullptr;

	SensorData *sd = nullptr;
	const char *c;

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
			stream_device = optarg;
			if (!stream_device.empty()) {
				c = stream_device.c_str();
				outputDevice = new VideoOutput(c);
				if (!outputDevice->isOpened()) {
					std::cerr
							<< "Erro ao abrir o dispostivo para saída do video"
							<< std::endl;
					exit(EXIT_FAILURE);
				}
			}
			break;
		case 'a':
			config.setAddress(optarg);
			break;
		case 't':
			sensor_device = optarg;
			if (!test_file(sensor_device)) {
				std::cout << "Verifique o dispositivo sensor." << std::endl;
				exit(EXIT_FAILURE);
			}
			c = sensor_device.c_str();
			sd = new SensorData();
			sensorsThread = StartSensorsThread(c, sd);
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
	} else if (source == UNK_FILE) {
		std::cout << "You must specify where data comes from. Use either:"
				<< std::endl << "\t--reg_source <source_file>." << std::endl
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
			if (debug)
				std::cout << "Verifique o dispositivo fonte do vídeo."
						<< std::endl;
			exit(EXIT_FAILURE);
		}
	}

	if (!cap) {
		if (debug)
			std::cout << "Error: could not create a VideoCapture object"
					<< std::endl;
		exit(EXIT_FAILURE);
	}

	if (debug)
		std::cout
				<< "** Obtendo o primeiro frame, para dimensionamento de janela"
				<< std::endl;
	// Obtemos o primeiro Frame para calculos de tamanho da janela
	cv::Mat frame;
	*cap >> frame;

	cv::Size frame_size = frame.size();
	if (debug)
		std::cout
				<< boost::format(
						"** Dimensionando janela, largura: %d, comprimento: %d")
						% frame_size.height % frame_size.width << std::endl;

	cv::VideoWriter *outputFile = nullptr;
	if (!record_file.empty()) {
		std::cout << "** Gravando video em: " << record_file << std::endl;
		outputFile = new cv::VideoWriter(record_file,
				CV_FOURCC('M', 'P', 'E', 'G'), 30, frame_size);
	}

	if (debug)
		std::cout << "** Criando Janela Principal." << std::endl;
	cv::imshow(config.getAddress(), frame);

	config.setFrameSize(frame_size);

	if (pick_points) {
		if (debug)
			std::cout
					<< "** Definindo Callback para obtenção de novas configurações"
					<< std::endl;

		cv::setMouseCallback(config.getAddress(), MouseCallback, nullptr);
	}

	cv::Rect interestArea(config.getInterestX(0), config.getInterestY(0),
			config.getInterestWidth(), config.getInterestHeight());
	if (debug)
		std::cout
				<< boost::format(
						"** Janela de Interesse: (%d,%d), largura: %d, comprimento: %d")
						% config.getInterestX(0) % config.getInterestY(0)
						% config.getInterestWidth() % config.getInterestHeight()
				<< std::endl;

	if (debug)
		std::cout << "** Criando rastreador de objetos " << std::endl;
	ObjectTracker ot(30, 50, interestArea);

	if (debug)
		std::cout << "** Criando Processador de imagens" << std::endl;

	ImageProcessor ip;

	cv::Point lCounter(config.getCounterX(0), config.getCounterY(0));
	cv::Point rCounter(config.getCounterX(1), config.getCounterY(1));
	if (debug)
		std::cout
				<< boost::format(
						"** Contador a Esquerda: (%d,%d), a Direita (%d,%d)")
						% config.getCounterX(0) % config.getCounterY(0)
						% config.getCounterX(1) % config.getCounterY(1)
				<< std::endl;

	cv::Point2f p0(config.getX(0), config.getY(0));
	cv::Point2f p1(config.getX(1), config.getY(1));
	cv::Point2f p2(config.getX(2), config.getY(2));
	cv::Point2f p3(config.getX(3), config.getY(3));
	if (debug)
		std::cout
				<< boost::format(
						"** Preparando Perspectiva: Ponto a esquerda: (%d,%d), a Direita (%d,%d)")
						% config.getX(0) % config.getY(0) % config.getX(1)
						% config.getY(1) << std::endl
				<< boost::format(
						"                           Esquerda inferior: (%d,%d), a direita (%d,%d)")
						% config.getX(2) % config.getY(2) % config.getX(3)
						% config.getY(3) << std::endl;
	cv::Rect cropFrame(config.getCropX(0), config.getCropY(0),
			config.getCropWidth(), config.getCropHeight());
	if (debug)
		std::cout
				<< boost::format(
						"** Janela de Corte: (%d,%d), largura: %d, comprimento: %d")
						% config.getCropX(0) % config.getCropY(0)
						% config.getCropWidth() % config.getCropHeight()
				<< std::endl;
	cv::Mat fore;
	if (debug)
		std::cout << "** Inciando loop de processamento das imagens"
				<< std::endl;
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

		std::cout << "** passei **" << debug++ << std::endl;
		std::cout.flush();
		ProvideOsd(full, sd, ot);
		ProvidePip(frame, full);

		std::cout << "** passei **" << debug++ << std::endl;
		std::cout.flush();
		cv::imshow(config.getAddress(), frame);
		cv::imshow("Full Frame", full);

		std::cout << "** passei **" << debug++ << std::endl;
		std::cout.flush();
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
