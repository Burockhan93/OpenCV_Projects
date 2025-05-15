#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

enum class StitchingMethod {
	Default,
	Custom
};

void stitch_images(StitchingMethod);
std::vector<cv::Mat> load_images();
std::vector<cv::Mat> estimate_cumulative_homographies(std::vector<cv::Mat>& images);
cv::Mat warp_and_stitch(std::vector<cv::Mat>& images, std::vector<cv::Mat>& cumulativeH);