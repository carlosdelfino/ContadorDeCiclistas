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

#define CROP_MAX_WINDOW_PERCENT (410/640)

#define DEFAULT_CAPTURE_FRAME_DELAY 30
#define MAX_CAPTURE_FRAME_DELAY INT_MAX-1

#define PROP_FULL_FRAME_WIDTH  "frame.width"
#define PROP_FULL_FRAME_HEIGHT "frame.height"

#define PROP_DETECTEDOBJ_COUNTER_1        "counter.track.1.counter"
#define PROP_DETECTEDOBJ_COUNTER_2        "counter.track.2.counter"
#define PROP_DETECTEDOBJ_COUNTER_1_X      "counter.track.1.x"
#define PROP_DETECTEDOBJ_COUNTER_2_X      "counter.track.2.x"
#define PROP_DETECTEDOBJ_COUNTER_1_Y      "counter.track.1.y"
#define PROP_DETECTEDOBJ_COUNTER_2_Y      "counter.track.2.y"

#define PROP_COUNTER_DETECTEDOBJ_TOTAL          "counter.track.total"
#define PROP_COUNTER_DETECTEDOBJ_FILE_PATH      "counter.file.basepath"
#define PROP_COUNTER_DETECTEDOBJ_FILE_PREFIX    "counter.file.prefix"
#define PROP_COUNTER_DETECTEDOBJ_FILE_EXTENSION "counter.file.extension"

#define PROP_CROP_WINDOW_X  "crop.window.dist_x"
#define PROP_CROP_WINDOW_Y  "crop.window.dist_y"
#define PROP_CROP_LEFT_X    "crop.left_x"
#define PROP_CROP_LEFT_Y    "crop.left_y"
#define PROP_CROP_RIGHT_X   "crop.right_x"
#define PROP_CROP_RIGHT_Y   "crop.right_y"

#define PROP_PERSPECTIVE_SUPERIOR_LEFT_X  "perspective.superior.left_x"
#define PROP_PERSPECTIVE_SUPERIOR_LEFT_Y  "perspective.superior.left_y"
#define PROP_PERSPECTIVE_SUPERIOR_RIGHT_X "perspective.superior.right_x"
#define PROP_PERSPECTIVE_SUPERIOR_RIGHT_Y "perspective.superior.right_y"
#define PROP_PERSPECTIVE_INFERIOR_LEFT_X  "perspective.inferior.left_x"
#define PROP_PERSPECTIVE_INFERIOR_LEFT_Y  "perspective.inferior.left_y"
#define PROP_PERSPECTIVE_INFERIOR_RIGHT_X "perspective.inferior.right_x"
#define PROP_PERSPECTIVE_INFERIOR_RIGHT_Y "perspective.inferior.right_y"

#define PROP_LOGO_PRINCIPAL "images.logos.principal"
#define PROP_LOGO_SECOND    "images.logos.second"

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

	unsigned int x_counter[2] = { 50, 50 };
	unsigned int y_counter[2] = { 150, 200 };

	unsigned int x[4] = { 0, 640, 0, 640 };
	unsigned int y[4] = { 0, 0, 480, 480 };

	unsigned int x_crop_windowPos = 10;
	unsigned int y_crop_windowPos = 60;

	unsigned int cropMaxWindowWidth = 350;//640 * CROP_MAX_WINDOW_PERCENT;

	unsigned int cropMaxWindowWeight = 300;//480 * CROP_MAX_WINDOW_PERCENT;

	unsigned int x_crop[2] = { 0, 640 };
	unsigned int y_crop[2] = { 0, 480 };

	unsigned int x_interest[2] = { 0, 640 };
	unsigned int y_interest[2] = { 0, 480 };

	std::string pathImageLogo = "images/logo.jpg";
	std::string pathImageCyclist = "images/cyclist.jpg";
	std::string address = "Av. Carapinina, 1000";

	unsigned int counterDetectedObject = 0;

	std::string filePahtDetectedObject = "./images/";
	std::string filePrefixDetectedObject = "Bike-";
	std::string fileExtensionDetectedObject = "jpg";

	unsigned int distanceThreshold = 30;
	double contourThreshold = 50;

	unsigned int captureFrameDelay = std::min(DEFAULT_CAPTURE_FRAME_DELAY,MAX_CAPTURE_FRAME_DELAY);

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
	std::string configFileName = "CycloTracker.conf";
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

	void PersistData(std::string  fileName);
	void PersistData();
	void LoadData();

	void parseCommandOptions(int argc, char * const *argv);

	std::string getOutputDeviceName();
	std::string getSensorDeviceName();
	std::string getSourceFileName();
	std::string getRecordFileName();
	int getSourceDeviceId();
	bool hasSensorDeviceName();
	bool hasOutputDeviceName();
	bool hasRecordFileName();

	FileType getSourceType();

	/** @brief cria uma única instância pra uso em todo o programa (Singelton)
	 *
	 * Uma única instância da classe CycloConfig deve existir para toda a
	 * aplicação assim evita-se bugs e redundância de controle das
	 * configurações mantendo o sistema padronizado na forma de
	 * armazena-la.
	 *
	 * Para utilizar a classe siga o código abaixo.
	 * Observe que é obrigatório receber o objeto como ponteiro evitando
	 * sua duplicação. A tentativa de dúplica-lo irá acarretar erro na compilação.
	 * @code
	 * CloseConfig *config = CycloConfig::get();
	 * @endcode
	 */
	static CycloConfig *get();

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
	unsigned int getCropWindowMaxWidth();
	unsigned int getCropWindowMaxHeight();

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

	std::string getPathPrincipalLogo();
	std::string getPathSecondLogo();
	void setPathImageLogo(std::string);
	void setPathImageCyclist(std::string);

	unsigned int getCounterDetectedObject();
	void setCounterDetectedObject(unsigned int);

	void setFilePahtDetectedObject(std::string);
	void setFilePrefixDetectedObject(std::string);
	void setFileExtensionDetectedObject(std::string);

	std::string getFilePahtDetectedObject();
	std::string getFilePrefixDetectedObject();
	std::string getFileExtensionDetectedObject();

	unsigned int getDistanceThreshold();
	double getContourThreshold();
	void setDistanceThreshold(unsigned int);
	void setContourThreshold(double);

	void setReconfigure(bool b);
	bool reconfigure();

	unsigned long countInteraction();

	unsigned int getCaptureFrameDelay();
	void setCaptureFrameDelay(unsigned int );
};

#endif /* CYCLOCONFIG_HPP_ */
