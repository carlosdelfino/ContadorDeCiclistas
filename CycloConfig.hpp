/*
 * CycloConfig.h
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#ifndef CYCLOCONFIG_HPP_
#define CYCLOCONFIG_HPP_

#include<opencv2/opencv.hpp>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

typedef struct {
	unsigned int left_counter;
	unsigned int right_counter;

	int x_counter[2];
	int y_counter[2];

	int x[4];
	int y[4];

	int x_crop[2];
	int y_crop[2];

	int x_interest[2];
	int y_interest[2];

} ConfigData;

class CycloConfig {
private:
	const char *configFile = ".data";
	ConfigData data;

	static CycloConfig* instance;
	CycloConfig();
	CycloConfig(CycloConfig const&);              // Don't Implement
	void operator=(CycloConfig const&); // Don't implement

public:
	virtual ~CycloConfig();

	static CycloConfig* get();

	void setFrameSize(cv::Size size);
	void setCropArea(cv::Size size);

	const unsigned int getX(int i) const {
		return data.x[i];
	}

	const unsigned int getCounterX(int i) const {
		return data.x_counter[i];
	}

	const unsigned int getCropX(int i) const {
		return data.x_crop[i];
	}

	const unsigned int getInterestX(int i) const {
		return data.x_interest[i];
	}

	const unsigned int getY(int i) const {
		return data.y[i];
	}

	const unsigned int getCounterY(int i) const {
		return data.y_counter[i];
	}

	const unsigned int getCropY(int i) const {
		return data.y_crop[i];
	}

	const unsigned int getInterestY(int i) const {
		return data.y_interest[i];
	}

	const unsigned int getInterestWidth() const {
		return getInterestX(1) - getInterestX(0);
	}

	const unsigned int getInterestHeight() const {
		return getInterestY(1) - getInterestY(0);
	}

	const unsigned int getCropWidth() const {
		return getCropX(1) - getCropX(0);
	}
	const unsigned int getCropHeight() const {
		return getCropY(1) - getCropY(0);
	}

	unsigned int GetLeftCounter();
	void SetLeftCounter(unsigned int counter);
	unsigned int GetRightCounter();
	void SetRightCounter(unsigned int counter);

	cv::Point GetCounterPos(unsigned int index);
	void SetCounterPos(unsigned int index, cv::Point pt);
	void SetCounterPos(unsigned int index, int x, int y);

	cv::Point GetPerspectivePos(unsigned int index);
	void SetPerspectivePos(unsigned int index, cv::Point pt);
	void SetPerspectivePos(unsigned int index, int x, int y);

	cv::Point GetCropPos(unsigned int index);
	void SetCropPos(unsigned int index, cv::Point pt);
	void SetCropPos(unsigned int index, int x, int y);

	cv::Point GetInterestPos(unsigned int index);
	void SetInterestPos(unsigned int index, cv::Point pt);
	void SetInterestPos(unsigned int index, int x, int y);

protected:
	void PersistData(ConfigData &config);
	void PersistData();
	ConfigData LoadData();
};

#endif /* CYCLOCONFIG_HPP_ */
