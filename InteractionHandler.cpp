/*
 * InteractionHandler.cpp
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#include "InteractionHandler.hpp"

#include<boost/format.hpp>

#include<opencv2/opencv.hpp>

#include <string>

#include "CycloConfig.hpp"
#include "CycloTracker.hpp"

InteractionHandler::InteractionHandler(std::string windowName) {
	this->windowName = windowName;
	std::cout << "** Criando Gerenciador de Janela " << std::endl;
	SetAction(InteractionAction::START);
}

InteractionHandler::~InteractionHandler() {
}

void InteractionHandler::GetInterestArea(int x, int y) {
	static int step = 0;
	static int x_internal[2];
	static int y_internal[2];
	x_internal[step] = x;
	y_internal[step] = y;
	switch (step) {
	case 0:
		std::cout
				<< "Selecione agora o limite da area de contagem a direita inferior"
				<< std::endl;
		break;
	case 1:
		config.SetInterestPos(0, x_internal[0], y_internal[0]);
		config.SetInterestPos(1, x_internal[1], y_internal[1]);
		std::cout
				<< boost::format(
						"Pronto os limites de area de interesse são: (%d,%d x %d,%d)")
						% config.getInterestX(0) % config.getInterestY(0)
						% config.getInterestX(1) % config.getInterestY(1)
				<< std::endl << std::endl;
		step = 0;
		nextCallBack();
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
		std::cout << "## Pronto Finzalizado a parametrização do sistema"
				<< std::endl << std::endl
				<< "## Você pode continuar usando o software," << std::endl
				<< "## ou se prefire tecle [ESC]" << std::endl
				<< "## e rode quando quiser sem a opção -O" << std::endl
				<< std::endl;
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
	std::cout << "currentCallBack" << std::endl;
	(this->*CurrentCallbackFunction)(x, y);
}

void InteractionHandler::nextCallBack() {
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
	nextCallBack();
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
		config.SetCounterPos(0, x_internal[0], y_internal[0]);
		config.SetCounterPos(1, x_internal[1], y_internal[1]);
		std::cout << "Pronto, você posicionou os contadores:" << std::endl;
		std::cout
				<< boost::format("primeiro contador (%d,%d)")
						% config.getCounterX(0) % config.getCounterY(0)
				<< std::endl
				<< boost::format("segundo contador (%d,%d)")
						% config.GetCounterPos(1) % config.getCounterY(1)
				<< std::endl << std::endl;
		step = 0;
		nextCallBack();
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
		std::cout << "Selecione agora o canto direito superior da pespectiva"
				<< std::endl;
		break;
	case 1:
		std::cout << "Selecione agora o canto esquerdo inferior da pespectiva"
				<< std::endl;
		break;
	case 2:
		std::cout << "Selecione agora o canto direito inferior da pespectiva"
				<< std::endl;
		break;
	case 3:
		config.setX(0, x_internal[0]);
		config.setY(0, y_internal[0]);
		config.setX(1, x_internal[1]);
		config.setY(1, y_internal[1]);
		config.setX(2, x_internal[2]);
		config.setY(2, y_internal[2]);
		config.setX(3, x_internal[3]);
		config.setY(3, y_internal[3]);
		std::cout << "Pronto nossa perspectiva de visão está defindia"
				<< std::endl << std::endl;
		nextCallBack();
		step = 0;
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
		config.SetCropPos(0, x_internal[0], y_internal[0]);
		config.SetCropPos(1, x_internal[1], y_internal[1]);
		std::cout
				<< boost::format(
						"Pronto os limites de processamento de interesse são: (%d,%d x %d,%d)")
						% config.getCropX(0) % config.getCropY(0)
						% config.getCropX(1) % config.getCropY(1) << std::endl
				<< std::endl;
		nextCallBack();
		step = 0;
		return;
	default:
		printf("Error: GetCropFrame: step = %d\n", step);
		exit(EXIT_FAILURE);
		break;
	}
	step++;
}
