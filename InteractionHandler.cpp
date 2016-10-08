/*
 * InteractionHandler.cpp
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#include "InteractionHandler.hpp"

#include "CycloConfig.hpp"
#include<opencv2/opencv.hpp>
#include <string>

InteractionHandler::InteractionHandler(std::string windowName) {
	this->windowName = windowName;

	SetAction(InteractionAction::START);
}

InteractionHandler::~InteractionHandler() {}

void InteractionHandler::GetInterestArea(int x, int y) {
	static int step = 0;
	static int x_internal[2];
	static int y_internal[2];
	x_internal[step] = x;
	y_internal[step] = y;
	switch (step) {
	case 0:
		printf(
				"Selecione agora o limite da area de contagem a direita inferior\n");
		break;
	case 1:
		step = 0;
		config.SetInterestPos(0, x_internal[0], y_internal[0]);
		config.SetInterestPos(1, x_internal[1], y_internal[1]);

		printf(
				"Pronto os limites de area de interesse são: (%d,%d x %d,%d)\n\n",
				x_internal[0], y_internal[0], x_internal[1], y_internal[1]);
		return;
	default:
		printf("Error: GetInterestArea: step = %d\n", step);
		exit(EXIT_FAILURE);
		break;
	}
	step++;
}

void InteractionHandler::SetAction(InteractionAction action) {
	switch (action) {
	case START:
		CurrentCallbackFunction = &InteractionHandler::start;
		break;
	case SET_COUNTERS_AREA:
		printf("Selecione a posição de cada contador!\n");
		printf("Clique onde deseja colocar o primeiro contador.\n");
		CurrentCallbackFunction = &InteractionHandler::GetCountersArea;
		break;
	case SET_PERSPECTIVE_AREA:
		printf("Defina a nossa perspectiva da cena de contagem.\n");
		printf("Selecione agora o canto esquerdo superior da pespectiva\n");
		CurrentCallbackFunction = &InteractionHandler::GetPerspectivePoints;
		break;
	case SET_CROP_AREA:
		printf("Defina a area de processamento da imagem.\n");
		printf(
				"Procure definir uma area clara e onde apenas passe os objetos que serão contado,\n");
		printf("esta area deve ser maior que a area de contagem, ");
		printf("principalmente nos pontos de entrada e saída!\n");
		printf("Selecione o canto esquerdo superior, clicando nele.\n");
		CurrentCallbackFunction = &InteractionHandler::GetCropFrame;
		break;
	case SET_INTEREST_AREA:
		printf("Defina a area de interesse, onde será contados os objetos.\n");
		printf("Esta área deve estar dentro da área da perpectiva.\n");
		printf(
				"ATENÇÃO: esta area deve ser lijeiramente maior que o maior objeto a ser contado.\n");
		CurrentCallbackFunction = &InteractionHandler::GetInterestArea;
		break;
	case NONE:
		CurrentCallbackFunction = nullptr;
		break;
	default:
		printf("Error: SetAction: action = %d\n", action);
		exit(EXIT_FAILURE);
		break;
	}
}

bool InteractionHandler::hasCurrentCallbackFunction() {
	return CurrentCallbackFunction != nullptr;
}

void InteractionHandler::callCurrentCallBack(int x, int y) {
	(this->*CurrentCallbackFunction)(x, y);
}

void InteractionHandler::nextStep() {
	globalStep++;
	if (globalStep >= InteractionAction::ERROR) {
		printf(
				"Erro na contagem de passos gerais para parametrização, passo %d\n",
				globalStep);
		exit(255);
	}
	SetAction(static_cast<InteractionAction>(globalStep));

}

void InteractionHandler::start(int x __attribute__((unused)),
		int y __attribute__((unused))) {

	printf(
			"\n\nClique na janela %s para começar a marcação.\n ATENÇÃO: Este clique é apenas para começar as marcações.\n\n",
			InteractionHandler::windowName.c_str());
}

void InteractionHandler::GetCountersArea(int x, int y) {
	static int step = 0;
	static int x_internal[2];
	static int y_internal[2];
	x_internal[step] = x;
	y_internal[step] = y;
	switch (step) {
	case 0:
		printf("Agora, clique para posicionar o seguundo contador");
		break;
	case 1:
		step = 0;
		config.SetCounterPos(0, x_internal[0], y_internal[0]);
		config.SetCounterPos(1, x_internal[1], y_internal[1]);
		printf("Pronto, você posicionou os contadores:\n");
		printf("primeiro contador (%d,%d)\n segundo contador (%d,%d)\n\n",
				x_internal[0], y_internal[0], x_internal[1], y_internal[1]);
		return;
	default:
		printf("Error: GetCountersArea: step = %d\n", step);
		exit(EXIT_FAILURE);
		break;
	}
	step++;
}

void InteractionHandler::GetPerspectivePoints(int x, int y) {
	static int step = 0;
	static int x_internal[4];
	static int y_internal[4];
	x_internal[step] = x;
	y_internal[step] = y;
	switch (step) {
	case 0:
		printf("Selecione agora o canto direito superior da pespectiva");
		break;
	case 1:
		printf("Selecione agora o canto esquerdo inferior da pespectiva");
		break;
	case 2:
		printf("Selecione agora o canto direito inferior da pespectiva");
		break;
	case 3:
		step = 0;
		config.setX(0, x_internal[0]);
		config.setY(0, y_internal[0]);
		config.setX(1, x_internal[1]);
		config.setY(1, y_internal[1]);
		config.setX(2, x_internal[2]);
		config.setY(2, y_internal[2]);
		config.setX(3, x_internal[3]);
		config.setY(3, y_internal[3]);
		printf("Pronto nossa perspectiva de visão está defindia\n\n");
		return;
	default:
		printf("Erro GetPerspectivePoints!! step == %d\n", step);
		exit(EXIT_FAILURE);
		break;
	}
	step++;
}

void InteractionHandler::GetCropFrame(int x, int y) {
	static int step = 0;
	static int x_internal[2];
	static int y_internal[2];
	x_internal[step] = x;
	y_internal[step] = y;
	switch (step) {
	case 0:
		printf("Selecione agora o canto inferior direito\n");
		break;
	case 1:
		step = 0;
		step = 0;
		config.SetCropPos(0, x_internal[0], y_internal[0]);
		config.SetCropPos(1, x_internal[1], y_internal[1]);

		printf(
				"Pronto os limites de processamento de interesse são: (%d,%d x %d,%d)\n\n",
				x_internal[0], y_internal[0], x_internal[1], y_internal[1]);

		return;

	default:
		printf("Error: GetCropFrame: step = %d\n", step);
		exit(EXIT_FAILURE);
		break;
	}
	step++;
}
