#include "threedreconstructor.h"
#include <fstream>
#include <vector>

void run_threed_from_point_cloud();
void run_threed_from_depth_map();




void load_images(cv::Mat& image1, cv::Mat& image2) {
	image1 = cv::imread("Resources//im0.png");
	image2 = cv::imread("Resources//im1.png");
	if (image1.empty() || image2.empty()) {
		std::cerr << "Could not open or find the image!\n";
	}
}


CameraParams get_xiaomi_camera_params() {
	CameraParams xaomi_14_ultra;
	xaomi_14_ultra.focal_length = 8.72;
	//xaomi_14_ultra.actual_image_size = Size(3072, 4096);
	xaomi_14_ultra.actual_image_size = cv::Size(3072, 4096);
	xaomi_14_ultra.sensor_width = 13.2;
	xaomi_14_ultra.sensor_height = 8.8;

	xaomi_14_ultra.fx = (xaomi_14_ultra.focal_length / xaomi_14_ultra.sensor_width) * xaomi_14_ultra.actual_image_size.width;
	xaomi_14_ultra.fy = (xaomi_14_ultra.focal_length / xaomi_14_ultra.sensor_height) * xaomi_14_ultra.actual_image_size.height;

	xaomi_14_ultra.cx = xaomi_14_ultra.actual_image_size.width / 2;
	xaomi_14_ultra.cy = xaomi_14_ultra.actual_image_size.height / 2;
	return xaomi_14_ultra;
}
CameraParams get_middlebury_camera_params() {
	CameraParams middlebury;
	middlebury.fx = 7190.247;
	middlebury.fy = 7190.247;
	middlebury.cx = 1035.513;
	middlebury.cy = 945.196;
	middlebury.sensor_width = 1920;
	middlebury.sensor_height = 1080;
	middlebury.baseline = 174.945;
	middlebury.doffset = 342.523;
	middlebury.cx_ = 1378.036;
	return middlebury;
}

void run_3d_reconstruction(Technique3D techniqe) {

	if (techniqe == Technique3D::POINT_CLOUD) {
		std::cout << "3D reconstruction from point cloud..." << std::endl;
		run_threed_from_point_cloud();
	}
	else if (techniqe == Technique3D::DEPTH_MAP) {
		std::cout << "3D reconstruction from depth map" << std::endl;
		run_threed_from_depth_map();

	}
	else {
		std::cerr << "Unknown technique" << std::endl;
		return;
	}

}