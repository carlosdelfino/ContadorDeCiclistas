#ifndef UTILS_HPP
#define UTILS_HPP

#include<opencv2/opencv.hpp>

#include "ObjectTracker.hpp"
#include "Sensors.hpp"

#include<time.h>

bool IsMidnight();

void print_usage(std::string program_name);
bool test_file(std::string file);

void Print(const char *message, cv::Point position, cv::Mat &frame,
		   unsigned int fontSize = 1, unsigned int thickness = 1);




#endif
