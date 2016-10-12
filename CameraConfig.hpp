/*
 * CycloConfig.h
 *
 *  Created on: 5 de out de 2016
 *      Author: consu_000
 */

#ifndef CAMERACONFIG_HPP_
#define CAMERACONFIG_HPP_

#include<opencv2/opencv.hpp>

#include <string>
#include <exception>
#include <iostream>


#define PROP_CAMERA_COS_THETA		"camera.theta.cos"
#define PROP_CAMERA_SIN_THETA		"camera.theta.sin"
#define PROP_CAMERA_THETA			"camera.theta.value"
#define PROP_CAMERA_FRAME_ROWS		"camera.frame.row"
#define PROP_CAMERA_FRAME_COLS		"camera.frame.cols"
#define PROP_CAMERA_DISTANCE_FOCAL "camera.distance.focal"
#define PROP_CAMERA_DISTANCE_RZ 	"camera.distance.Rz"
#define PROP_CAMERA_HEIGHT 			"camera.height"

typedef struct {
	int frameRows;
	int frameCols;
	float theta;
	float cosTheta;
	float sinTheta;
	float focalDistance;
	float height;
	float Rz; //distance from its projection on the ground(xz plane) to
			  //ciclyst
} CameraData;

class CameraConfig {

private:

	std::string configFileName;

	CameraData data;

public:

	CameraConfig(std::string  configFile);
	/* @brief Desabilita Construtor de Cópia gerado pelo compilador
	 *
	 */
	CameraConfig(const CameraConfig &) = delete;

	/* @brief Desabilita Operador de Atribuição gerado pelo compilador
	 *
	 */
	CameraConfig &operator=(const CameraConfig &) = delete;

	virtual ~CameraConfig();

	void PersistData( );
	void PersistData(std::string );
	void LoadData();

	int getFrameRows();
	int getFrameCols();
	float getTheta();
	float getCosTheta();
	float getSinTheta();
	float getFocalDistance();
	float getHeight();
	float getRz();

	void setFrameRows(int);
	void setFrameCols(int);
	void setTheta(float);
	void setCosTheta(float);
	void setSinTheta(float);
	void setFocalDistance(float);
	void setHeight(float);
	void setRz(float);
};

#endif /* CYCLOCONFIG_HPP_ */
