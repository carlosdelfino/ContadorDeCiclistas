/*
 * CycloConfig.h
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#ifndef CYCLOCONFIG_HPP_
#define CYCLOCONFIG_HPP_

#include<opencv2/opencv.hpp>

#define PROP_COUNTER_LEFT  "counter.left"
#define PROP_COUNTER_RIGHT  "counter.right"
#define PROP_COUNTER_LEFT_X  "counter.left_x"
#define PROP_COUNTER_RIGHT_X  "counter.right_x"
#define PROP_COUNTER_LEFT_Y  "counter.left_y"
#define PROP_COUNTER_RIGHT_Y  "counter.right_y"

#define PROP_CROP_WINDOW_X  "crop.window.dist_x"
#define PROP_CROP_WINDOW_Y  "crop.window.dist_y"
#define PROP_CROP_LEFT_X  "crop.left_x"
#define PROP_CROP_LEFT_Y  "crop.left_y"
#define PROP_CROP_RIGHT_X  "crop.right_x"
#define PROP_CROP_RIGHT_Y  "crop.right_y"

#define PROP_PERSPECTIVE_SUPERIOR_LEFT_X "perspective.superior.left_x"
#define PROP_PERSPECTIVE_SUPERIOR_LEFT_Y "perspective.superior.left_y"
#define PROP_PERSPECTIVE_SUPERIOR_RIGHT_X "perspective.superior.right_x"
#define PROP_PERSPECTIVE_SUPERIOR_RIGHT_Y "perspective.superior.right_y"
#define PROP_PERSPECTIVE_INFERIOR_LEFT_X "perspective.inferior.left_x"
#define PROP_PERSPECTIVE_INFERIOR_LEFT_Y "perspective.inferior.left_y"
#define PROP_PERSPECTIVE_INFERIOR_RIGHT_X "perspective.inferior.right_x"
#define PROP_PERSPECTIVE_INFERIOR_RIGHT_Y "perspective.inferior.right_y"

#define POINT_LEFT_SUPERIOR_COORNER_X 0
#define POINT_LEFT_SUPERIOR_COORNER_Y 0
#define POINT_RIGHT_SUPERIOR_COORNER_X 640
#define POINT_RIGHT_SUPERIOR_COORNER_Y 0
#define POINT_LEFT_INFERIOR_COORNER_X 0
#define POINT_LEFT_INFERIOR_COORNER_Y 480
#define POINT_RIGHT_INFERIOR_COORNER_X 640
#define POINT_RIGHT_INFERIOR_COORNER_Y 480

typedef struct {
	unsigned int left_counter = 0;
	unsigned int right_counter = 0;

	unsigned int x_counter[2] = { 50, 500 };
	unsigned int y_counter[2] = { 350, 350 };

	unsigned int x[4] = { 0, 640, 0, 640 };
	unsigned int y[4] = { 0, 0, 480, 480 };


	unsigned int x_crop_windowPos = 10;
	unsigned int y_crop_windowPos = 60;

	unsigned int x_crop[2] = { 0, 640 };
	unsigned int y_crop[2] = { 0, 480 };

	unsigned int x_interest[2] = { 0, 640 };
	unsigned int y_interest[2] = { 0, 480 };

	std::string address = "Av. Carapinina, 1000";

} ConfigData;

class CycloConfig {

private:
	const char *configFile = "CycloTracker.conf";
	ConfigData data;
	unsigned long interaction = 0;
	CycloConfig();

public:
	//Desabilita Construtor de Cópia gerado pelo compilador
	CycloConfig(const CycloConfig &) = delete;

	//Desabilita Operador de Atribuição gerado pelo compilador
	CycloConfig &operator=(const CycloConfig &) = delete;

	virtual ~CycloConfig();

	void PersistData();
	void LoadData();

	static CycloConfig& get();

	void setFrameSize(cv::Size size);
	void setCropArea(cv::Size size);

	void setX(unsigned int i, int x);
	void setY(unsigned int i, int y);

	void setCropX(unsigned int i, int x);

	void setCropY(unsigned int i, int y);

	unsigned int getX(int i);

	unsigned int getCounterX(int i);

	void setCounterX(int i, int x);

	void setCounterY(int i, int y);

	unsigned int getCropX(int i);

	unsigned int getInterestX(int i);

	void setInterestX(int i, int x);

	unsigned int getY(int i);

	unsigned int getCounterY(int i);

	unsigned int getCropY(int i);

	unsigned int getInterestY(int i);

	void setInterestY(int i, int y);

	unsigned int getInterestWidth();

	unsigned int getInterestHeight();

	unsigned int getCropWidth();
	unsigned int getCropHeight();

	std::string getAddress();

	void setAddress(std::string a);

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

	unsigned int getCropWindowPosX();
	unsigned int getCropWindowPosY();
	unsigned long countInteraction();
};

#endif /* CYCLOCONFIG_HPP_ */
