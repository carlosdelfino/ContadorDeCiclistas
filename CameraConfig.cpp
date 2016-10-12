/*
 * CycloConfig.cpp
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#include "CameraConfig.hpp"

#include <opencv2/opencv.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <string>
#include <exception>
#include <iostream>

namespace pt = boost::property_tree;

CameraConfig::CameraConfig(std::string configFile) {
	this->configFileName = configFile;
	this->LoadData();
	std::cout << "** CameraConfig> Criado." << std::endl;
}

CameraConfig::~CameraConfig() {
	this->PersistData();
	std::cout << "** CameraConfig> Descartado." << std::endl;
}

void CameraConfig::PersistData() {
	PersistData(this->configFileName);
}
void CameraConfig::PersistData(std::string fileName) {

	std::cout << "** CameraConfig> Persistindo configurações" << std::endl;
	pt::ptree tree;
	tree.put(PROP_CAMERA_COS_THETA, this->getCosTheta());
	tree.put(PROP_CAMERA_SIN_THETA, this->getSinTheta());
	tree.put(PROP_CAMERA_THETA, this->getTheta());
	tree.put(PROP_CAMERA_DISTANCE_FOCAL, this->getFocalDistance());
	tree.put(PROP_CAMERA_DISTANCE_RZ, this->getRz());
	tree.put(PROP_CAMERA_FRAME_COLS, this->getFrameCols());
	tree.put(PROP_CAMERA_FRAME_ROWS, this->getFrameRows());
	tree.put(PROP_CAMERA_HEIGHT, this->getHeight());

	std::cout << "** CameraConfig> Finalizando Persistencia da configurações"
			<< std::endl;

	pt::write_json(fileName, tree);

	std::cout << "** CameraConfig> Configurações Armazenada" << std::endl;
}

void CameraConfig::LoadData() {

	std::cout << "** CameraConfig> Lendo configurações" << std::endl;

	pt::ptree tree;
	try {
		try {
			pt::read_json(configFileName, tree);
		} catch (boost::property_tree::json_parser::json_parser_error& e) {
			std::cerr << e.what() << std::endl;
			std::cout << "**#############################" << std::endl;
			std::cout
					<< "** CycloConfig> Nenhum arquivo encontrado, criando um novo."
					<< std::endl;

			this->PersistData();

			std::cout << "**#############################" << std::endl;

			pt::read_json(configFileName, tree);

			std::cout << "**#############################" << std::endl;
		}

		if (tree.empty()) {
			std::cout << "**#############################" << std::endl;
			std::cout << "** CycloConfig> Criando um novo arquivo, atual vazio"
					<< std::endl;

			this->PersistData();

			std::cout << "**#############################" << std::endl;

			pt::read_json(configFileName, tree);

			std::cout << "**#############################" << std::endl;
		}

		this->setCosTheta(tree.get<float>(PROP_CAMERA_COS_THETA));
		this->setSinTheta(tree.get<float>(PROP_CAMERA_SIN_THETA));
		this->setTheta(tree.get<float>(PROP_CAMERA_THETA));
		this->setFocalDistance(tree.get<float>(PROP_CAMERA_DISTANCE_FOCAL));
		this->setRz(tree.get<float>(PROP_CAMERA_DISTANCE_RZ));
		this->setFrameCols(tree.get<int>(PROP_CAMERA_FRAME_COLS));
		this->setFrameRows(tree.get<int>(PROP_CAMERA_FRAME_ROWS));
		this->setHeight(tree.get<float>(PROP_CAMERA_HEIGHT));

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

int CameraConfig::getFrameRows() {
	return data.frameRows;
}
int CameraConfig::getFrameCols() {
	return data.frameCols;
}
float CameraConfig::getTheta() {
	return data.theta;
}
float CameraConfig::getCosTheta() {
	return data.cosTheta;
}
float CameraConfig::getSinTheta() {
	return data.sinTheta;
}
float CameraConfig::getFocalDistance() {
	return data.focalDistance;
}
float CameraConfig::getHeight() {
	return data.height;
}
float CameraConfig::getRz() {
	return data.Rz;
}

void CameraConfig::setFrameRows(int v) {
	data.frameRows = v;
}
void CameraConfig::setFrameCols(int v) {
	data.frameCols = v;
}
void CameraConfig::setTheta(float v) {
	data.theta = v;
}
void CameraConfig::setCosTheta(float v) {
	data.cosTheta = v;
}
void CameraConfig::setSinTheta(float v) {
	data.theta = v;
}
void CameraConfig::setFocalDistance(float v) {
	data.focalDistance = v;
}
void CameraConfig::setHeight(float v) {
	data.height = v;
}
void CameraConfig::setRz(float v) {
	data.Rz = v;
}
