/*
 * InteractionHandler.h
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#ifndef INTERACTIONHANDLER_H_
#define INTERACTIONHANDLER_H_

enum InteractionAction : uint8_t{
	START,
	SET_COUNTERS_AREA,
	SET_PERSPECTIVE_AREA,
	SET_CROP_AREA,
	SET_INTEREST_AREA,
	NONE,
	ERROR
};

class InteractionHandler {


private:
	std::string windowName;
	static void (*CurrentCallbackFunction)(int x, int y);
	CycloConfig* config;
	int globalStep;
public:
	static void Subscribe(std::string windowName);

	static void SetAction(InteractionAction action);

protected:
	static void MouseCallback(int event, int x, int y,
			int __attribute__((__unused__)) flags,
			void __attribute__((__unused__)) *data);

private:
	static void nextStep(void);
	static void start(int x, int y);

	static void GetCountersArea(int x, int y);

	static void GetPerspectivePoints(int x, int y);

	static void GetCropFrame(int x, int y);

	static void GetInterestArea(int x, int y);
};

#endif /* INTERACTIONHANDLER_H_ */
