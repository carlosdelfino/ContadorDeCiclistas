#include "Utils.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/timeb.h>

bool IsMidnight() {
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	int now = timeinfo->tm_sec + timeinfo->tm_min * 60
			+ timeinfo->tm_hour * 60 * 60;
	return now == 0;
}

void print_usage(std::string program_name) {
	std::cout << program_name << " usage:" << std::endl
			<< "\t--help   (-h): print this message." << std::endl
			<< "\t--override (-O): override point picked parameters."
			<< std::endl
			<< "\t--reg_source (-s) <file_name>: Specify regular file where data "
			<< "comes from." << std::endl
			<< "\t--dev_source (-D) <number>: Specify device number where data "
			<< "comes from." << std::endl
			<< "\t--record (-r) <file_name>: Record video to filename."
			<< std::endl
			<< "\t--stream (-S) <device>: Streams video via device."
			<< std::endl
			<< "\t--sensor (-t) <device>: Specify device file which is a sensor."
			<< std::endl
			<< "\t--address (-a) <address>: address must be present and must be in "
			<< "in this format: street-number. --address faria_lima-1200."
			<< std::endl;
	return;
}

bool test_file(std::string file) {
	const char *c;
	int fd;

	if (file.empty())
		return true;

	c = file.c_str();
	if ((fd = open(c, O_RDWR)) < 0) {
		std::cout << "'" << file << "': ";
		std::cout.flush();
		perror("The following error occurred");
		return false;
	} else {
		close(fd);
		return true;
	}
}

void Print(const char *message, cv::Point position, cv::Mat &frame,
		unsigned int fontSize, unsigned int thickness) {
	cv::putText(frame, std::string(message), position,
	CV_FONT_HERSHEY_PLAIN, fontSize, cv::Scalar(0, 0, 0), thickness, CV_AA);
}
