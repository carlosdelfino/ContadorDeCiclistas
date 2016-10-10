/*
 * CycloConfig.h
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#ifndef CYCLOCONFIG_HPP_
#define CYCLOCONFIG_HPP_

#include<opencv2/opencv.hpp>

#include <string>
#include <exception>
#include <iostream>
#include <getopt.h>

#define PROP_FULL_FRAME_WIDTH "frame.width"
#define PROP_FULL_FRAME_HEIGHT "frame.height"

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

#define PROP_ADDRESS "address"

#define POINT_LEFT_SUPERIOR_COORNER_X 0
#define POINT_LEFT_SUPERIOR_COORNER_Y 0
#define POINT_RIGHT_SUPERIOR_COORNER_X 640
#define POINT_RIGHT_SUPERIOR_COORNER_Y 0
#define POINT_LEFT_INFERIOR_COORNER_X 0
#define POINT_LEFT_INFERIOR_COORNER_Y 480
#define POINT_RIGHT_INFERIOR_COORNER_X 640
#define POINT_RIGHT_INFERIOR_COORNER_Y 480

typedef struct {

	unsigned int width = 640;
	unsigned int height = 480;

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

enum FileType
	: uint8_t {
		DEV_FILE = 0x02, REG_FILE = 0x01, UNK_FILE = 0x00
};

class CycloConfig {

private:

	struct option long_options[9] = 								//..
			{			                  								//..
			{ "reg_source", required_argument, nullptr, 's' }, 			//..
					{ "override", required_argument, nullptr, 'O' },   	//..
					{ "dev_source", required_argument, nullptr, 'D' }, 	//..
					{ "record", required_argument, nullptr, 'r' },     	//..
					{ "stream", required_argument, nullptr, 'S' },     	//..
					{ "address", required_argument, nullptr, 'a' },    	//..
					{ "sensor", required_argument, nullptr, 't' }, 		//..
					{ "help", no_argument, nullptr, 'h' }, 				//..
					{ nullptr, 0, nullptr, 0 } 							//..
			};

	int opt = 0;
	int long_index = 0;

	std::string source_file = ""; //where data comes from if source is a
//regular file.
	int source_device = -1; //where data comes from if source is
//device file.
	std::string record_file = ""; //output file.
	std::string stream_device = ""; //device file used to stream data.
	std::string sensor_device = ""; //sensor device file

	FileType source = UNK_FILE;
	const char *configFile = "CycloTracker.conf";
	ConfigData data;
	unsigned long interaction = 0;
	bool reconfigureFlag = false;
	CycloConfig();

public:
	/* @brief Desabilita Construtor de Cópia gerado pelo compilador
	 *
	 */
	CycloConfig(const CycloConfig &) = delete;

	/* @brief Desabilita Operador de Atribuição gerado pelo compilador
	 *
	 */
	CycloConfig &operator=(const CycloConfig &) = delete;

	virtual ~CycloConfig();

	void PersistData();
	void LoadData();

	void parseCommandOptions(int argc, char * const *argv);

	std::string getOutputDeviceName();
	std::string getSensorDeviceName();
	int getSourceDeviceId();
	std::string getSourceFileName();
	std::string getRecordFileName();
	bool hasSensorDeviceName();
	bool hasOutputDeviceName();
	bool hasRecordFileName();
	FileType getSourceType();

	/** @brief cria uma unica instância pra uso em todo o programa (Singelton)
	 *
	 * Uma unica instância da classe CycloConfig deve existir para toda a
	 * aplicação assim evita-se bugs e redundância de controle das
	 * configurações mantendo o sistema padronizado na forma de
	 * armazena-la.
	 *
	 * Para utilizar a classe siga o código abaixo.
	 * Observe que é obrigatório receber o objeto por referência evitando
	 * sua duplicação. A tentativa de dúplica-lo irá acarretar erro na compilação.
	 * @code
	 * CloseConfig &config = CycloConfig::get();
	 * @endcode
	 */
	static CycloConfig& get();

	/** @brief Seta o tamanho do frame principal, e mantem referência para criação dos demais  frames.

	 Esta função deve ser usada para armazenar a configuração do
	 frameprincipal, inicialmente obitido com a capitura do primeiro
	 frame de video no dispositivo de entrada.

	 Quando ativado a opção de reconfiguração durante a execução do
	 programa, esta função tambem dimenciona a area de corte e area de
	 interesse para que fiquem no mesmo tamanho do frame principal.
	 @code
	 @endcode
	 @see CycloConfig::setCropArea()
	 @see CycloConfig::SetInterestArea()
	 */
	void setFullFrameSize(cv::Size size);
	void setFullFrameWidth(unsigned int w);
	void setFullFrameHeight(unsigned int w);
	unsigned int getFullFrameWidth();
	unsigned int getFullFrameHeight();
	cv::Size getFullFrameSize();

	unsigned int getY(int i);
	unsigned int getX(int i);
	void setX(unsigned int i, int x);
	void setY(unsigned int i, int y);

	unsigned int getCounterX(int i);
	unsigned int getCounterY(int i);
	void setCounterX(int i, int x);
	void setCounterY(int i, int y);

	void clearInteresstArea();
	cv::Point GetInterestPos(unsigned int index);
	cv::Rect getInterestArea();
	unsigned int getInterestX(int i);
	unsigned int getInterestY(int i);
	unsigned int getInterestWidth();
	unsigned int getInterestHeight();
	void setInterestX(int i, int x);
	void setInterestY(int i, int y);
	void setInterestArea(cv::Size size);
	void SetInterestPos(unsigned int index, cv::Point pt);
	void SetInterestPos(unsigned int index, int x, int y);

	void clearCropArea();
	cv::Rect getCropArea();
	cv::Point GetCropPos(unsigned int index);
	unsigned int getCropX(int i);
	unsigned int getCropY(int i);
	unsigned int getCropWidth();
	unsigned int getCropHeight();
	unsigned int getCropWindowPosX();
	unsigned int getCropWindowPosY();
	void setCropArea(cv::Size size);
	void setCropX(unsigned int i, int x);
	void setCropY(unsigned int i, int y);
	void setCropWindowX(unsigned int x);
	void setCropWindowY(unsigned int y);
	void SetCropPos(unsigned int index, cv::Point pt);
	void SetCropPos(unsigned int index, int x, int y);

	unsigned int GetLeftCounter();
	unsigned int GetRightCounter();
	void SetLeftCounter(unsigned int counter);
	void SetRightCounter(unsigned int counter);

	cv::Point GetCounterPos(unsigned int index);
	void SetCounterPos(unsigned int index, cv::Point pt);
	void SetCounterPos(unsigned int index, int x, int y);

	void clearPerspectiveArea();
	cv::Point GetPerspectivePos(unsigned int index);
	void SetPerspectivePos(unsigned int index, cv::Point pt);
	void SetPerspectivePos(unsigned int index, int x, int y);
	void setPerspectiveArea(cv::Size size);

	std::string getAddress();
	void setAddress(std::string a);

	void setReconfigure(bool b);
	bool reconfigure();

	unsigned long countInteraction();
};

#endif /* CYCLOCONFIG_HPP_ */
