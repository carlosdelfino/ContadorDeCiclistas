#include<opencv2/opencv.hpp>

#include<boost/format.hpp>

#include<iostream>

#include "CycloTracker.hpp"
#include "CycloConfig.hpp"
#include "InteractionHandler.hpp"
#include "VideoOutput.hpp"
#include "Sensors.hpp"
#include "Utils.hpp"

unsigned long debug = 1;
static CycloConfig *config = nullptr;

inline void MouseCallback(int event, int x, int y,
		int __attribute__((__unused__)) flags,
		void __attribute__((__unused__)) *data) {
	if (cv::EVENT_LBUTTONDOWN == event) {
		CycloConfig *config = CycloConfig::get();
		static InteractionHandler ih(config->getAddress());

		if (ih.hasCurrentCallbackFunction()) {
			if (debug)
				std::cout << "** Callback: Mouse";
			ih.callCurrentCallBack(x, y);
		}
	} else if (cv::EVENT_MOUSEMOVE == event) {
		;
	}

}
int main(int argc, char **argv) {

	std::cout << "*****************************************************"
			<< std::endl;
	std::cout << "** Obtendo configurações                           **"
			<< std::endl;

	config = CycloConfig::get();

	std::cout << "** configurações carregadas                        **"
			<< std::endl;
	std::cout << "** Verificando Opções da Linha de comando          **"
			<< std::endl;

	config->parseCommandOptions(argc, argv);

	std::cout << "** Linha de comando processada                      **"
			<< std::endl;

	SensorData *sd = nullptr;
	std::thread *sensorsThread = nullptr;
	if (config->hasSensorDeviceName()) {

		sensorsThread = StartSensorsThread(
				config->getSensorDeviceName().c_str(), sd);
		std::cout << "** Thread de Sensores ativada.                        **"
				<< std::endl;
	}

	CycloTracker tracker(config);

	cv::VideoCapture *cap = tracker.openVideoCapture();

	std::cout << "** Obtendo o primeiro frame, para dimensionamento de janela"
			<< std::endl;
	std::cout << "** Criando Janela Principal." << std::endl;

	// Obtemos o primeiro Frame para calculos de tamanho da janela
	cv::Mat frame;
	*cap >> frame;

	cv::imshow(config->getAddress(), frame);

	cv::Size frame_size = frame.size();
	std::cout
			<< boost::format(
					"** Dimensionando janela, largura: %d, comprimento: %d")
					% frame_size.height % frame_size.width << std::endl;
	// seta o tamanho do frame nas configurações com base no primeiro frame obtido.
	// veja documentação do setFrameSize() para detalhes.
	config->setFullFrameSize(frame_size);

	std::cout
			<< boost::format(
					"** Contador a Esquerda: (%d,%d), a Direita (%d,%d)")
					% config->getCounterX(0) % config->getCounterY(0)
					% config->getCounterX(1) % config->getCounterY(1)
			<< std::endl;

	std::cout
			<< boost::format(
					"** Janela de Interesse: (%d,%d), largura: %d, altura: %d")
					% config->getInterestX(0) % config->getInterestY(0)
					% config->getInterestWidth() % config->getInterestHeight()
			<< std::endl;

	std::cout
			<< boost::format(
					"** Perspectiva atual: Ponto a esquerda: (%d,%d), a Direita (%d,%d)")
					% config->getX(0) % config->getY(0) % config->getX(1)
					% config->getY(1) << std::endl
			<< boost::format(
					"                      Esquerda inferior: (%d,%d), a direita (%d,%d)")
					% config->getX(2) % config->getY(2) % config->getX(3)
					% config->getY(3) << std::endl;
	std::cout
			<< boost::format(
					"** Janela de Corte: (%d,%d), comprimento: %d, altura: %d")
					% config->getCropX(0) % config->getCropY(0)
					% config->getCropWidth() % config->getCropHeight()
			<< std::endl;

	if (config->reconfigure()) {
		std::cout << std::endl
				<< "** Zerando, perspectiva, área de corte e area de interesse "
				<< std::endl;

		config->clearPerspectiveArea();
		config->clearCropArea();
		config->clearInteresstArea();

		std::cout
				<< "** Definindo Callback para obtenção de novas configurações"
				<< std::endl << std::endl
				<< boost::format(
						"@@ Clique na janela %s para começar a marcação.")
						% config->getAddress() << std::endl
				<< "@@@@ ATENÇÃO: Este clique é apenas para começar as marcações."
				<< std::endl << std::endl;
		cv::setMouseCallback(config->getAddress(), MouseCallback, nullptr);
	}

	std::cout << "** Inciando loop de processamento das imagens" << std::endl;

	tracker.openVideoOutput();
	tracker.openVideoWriter();

	tracker.processFrames();

	if (sensorsThread) {
		sensorsThread->join();
	}

	delete config;

	exit(0);
	return 0;
}

