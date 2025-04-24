#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include "Utils\plotlib.h"



#define ENTRY
#define BACKGROUND_SUBTRACTION
#define NEURAL_NETWORK

extern void run_background_subtraction();
extern void testNeuralNetwork();

void fill_frame(cv::Mat& mask, cv::Mat& hsv_mask, cv::Mat& result, cv::Mat& alternate_background);

void erosion_dilation(cv::Mat& mask, cv::Mat& hsv_mask);

int main()
{
#ifndef	ENTRY
	cv::Mat img = cv::imread("Resources//shrek.png");
	if (img.empty()) {
		std::cerr << "Could not open or find the image!\n";
		return EXIT_FAILURE;
	}
	const std::string text{ "ADAM SANDIK ESEGEEE" };
	cv::putText(img, text,cv::Point(img.rows/10,img.cols/10),0,1,cv::Scalar(255,255,255),5);

	cv::imshow("Display window", img);
	 
	cv::waitKey(0);
#endif
#ifndef BACKGROUND_SUBTRACTION

	run_background_subtraction();
#endif 
#ifndef NEURAL_NETWORK
	testNeuralNetwork();
#endif
	using namespace utils;

	std::mt19937 gen(std::random_device{}());
	std::uniform_real_distribution<float> dis(0.0f,0.05f);
	std::vector<std::vector<float>> points(100);
	float _init_x = -0.99f;
	float _init_y = -0.99f;
	points[0] = { _init_x, _init_y };
	for (size_t i = 1; i < points.size(); i++)
	{
		float x = dis(gen);
		float y = dis(gen);
		points[i] = { points[i-1][0] + x,points[i - 1][1] + y};
	}
	PlotLib plot(800, 600,points);
	plot.plot_show();


    
}
