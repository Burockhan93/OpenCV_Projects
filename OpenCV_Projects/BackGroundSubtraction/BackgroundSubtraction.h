#pragma once
#include <opencv2/opencv.hpp>

enum class SubtractionMethod {
	patch_based,
	dnn_based,
};

void run_background_subtraction(SubtractionMethod method);
void erosion_dilation(cv::Mat& mask, cv::Mat& hsv_mask);
void image_subtraction(const cv::Mat& firstImg, const cv::Mat& secondImg, cv::Mat& result);
void fill_frame(cv::Mat& mask, cv::Mat& hsv_mask, cv::Mat& result, cv::Mat& alternate_background);
