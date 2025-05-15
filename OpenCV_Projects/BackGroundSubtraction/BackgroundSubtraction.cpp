
#include <iostream>
#include "BackgroundSubtraction.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;


int g_patch_size = 5;
int g_diff = 50;
Mat real_frame;
const string window_name = "Background Subtraction";

void run_background_subtraction(SubtractionMethod method) {

	cv::VideoCapture cap(0);


	if (!cap.isOpened()) {
		std::cerr << "Error: Could not open camera.\n";
		return;
	}

	Mat background, background_gray, alternate_background;

	background = imread("Resources//background.jpg");
	alternate_background = imread("Resources//background.png");
	if (background.empty() || alternate_background.empty()) {
		std::cerr << "Error: Could not read background image.\n";
		return;
	}
	cvtColor(background, background_gray, COLOR_BGR2GRAY);


	Mat frame_current, frame_previous, hsv_current, hsv_previous;

	cap >> frame_current;

	resize(alternate_background, alternate_background, frame_current.size());

	namedWindow(window_name, WINDOW_AUTOSIZE);
	int counter = 0;

	while (true) {

		cap >> frame_current;
		real_frame = frame_current.clone();
		cvtColor(frame_current, hsv_current, COLOR_BGR2HSV);
		cvtColor(frame_current, frame_current, COLOR_BGR2GRAY);

		vector<Mat> hsv_channels;
		split(hsv_current, hsv_channels);
		hsv_current = hsv_channels[1];//saturation channel

		if (counter <= 1)
		{
			frame_previous = frame_current.clone();
			hsv_previous = hsv_current.clone();
			counter++;
			continue;

		}


		Mat mask, hsv_mask;
		image_subtraction(frame_current, background_gray, mask);
		image_subtraction(hsv_current, hsv_previous, hsv_mask);

		erosion_dilation(mask, hsv_mask);

		Mat result(mask.size(), CV_8UC3);

		fill_frame(mask, hsv_mask, result, alternate_background);

		cv::imshow(window_name, result);
#ifdef DEBUG
		cv::imshow("Source", real_frame);
#endif

		frame_previous = frame_current.clone();
		hsv_previous = hsv_current.clone();



		counter++;
		cv::waitKey(1);



	}

	cap.release();
	cv::destroyAllWindows();

}
void erosion_dilation(cv::Mat& mask, cv::Mat& hsv_mask)
{
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	Mat element2 = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

	erode(mask, mask, element);
	dilate(mask, mask, element2);
	erode(hsv_mask, hsv_mask, element);
	dilate(hsv_mask, hsv_mask, element2);
}

void image_subtraction(const Mat& firstImg, const Mat& secondImg, Mat& result) {

	Mat blur1, blur2;
	result = Mat::zeros(firstImg.size(), CV_8UC1);
	medianBlur(firstImg, blur1, 3);
	medianBlur(secondImg, blur2, 3);
	for (int i = 0; i < blur1.rows; i += g_patch_size) {
		for (int j = 0; j < blur1.cols; j += g_patch_size) {


			if (j + g_patch_size >= blur1.cols || i + g_patch_size >= blur1.rows) continue;

			int sumFirst = 0, sumSecond = 0;
			for (int y = 0; y < g_patch_size; ++y)
				for (int x = 0; x < g_patch_size; ++x) {
					sumFirst += (int)blur1.at<uchar>(i + y, j + x);
					sumSecond += (int)blur2.at<uchar>(i + y, j + x);
				}

			int diff = abs(sumFirst / pow(g_patch_size,2) - sumSecond / pow(g_patch_size, 2));
			if (diff > g_diff) {
				for (int y = 0; y < g_patch_size; ++y)
					for (int x = 0; x < g_patch_size; ++x)
						result.at<uchar>(i + y, j + x) = 255;
			}


		}
	}
}
void fill_frame(cv::Mat& mask, cv::Mat& hsv_mask, cv::Mat& result, cv::Mat& alternate_background)
{
	for (int m = 0; m < real_frame.rows; m++)
	{
		for (int n = 0; n < real_frame.cols; n++)
		{
			if ((int)mask.at<uchar>(Point(n, m)) != 0 || (int)hsv_mask.at<uchar>(Point(n, m)) != 0)
			{
				result.at<Vec3b>(Point(n, m))[0] = real_frame.at<Vec3b>(Point(n, m))[0];
				result.at<Vec3b>(Point(n, m))[1] = real_frame.at<Vec3b>(Point(n, m))[1];
				result.at<Vec3b>(Point(n, m))[2] = real_frame.at<Vec3b>(Point(n, m))[2];

			}

			else
			{
				result.at<Vec3b>(Point(n, m))[0] = alternate_background.at<Vec3b>(Point(n, m))[0];
				result.at<Vec3b>(Point(n, m))[1] = alternate_background.at<Vec3b>(Point(n, m))[1];
				result.at<Vec3b>(Point(n, m))[2] = alternate_background.at<Vec3b>(Point(n, m))[2];
			}

		}
	}
}

