/*
 * CycloConfig.cpp
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#include "CycloConfig.hpp"

#include <opencv2/opencv.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <string>
#include <exception>
#include <iostream>
#include<getopt.h>

#include "Utils.hpp"

namespace pt = boost::property_tree;

CycloConfig* CycloConfig::get() {
#if __cplusplus < 201103L
#warning "Este código é seguro com o uso do C+11, pode haver problemas relativo a concorrencia em verões inferiores."
#endif
	static CycloConfig *instance = new CycloConfig();
	return instance;
}

void CycloConfig::setFullFrameSize(cv::Size size) {

	this->data.width = size.width;
	this->data.height = size.height;

	if (reconfigure()) {
		setPerspectiveArea(size);
		setCropArea(size);
		setInterestArea(size);
	}
}

void CycloConfig::setFullFrameWidth(unsigned int w) {
	this->data.width = w;
}

void CycloConfig::setFullFrameHeight(unsigned int h) {
	this->data.height = h;
}

cv::Size CycloConfig::getFullFrameSize() {
	cv::Size sz(getFullFrameWidth(), getFullFrameHeight());
	return sz;
}

/**
 * @deprecated
 */
void CycloConfig::setPerspectiveArea(cv::Size size) {
	this->data.x[0] = 0;
	this->data.y[0] = 0;

	this->data.x[1] = size.width;
	this->data.y[1] = 0;

	this->data.x[2] = 0;
	this->data.y[2] = size.height;

	this->data.x[3] = size.width;
	this->data.y[3] = size.height;
}

/**
 * @deprecated
 */
void CycloConfig::setInterestArea(cv::Size size) {
	this->data.x_interest[0] = 0;
	this->data.y_interest[0] = 0;
	this->data.x_interest[1] = size.width;
	this->data.y_interest[1] = size.height;
}

/**
 * @deprecated
 */
void CycloConfig::setCropArea(cv::Size size) {
	this->data.x_crop[0] = 0;
	this->data.y_crop[0] = 0;
	this->data.x_crop[1] = size.width;
	this->data.y_crop[1] = size.height;
}

CycloConfig::CycloConfig() {
	this->LoadData();
	std::cout << "** CycloConfig> Criado." << std::endl;
}

CycloConfig::~CycloConfig() {
	this->PersistData();
	std::cout << "** CycloConfig> Descartado." << std::endl;

}

unsigned int CycloConfig::GetLeftCounter() {
	return this->data.left_counter;
}

void CycloConfig::SetLeftCounter(unsigned int counter) {
	this->data.left_counter = counter;
}

unsigned int CycloConfig::GetRightCounter() {
	return this->data.right_counter;
}

void CycloConfig::SetRightCounter(unsigned int counter) {
	this->data.right_counter = counter;
}

cv::Point CycloConfig::GetCounterPos(unsigned int index) {
	cv::Point pt(this->data.x_counter[index], this->data.y_counter[index]);
	return pt;
}

cv::Point CycloConfig::GetPerspectivePos(unsigned int index) {
	cv::Point pt;
	pt.x = this->data.x[index];
	pt.y = this->data.y[index];
	return pt;
}

cv::Point CycloConfig::GetCropPos(unsigned int index) {
	cv::Point pt;
	pt.x = this->data.x_crop[index];
	pt.y = this->data.y_crop[index];
	return pt;
}

cv::Point CycloConfig::GetInterestPos(unsigned int index) {
	cv::Point pt;
	pt.x = this->data.x_interest[index];
	pt.y = this->data.y_interest[index];
	return pt;
}

unsigned int CycloConfig::getFullFrameHeight() {
	return this->data.height;
}

unsigned int CycloConfig::getFullFrameWidth() {
	return this->data.width;
}

void CycloConfig::clearPerspectiveArea() {
	setPerspectiveArea(getFullFrameSize());
}

void CycloConfig::clearCropArea() {
	setCropArea(getFullFrameSize());
}

void CycloConfig::setPathImageLogo(std::string p) {
	this->data.pathImageLogo = p;
}

void CycloConfig::setPathImageCyclist(std::string p) {
	this->data.pathImageCyclist = p;
}

std::string CycloConfig::getPathPrincipalLogo() {
	return this->data.pathImageLogo;
}

std::string CycloConfig::getPathSecondLogo() {
	return this->data.pathImageCyclist;
}

void CycloConfig::clearInteresstArea() {
	setInterestArea(getFullFrameSize());
}

void CycloConfig::PersistData() {
	PersistData(this->configFileName);
}
void CycloConfig::PersistData(std::string fileName) {

	std::cout << "** CycloConfig> Persistindo configurações" << std::endl;
	pt::ptree tree;

	std::cout << "** CycloConfig> gravando tamanho padrão da janela principal"
			<< std::endl;
	tree.put(PROP_FULL_FRAME_WIDTH, this->getFullFrameWidth());
	tree.put(PROP_FULL_FRAME_HEIGHT, this->getFullFrameHeight());

	std::cout << "** CycloConfig> gravando valor dos contadores" << std::endl;
	tree.put(PROP_DETECTEDOBJ_COUNTER_1, this->GetLeftCounter());
	tree.put(PROP_DETECTEDOBJ_COUNTER_2, this->GetRightCounter());

	std::cout << "** CycloConfig> gravando posição dos contadores" << std::endl;
	tree.put(PROP_DETECTEDOBJ_COUNTER_1_X, this->getCounterX(0));
	tree.put(PROP_DETECTEDOBJ_COUNTER_1_Y, this->getCounterY(0));
	tree.put(PROP_DETECTEDOBJ_COUNTER_2_X, this->getCounterX(1));
	tree.put(PROP_DETECTEDOBJ_COUNTER_2_Y, this->getCounterY(1));

	tree.put(PROP_COUNTER_DETECTEDOBJ_TOTAL, getCounterDetectedObject());

	std::cout << "** CycloConfig> gravando area de corte" << std::endl;
	tree.put(PROP_CROP_WINDOW_X, this->getCropWindowPosX());
	tree.put(PROP_CROP_WINDOW_Y, this->getCropWindowPosY());
	tree.put(PROP_CROP_LEFT_X, this->getCropX(0));
	tree.put(PROP_CROP_LEFT_Y, this->getCropY(0));
	tree.put(PROP_CROP_RIGHT_X, this->getCropX(1));
	tree.put(PROP_CROP_RIGHT_Y, this->getCropY(1));

	std::cout << "** CycloConfig> gravando area de interesse" << std::endl;
	tree.put("interest.start_x", this->getInterestX(0));
	tree.put("interest.start_y", this->getInterestY(0));
	tree.put("interest.end_x", this->getInterestX(1));
	tree.put("interest.end_y", this->getInterestY(1));

	std::cout << "** CycloConfig> gravando perspectiva" << std::endl;
	tree.put(PROP_PERSPECTIVE_SUPERIOR_LEFT_X, this->getX(0));
	tree.put(PROP_PERSPECTIVE_SUPERIOR_LEFT_Y, this->getY(0));
	tree.put(PROP_PERSPECTIVE_SUPERIOR_RIGHT_X, this->getX(1));
	tree.put(PROP_PERSPECTIVE_SUPERIOR_RIGHT_Y, this->getY(1));
	tree.put(PROP_PERSPECTIVE_INFERIOR_LEFT_X, this->getX(2));
	tree.put(PROP_PERSPECTIVE_INFERIOR_LEFT_Y, this->getY(2));
	tree.put(PROP_PERSPECTIVE_INFERIOR_RIGHT_X, this->getX(3));
	tree.put(PROP_PERSPECTIVE_INFERIOR_RIGHT_Y, this->getY(3));

	tree.put(PROP_COUNTER_DETECTEDOBJ_FILE_PATH, getFilePahtDetectedObject());
	tree.put(PROP_COUNTER_DETECTEDOBJ_FILE_PREFIX,
			getFilePrefixDetectedObject());
	tree.put(PROP_COUNTER_DETECTEDOBJ_FILE_EXTENSION,
			getFileExtensionDetectedObject());

	std::cout << "** CycloConfig> gravando Logo Principal atual" << std::endl;
	tree.put(PROP_LOGO_PRINCIPAL, this->getPathPrincipalLogo());

	std::cout << "** CycloConfig> gravando Logo Secundário atual" << std::endl;
	tree.put(PROP_LOGO_SECOND, this->getPathSecondLogo());

	std::cout << "** CycloConfig> gravando endereço" << std::endl;
	tree.put(PROP_ADDRESS, this->getAddress());

	std::cout << "** CycloConfig> Finalizando Persistencia da configurações"
			<< std::endl;

	pt::write_json(fileName, tree);

	std::cout << "** CycloConfig> Configurações Armazenada" << std::endl;
}

void CycloConfig::LoadData() {

	std::cout << "** CycloConfig> Lendo configurações" << std::endl;

	pt::ptree tree;
	try {
		try {
			pt::read_json(this->configFileName, tree);
		} catch (boost::property_tree::json_parser::json_parser_error& e) {
			std::cerr << e.what() << std::endl;
			std::cout << "**#############################" << std::endl;
			std::cout
					<< "** CycloConfig> Nenhum arquivo encontrado, criando um novo."
					<< std::endl;

			this->PersistData();

			std::cout << "**#############################" << std::endl;

			pt::read_json(this->configFileName, tree);

			std::cout << "**#############################" << std::endl;
		}

		if (tree.empty()) {
			std::cout << "**#############################" << std::endl;
			std::cout << "** CycloConfig> Criando um novo arquivo, atual vazio"
					<< std::endl;

			this->PersistData();

			std::cout << "**#############################" << std::endl;

			pt::read_json(this->configFileName, tree);

			std::cout << "**#############################" << std::endl;
		}

		std::cout
				<< "** CycloConfig> gravando tamanho padrão da janela principal"
				<< std::endl;
		this->setFullFrameWidth(tree.get<unsigned int>(PROP_FULL_FRAME_WIDTH));
		this->setFullFrameHeight(
				tree.get<unsigned int>(PROP_FULL_FRAME_HEIGHT));

		std::cout << "** CycloConfig> Lendo valor dos contadores" << std::endl;
		this->SetLeftCounter(tree.get<int>(PROP_DETECTEDOBJ_COUNTER_1));
		this->SetRightCounter(tree.get<int>(PROP_DETECTEDOBJ_COUNTER_2));

		std::cout << "** CycloConfig> Lendo posição dos contadores"
				<< std::endl;
		this->setCounterX(0, tree.get<int>(PROP_DETECTEDOBJ_COUNTER_1_X));
		this->setCounterY(0, tree.get<int>(PROP_DETECTEDOBJ_COUNTER_1_Y));
		this->setCounterX(1, tree.get<int>(PROP_DETECTEDOBJ_COUNTER_2_X));
		this->setCounterY(1, tree.get<int>(PROP_DETECTEDOBJ_COUNTER_2_Y));

		setCounterDetectedObject(
				tree.get<unsigned int>(PROP_COUNTER_DETECTEDOBJ_TOTAL));

		setFilePahtDetectedObject(
				tree.get<std::string>(PROP_COUNTER_DETECTEDOBJ_FILE_PATH));
		setFilePrefixDetectedObject(
				tree.get<std::string>(PROP_COUNTER_DETECTEDOBJ_FILE_PREFIX));
		setFileExtensionDetectedObject(
				tree.get<std::string>(PROP_COUNTER_DETECTEDOBJ_FILE_EXTENSION));

		std::cout << "** CycloConfig> Lendo area de corte" << std::endl;
		this->setCropWindowX(tree.get<unsigned int>(PROP_CROP_WINDOW_X));
		this->setCropWindowY(tree.get<unsigned int>(PROP_CROP_WINDOW_Y));
		this->setCropX(0, tree.get<int>(PROP_CROP_LEFT_X));
		this->setCropY(0, tree.get<int>(PROP_CROP_LEFT_Y));
		this->setCropX(1, tree.get<int>(PROP_CROP_RIGHT_X));
		this->setCropY(1, tree.get<int>(PROP_CROP_RIGHT_Y));

		std::cout << "** CycloConfig> Lendo area de interesse" << std::endl;
		this->setInterestX(0, tree.get<int>("interest.start_x"));
		this->setInterestY(0, tree.get<int>("interest.start_y"));
		this->setInterestX(1, tree.get<int>("interest.end_x"));
		this->setInterestY(1, tree.get<int>("interest.end_y"));

		std::cout << "** CycloConfig> Lendo perspectiva" << std::endl;
		this->setX(0, tree.get<int>(PROP_PERSPECTIVE_SUPERIOR_LEFT_X));
		this->setY(0, tree.get<int>(PROP_PERSPECTIVE_SUPERIOR_LEFT_Y));
		this->setX(1, tree.get<int>(PROP_PERSPECTIVE_SUPERIOR_RIGHT_X));
		this->setY(1, tree.get<int>(PROP_PERSPECTIVE_SUPERIOR_RIGHT_Y));
		this->setX(2, tree.get<int>(PROP_PERSPECTIVE_INFERIOR_LEFT_X));
		this->setY(2, tree.get<int>(PROP_PERSPECTIVE_INFERIOR_LEFT_Y));
		this->setX(3, tree.get<int>(PROP_PERSPECTIVE_INFERIOR_RIGHT_X));
		this->setY(3, tree.get<int>(PROP_PERSPECTIVE_INFERIOR_RIGHT_Y));

		std::cout << "** CycloConfig> Lendo Logo Principal atual" << std::endl;
		this->setPathImageLogo(tree.get<std::string>(PROP_LOGO_PRINCIPAL));

		std::cout << "** CycloConfig> Lendo Logo Secundário atual" << std::endl;
		this->setPathImageCyclist(tree.get<std::string>(PROP_LOGO_SECOND));

		std::cout << "** CycloConfig> Lendo endereço" << std::endl;
		this->setAddress(tree.get<std::string>(PROP_ADDRESS));

	} catch (boost::property_tree::ptree_bad_path& e) {
		std::cerr << e.what() << std::endl;
		this->PersistData();
		std::cout << "**#############################" << std::endl;
		std::cout
				<< "** CycloConfig> Verifique o arquivo de configuração e reinicie a aplicação."
				<< std::endl;
		exit(255);
	} catch (boost::property_tree::json_parser::json_parser_error& e) {
		std::cerr << e.what() << std::endl;
		this->PersistData();
		std::cout << "**#############################" << std::endl;
		std::cout
				<< "** CycloConfig> Verifique o arquivo de configuração e reinicie a aplicação."
				<< std::endl;
		exit(255);
	}
}

void CycloConfig::SetCounterPos(unsigned int i, int x, int y) {
	data.x_counter[i] = x;
	data.y_counter[i] = y;
}

void CycloConfig::SetPerspectivePos(unsigned int i, int x, int y) {
	data.x[i] = x;
	data.y[i] = y;
}

void CycloConfig::SetCropPos(unsigned int i, int x, int y) {
	data.x_crop[i] = x;
	data.y_crop[i] = y;
}

void CycloConfig::SetInterestPos(unsigned int i, int x, int y) {
	data.x_interest[i] = x;
	data.y_interest[i] = y;
}
void CycloConfig::SetCounterPos(unsigned int i, cv::Point pt) {
	SetCounterPos(i, pt.x, pt.y);
}
void CycloConfig::SetPerspectivePos(unsigned int i, cv::Point p) {
	SetPerspectivePos(i, p.x, p.y);
}
void CycloConfig::SetCropPos(unsigned int i, cv::Point pt) {
	SetCropPos(i, pt.x, pt.y);
}
void CycloConfig::SetInterestPos(unsigned int i, cv::Point pt) {
	SetInterestPos(i, pt.x, pt.y);
}

void CycloConfig::setX(unsigned int i, int x) {
	data.x[i] = x;
}

void CycloConfig::setY(unsigned int i, int y) {
	data.y[i] = y;
}

void CycloConfig::setCropWindowX(unsigned int x) {
	data.x_crop_windowPos = x;
}

void CycloConfig::setCropWindowY(unsigned int y) {
	data.y_crop_windowPos = y;
}

void CycloConfig::setCropX(unsigned int i, int x) {
	data.x_crop[i] = x;
}

void CycloConfig::setCropY(unsigned int i, int y) {
	data.y_crop[i] = y;
}

unsigned int CycloConfig::getX(int i) {
	return data.x[i];
}

unsigned int CycloConfig::getCounterX(int i) {
	return data.x_counter[i];
}

void CycloConfig::setCounterX(int i, int x) {
	data.x_counter[i] = x;
}

void CycloConfig::setCounterY(int i, int y) {
	data.y_counter[i] = y;
}

unsigned int CycloConfig::getCropX(int i) {
	return data.x_crop[i];
}

unsigned int CycloConfig::getInterestX(int i) {
	return data.x_interest[i];
}

void CycloConfig::setInterestX(int i, int x) {
	data.x_interest[i] = x;
}

unsigned int CycloConfig::getY(int i) {
	return data.y[i];
}

unsigned int CycloConfig::getCounterY(int i) {
	return data.y_counter[i];
}

cv::Rect CycloConfig::getCropArea() {
	cv::Rect cropArea(getCropX(0), getCropY(0), getCropWidth(),
			getCropHeight());
	return cropArea;
}
unsigned int CycloConfig::getCropY(int i) {
	return data.y_crop[i];
}

unsigned int CycloConfig::getInterestY(int i) {
	return data.y_interest[i];
}

void CycloConfig::setInterestY(int i, int y) {
	data.y_interest[i] = y;
}

unsigned int CycloConfig::getInterestWidth() {
	return getInterestX(1) - getInterestX(0);
}

unsigned int CycloConfig::getInterestHeight() {
	return getInterestY(1) - getInterestY(0);
}

unsigned int CycloConfig::getCropWidth() {
	return getCropX(1) - getCropX(0);
}
unsigned int CycloConfig::getCropHeight() {
	return getCropY(1) - getCropY(0);
}

std::string CycloConfig::getAddress() {
	return data.address;
}

void CycloConfig::setAddress(std::string a) {
	if (a.empty()) {
		std::cout << "You must specify your address. " << "faria_lima-1280."
				<< std::endl;
		exit(EXIT_FAILURE);
	}

	this->data.address = a;
}

unsigned int CycloConfig::getCropWindowPosX() {
	return data.x_crop_windowPos;
}

unsigned int CycloConfig::getCropWindowPosY() {
	return data.y_crop_windowPos;

}

unsigned int CycloConfig::getCropWindowMaxWidth() {
	return data.cropMaxWindowWidth;
}

unsigned int CycloConfig::getCropWindowMaxHeight() {
	return data.cropMaxWindowWeight;
}

cv::Rect CycloConfig::getInterestArea() {
	cv::Rect interestArea(getInterestX(0), getInterestY(0), getInterestWidth(),
			getInterestHeight());
	return interestArea;
}

bool CycloConfig::reconfigure() {
	return reconfigureFlag;
}

void CycloConfig::setReconfigure(bool b) {
	reconfigureFlag = b;
}

unsigned long CycloConfig::countInteraction() {
	// contabilizar tempo entre interações também.
	return ++interaction;
}

void CycloConfig::parseCommandOptions(int argc, char * const *argv) {

	while ((opt = getopt_long(argc, argv, "s:r:S:t:a:D:hO", long_options,
			&long_index)) != -1) {
		switch (opt) {
		case 's':
			if (source == DEV_FILE) {
				std::cout
						<< "You can only have one source. It has to be either a "
						<< "regular file or a device file, not both."
						<< std::endl;
				exit(EXIT_FAILURE);
			}
			source_file = optarg;
			source = REG_FILE;
			if (!test_file(source_file)) {
				std::cout << "Verifique o arquivo de saida se é acessível."
						<< std::endl;
				exit(EXIT_FAILURE);
			}
			break;
		case 'D':
			if (source == REG_FILE) {
				std::cout
						<< "You can only have one source. It has to be either a "
						<< "regular file or a device file, not both."
						<< std::endl;
				exit(EXIT_FAILURE);
			}

			source_device = atoi(optarg);
			source = DEV_FILE;
			break;
		case 'r':
			record_file = optarg;
			if (!test_file(source_file)) {
				std::cout << "Verifique o arquivo está acessivel." << std::endl;
				exit(EXIT_FAILURE);
			}
			break;
		case 'S':
			stream_device = optarg;
			if (!test_file(sensor_device)) {
				std::cout << "Verifique o dispositivo de stream." << std::endl;
				exit(EXIT_FAILURE);
			}
			break;
		case 'a':
			data.address = optarg;
			break;
		case 't':
			sensor_device = optarg;
			if (!test_file(sensor_device)) {
				std::cout << "Verifique o dispositivo sensor." << std::endl;
				exit(EXIT_FAILURE);
			}

			break;
		case 'h':
			print_usage((std::string) argv[0]);
			exit(EXIT_SUCCESS);
			break;
		case 'O':
			reconfigureFlag = true;
			break;
		default:
			exit(EXIT_FAILURE);
			break;
		}
	}
	if (source == UNK_FILE) {
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
	} //Check all files forced.

}

std::string CycloConfig::getOutputDeviceName() {
	return this->stream_device;
}

bool CycloConfig::hasOutputDeviceName() {
	return !this->stream_device.empty();
}

std::string CycloConfig::getSensorDeviceName() {
	return this->sensor_device;
}

std::string CycloConfig::getSourceFileName() {
	return this->source_file;
}

int CycloConfig::getSourceDeviceId() {
	return this->source_device;
}

std::string CycloConfig::getRecordFileName() {
	return this->record_file;
}

bool CycloConfig::hasRecordFileName() {
	return !this->record_file.empty();
}

bool CycloConfig::hasSensorDeviceName() {
	return !this->sensor_device.empty();
}

FileType CycloConfig::getSourceType() {
	return this->source;
}

unsigned int CycloConfig::getCounterDetectedObject() {
	return this->data.counterDetectedObject;
}
void CycloConfig::setCounterDetectedObject(unsigned int i) {
	this->data.counterDetectedObject = i;
}

void CycloConfig::setFilePahtDetectedObject(std::string d) {
	this->data.filePahtDetectedObject = d;
}

void CycloConfig::setFilePrefixDetectedObject(std::string d) {
	this->data.filePrefixDetectedObject = d;
}

void CycloConfig::setFileExtensionDetectedObject(std::string d) {
	this->data.fileExtensionDetectedObject = d;
}

std::string CycloConfig::getFilePahtDetectedObject() {
	return this->data.filePahtDetectedObject;
}

std::string CycloConfig::getFilePrefixDetectedObject() {
	return this->data.filePrefixDetectedObject;
}
std::string CycloConfig::getFileExtensionDetectedObject() {
	return this->data.fileExtensionDetectedObject;
}

unsigned int CycloConfig::getDistanceThreshold() {
	return this->data.distanceThreshold;
}

double CycloConfig::getContourThreshold() {
	return this->data.contourThreshold;
}

void CycloConfig::setDistanceThreshold(unsigned int d) {
	this->data.distanceThreshold = d;
}
void CycloConfig::setContourThreshold(double c) {
	this->data.contourThreshold = c;
}

unsigned int CycloConfig::getCaptureFrameDelay() {
	return this->data.captureFrameDelay;
}
void CycloConfig::setCaptureFrameDelay(unsigned int c) {
	this->data.captureFrameDelay = c;
}
