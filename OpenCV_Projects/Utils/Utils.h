#pragma once
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>


void mouse(int event, int x, int y, int flag, void*) {

	cv::Mat img;
	if (event == cv::EVENT_LBUTTONDOWN) {
		if (x >= 0 && x < img.cols && y >= 0 && y < img.rows) {

			cv::Vec3b colors = img.at<cv::Vec3b>(y, x);

			int b = colors[0];
			int g = colors[1];
			int r = colors[2];

			// ANSI escape sequence for RGB background: \x1b[48;2;R;G;Bm
			std::cout << "\x1b[48;2;" << r << ";" << g << ";" << b << "m" << "  " << "\x1b[0m"
				<< "  (" << x << ", " << y << ")"
				<< std::endl;
		}

	}

}