/*
 * CycloConfig.cpp
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#include "CycloConfig.hpp"
#include<opencv2/opencv.hpp>
#include<algorithm>
#include<iostream>
#include<vector>
#include<map>
#include<getopt.h>
#include<unistd.h>
#include<fcntl.h>

CycloConfig::CycloConfig() {
	// TODO Auto-generated constructor stub
}

CycloConfig::~CycloConfig() {
	// TODO Auto-generated destructor stub
	instance == nullptr;
}

static CycloConfig* CycloConfig::get() {
	if (!instance) {
		instance = new CycloConfig();
	}
}

void CycloConfig::setFrameSize(cv::Size size) {
	this->data.x[0] = size.width;
	this->data.y[1] = size.height;
	this->data.x[2] = size.width;
	this->data.y[3] = size.height;
}

void CycloConfig::setCropArea(cv::Size size) {
	this->data.x_crop[1] = size.width;
	this->data.y_crop[1] = size.height;
}

CycloConfig::CycloConfig() {
	this->data = this->LoadData();
}

CycloConfig::~CycloConfig() {
	this->PersistData(this->data);
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
void CycloConfig::SetCounterPos(unsigned int index, cv::Point pt) {
	this->data.x_counter[index] = pt.x;
	this->data.y_counter[index] = pt.y;
}

cv::Point CycloConfig::GetPerspectivePos(unsigned int index) {
	cv::Point pt;
	pt.x = this->data.x[index];
	pt.y = this->data.y[index];
	return pt;
}
void CycloConfig::SetPerspectivePos(unsigned int index, cv::Point pt) {
	this->data.x[index] = pt.x;
	this->data.y[index] = pt.y;
}

cv::Point CycloConfig::GetCropPos(unsigned int index) {
	cv::Point pt;
	pt.x = this->data.x_crop[index];
	pt.y = this->data.y_crop[index];
	return pt;
}
void CycloConfig::SetCropPos(unsigned int index, cv::Point pt) {
	this->data.x_crop[index] = pt.x;
	this->data.y_crop[index] = pt.y;
}

cv::Point CycloConfig::GetInterestPos(unsigned int index) {
	cv::Point pt;
	pt.x = this->data.x_interest[index];
	pt.y = this->data.y_interest[index];
	return pt;
}

void CycloConfig::SetInterestPos(unsigned int index, cv::Point pt) {
	this->data.x_interest[index] = pt.x;
	this->data.y_interest[index] = pt.y;
}

void CycloConfig::PersistData(ConfigData &config) {
	PersistData(data);
}

void CycloConfig::PersistData(ConfigData &config) {
	int fd = open(this->configFile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd >= 0) {
		write(fd, &config, sizeof(config));
		close(fd);
	}
}

ConfigData CycloConfig::LoadData() {
	ConfigData data;
	memset(&data, 0, sizeof(ConfigData));
	int fd = open(this->configFile, O_RDONLY);
	if (fd >= 0) {
		read(fd, &data, sizeof(data));
		close(fd);
	}
	return data;
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
