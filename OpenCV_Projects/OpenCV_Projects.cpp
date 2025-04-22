#include <iostream>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

#define ENTRY
#define BACKGROUND_SUBTRACTION

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

	testNeuralNetwork();
    
}
