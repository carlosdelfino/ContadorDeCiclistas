/*
 * CycloConfig.cpp
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#include "CycloConfig.hpp"
#include<opencv2/opencv.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <string>
#include <exception>
#include <iostream>
namespace pt = boost::property_tree;

CycloConfig& CycloConfig::get() {
#if __cplusplus < 201103L
#warning "Este código é seguro com o uso do C+11, pode haver problemas relativo a concorrencia em verões inferiores."
#endif
	static CycloConfig instance;
	return instance;
}

void CycloConfig::setFrameSize(cv::Size size) {
	this->data.x[0] = 0;
	this->data.y[0] = 0;

	this->data.x[1] = size.width;
	this->data.y[1] = 0;

	this->data.x[2] = 0;
	this->data.y[2] = size.height;

	this->data.x[3] = size.width;
	this->data.y[3] = size.height;
}

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
	cv::Point pt;
	pt.x = this->data.x_counter[index];
	pt.y = this->data.y_counter[index];
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

void CycloConfig::PersistData() {

	std::cout << "** CycloConfig> Persistindo configurações" << std::endl;
	pt::ptree tree;

	std::cout << "** CycloConfig> gravando valor dos contadores" << std::endl;
	tree.put(PROP_COUNTER_LEFT, this->GetLeftCounter());
	tree.put(PROP_COUNTER_RIGHT, this->GetRightCounter());

	std::cout << "** CycloConfig> gravando posição dos contadores" << std::endl;
	tree.put(PROP_COUNTER_LEFT_X, this->getCounterX(0));
	tree.put(PROP_COUNTER_LEFT_Y, this->getCounterY(0));
	tree.put(PROP_COUNTER_RIGHT_X, this->getCounterX(1));
	tree.put(PROP_COUNTER_RIGHT_Y, this->getCounterY(1));

	std::cout << "** CycloConfig> gravando area de corte" << std::endl;
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

	std::cout << "** CycloConfig> Finalizando Persistencia da configurações"
			<< std::endl;

	pt::write_json(configFile, tree);

	std::cout << "** CycloConfig> Configurações Armazenada" << std::endl;
}

void CycloConfig::LoadData() {

	std::cout << "** CycloConfig> Lendo configurações" << std::endl;

	pt::ptree tree;
	try {
		try {
			pt::read_json(configFile, tree);
		} catch (boost::property_tree::json_parser::json_parser_error& e) {
			std::cerr << e.what() << std::endl;
			std::cout << "**#############################" << std::endl;
			std::cout
					<< "** CycloConfig> Nenhum arquivo encontrado, criando um novo."
					<< std::endl;

			this->PersistData();

			std::cout << "**#############################" << std::endl;

			pt::read_json(configFile, tree);

			std::cout << "**#############################" << std::endl;
		}

		if (tree.empty()) {
			std::cout << "**#############################" << std::endl;
			std::cout << "** CycloConfig> Criando um novo arquivo, atual vazio"
					<< std::endl;

			this->PersistData();

			std::cout << "**#############################" << std::endl;

			pt::read_json(configFile, tree);

			std::cout << "**#############################" << std::endl;
		}

		std::cout << "** CycloConfig> Lendo valor dos contadores" << std::endl;
		this->SetLeftCounter(tree.get<int>(PROP_COUNTER_LEFT));
		this->SetRightCounter(tree.get<int>(PROP_COUNTER_RIGHT));

		std::cout << "** CycloConfig> Lendo posição dos contadores"
				<< std::endl;
		this->setCounterX(0, tree.get<int>(PROP_COUNTER_LEFT_X));
		this->setCounterY(0, tree.get<int>(PROP_COUNTER_LEFT_Y));
		this->setCounterX(1, tree.get<int>(PROP_COUNTER_RIGHT_X));
		this->setCounterY(1, tree.get<int>(PROP_COUNTER_RIGHT_Y));

		std::cout << "** CycloConfig> Lendo area de corte" << std::endl;
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

unsigned long CycloConfig::countInteraction() {
	return ++interaction;
}
