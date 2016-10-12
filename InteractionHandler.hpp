/*
 * InteractionHandler.h
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#ifndef INTERACTIONHANDLER_HPP_
#define INTERACTIONHANDLER_HPP_
#include "CycloConfig.hpp"
#include <string>

enum InteractionAction
	: uint8_t {
		START,
	SET_COUNTERS_AREA,
	SET_PERSPECTIVE_AREA,
	SET_CROP_AREA,
	SET_INTEREST_AREA,
	SAVE_CONFIG,
	NONE,
	ERROR
};

void MouseCallback(int event, int x, int y,
		int __attribute__((__unused__)) flags,
		void __attribute__((__unused__)) *data);

class InteractionHandler {

public:
	InteractionHandler(std::string windowName);
	~InteractionHandler();

	void SetAction(InteractionAction action);

	void callCurrentCallBack(int x, int y);

	void nextCallBack(void);

	bool hasCurrentCallbackFunction();

private:
	CycloConfig *config = CycloConfig::get();
	int globalStep = 0;
	void (InteractionHandler::*CurrentCallbackFunction)(int x, int y);

	std::string windowName;

	InteractionHandler();

	void setWindowName(std::string windowName) {
		this->windowName = windowName;
	}

	void saveConfig(int x, int y);

	void start(int x, int y);

	void GetCountersArea(int x, int y);

	void GetPerspectivePoints(int x, int y);

	void GetCropFrame(int x, int y);

	void GetInterestArea(int x, int y);

};

#endif /* INTERACTIONHANDLER_HPP_ */
