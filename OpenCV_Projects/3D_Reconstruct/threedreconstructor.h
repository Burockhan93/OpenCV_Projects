#pragma once
#include <opencv2/opencv.hpp>


enum class Technique3D {
	POINT_CLOUD,
	DEPTH_MAP
};


struct CameraParams {

	cv::Mat K; // Intrinsic camera matrix
	cv::Mat R; // Rotation matrix
	cv::Mat T; // Translation vector

	double baseline{}; // Baseline distance
	double doffset{}; // Disparity offset
	double focal_length{}; // Focal length
	double sensor_width{}; // Sensor width
	double sensor_height{}; // Sensor height
	double scale_x{};
	double scale_y{};
	cv::Size actual_image_size;

	double fx{}, fy{}, cx{}, cx_{}, cy{}; // Intrinsic parameters

};

void load_images(cv::Mat& image1, cv::Mat& image2);
CameraParams get_xiaomi_camera_params();
CameraParams get_middlebury_camera_params();

void run_3d_reconstruction(Technique3D techniqe);