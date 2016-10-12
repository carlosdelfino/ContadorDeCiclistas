#include <math.h>
#include <fstream>
#include <iostream>

#include "Camera.hpp"
#include "CameraConfig.hpp"
#include "CoordTransform.hpp"

#define PI (4 * atan(tan(1.)))

Camera::Camera(std::string fileName) {
	this->fileName = fileName;
	cameraConfig = new CameraConfig(fileName);
}
Camera::~Camera() {
}

void Camera::SetFrameRows(int rows) {
	cameraConfig->setFrameRows(rows);
}
int Camera::GetFrameRows(void) {
	return cameraConfig->getFrameRows();
}
void Camera::SetFrameCols(int cols) {
	cameraConfig->setFrameCols(cols);
}
int Camera::GetFrameCols(void) {
	return cameraConfig->getFrameCols();
}

void Camera::SetFocalDistance(float d, float h, cv::Point2f P1,
		cv::Point2f P2) {
	cameraConfig->setFocalDistance(d * norm(P1 - P2) / h); //point - point is a vector
}
float Camera::GetFocalDistance(void) {
	return cameraConfig->getFocalDistance();
}

void Camera::SetHeight(float lr, cv::Point2f P1, cv::Point2f P2) {
	cameraConfig->setHeight(
			cameraConfig->getFocalDistance() * lr / norm(P1 - P2));
}
float Camera::GetHeight(void) {
	return cameraConfig->getHeight();
}

//it does not matter if SetTheta receives P1 and P2 crossing
//	y = cam.frame.rows / 2
void Camera::SetTheta(float l, cv::Point2f P1, cv::Point2f P2, float h) {
	double k, error, error2, theta, theta_increment;
	double fd = cameraConfig->getFocalDistance();

	int i;

	P1.y = CoordTransform::OpencvYCoord2PaperYCoord(*this, P1);
	P2.y = CoordTransform::OpencvYCoord2PaperYCoord(*this, P2);

	theta = 0.;
	theta_increment = PI / 180.;
	error2 = 0.;
	k = fabs(l / (h - cameraConfig->getHeight()));
	for (i = 0, error = DBL_MAX;
			(error > 10e-10) && (fabs(theta) <= PI / 2.) && (i < 1000); i++) {
		error2 = (fd * cos(theta) + P1.y * sin(theta))
				/ (fd * sin(theta) - P1.y * cos(theta));
		error2 -= (fd * cos(theta) + P2.y * sin(theta))
				/ (fd * sin(theta) - P2.y * cos(theta));
		error2 = fabs(error2);
		error2 = fabs(k - error2);
//		std::cout << "theta = " << theta*180./PI << "\ti = " << i <<
//			"      error = " << error << "  error2 =" << error2 << std::endl;
		if (error2 > error) {
			theta_increment *= -.5;
//			std::cout << "\t" << theta_increment*180/PI <<"°"<< std::endl;
		}
		theta += theta_increment;
		error = error2;
	}
	if (theta < 0.) {
		std::cout << "theta = " << theta << " < 0." << std::endl << "Aborting."
				<< std::endl;
		exit(EXIT_FAILURE);
	}
	cameraConfig->setTheta(theta);
	cameraConfig->setCosTheta(cos(theta));
	cameraConfig->setSinTheta(sin(theta));
}

float Camera::GetTheta(void) {
	return cameraConfig->getTheta();
}
float Camera::GetCosTheta(void) {
	return cameraConfig->getCosTheta();
}
float Camera::GetSinTheta(void) {
	return cameraConfig->getSinTheta();
}

void Camera::SetRz(cv::Point2f P, float h) {
	float Rz;

	P.y = CoordTransform::OpencvYCoord2PaperYCoord(*this, P);

	Rz = (cameraConfig->getFocalDistance() * cameraConfig->getCosTheta()
			+ P.y * cameraConfig->getSinTheta());
	Rz /= (cameraConfig->getFocalDistance() * cameraConfig->getSinTheta()
			- P.y * cameraConfig->getCosTheta());
	Rz *= (cameraConfig->getHeight() - h);

	cameraConfig->setRz(Rz);
}

float Camera::GetRz(void) {
	return cameraConfig->getRz();
}

void Camera::SaveConf(std::string fileName) {
	cameraConfig->PersistData(fileName);
}
