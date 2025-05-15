#define NOMINMAX //crtical to be at top! since std::min max is used in the code
#include <fstream>
#include <limits>
#include <opencv2\imgproc.hpp>
#include <opencv2\calib3d.hpp>
#include <open3d/Open3D.h>
#include <open3d/geometry/PointCloud.h>
#include "depthmap.h"

using namespace open3d;
using namespace cv;

#define DEBUG

void run_threed_from_depth_map() {
	cv::Mat img1, img2;
	load_images(img1, img2);
	cvtColor(img1, img1, COLOR_BGR2GRAY);
	cvtColor(img2, img2, COLOR_BGR2GRAY);

	//stereo configure, values are volatile. this works
	int numDisparities = 16*5;  
	int blockSize = 5;       
	int minDisparity = 0;
	int uniquenessRatio = 10;
	int speckleWindowSize = 100;
	int speckleRange = 32;
	int disp12MaxDiff = 1;
	int preFilterCap = 50;
	int channels = 1;// image already converted to gray but just to make sure
	int P1 = 8 * channels * blockSize * blockSize;
	int P2 = 32 * channels * blockSize * blockSize;

	Ptr<StereoSGBM> sgbm = StereoSGBM::create(minDisparity, numDisparities, blockSize,
		P1, P2, disp12MaxDiff, preFilterCap,
		uniquenessRatio, speckleWindowSize, speckleRange);

	Mat disparity;
	sgbm->compute(img1, img2, disparity);

#ifdef DEBUG
	// Normalize the disparity map for visualization
	cv::Mat disp_vis;
	cv::normalize(disparity, disp_vis, 0, 255, cv::NORM_MINMAX, CV_8U);
	cv::imshow("Disparity", disp_vis);
	cv::waitKey(0);
#endif
	CameraParams middlebury = get_middlebury_camera_params();


	//based on the existence of doffset we have to version of Q matrix
	//if doff is not 0 we have to use the first version
	Mat Q = (Mat_<double>(4, 4) <<
		1, 0, 0, -middlebury.cx,
		0, 1, 0, -middlebury.cy,
		0, 0, 0, middlebury.fx,
		0, 0, -1 / middlebury.baseline, middlebury.doffset/middlebury.baseline);
		
	
#ifdef DEBUG
	std::cout << "Q: " << Q << std::endl;
#endif

	Mat points3D;
	//disparity is 64F but thats apparently not supported. so we convert
	disparity.convertTo(disparity, CV_32F, 1.0 / 16.0);
	reprojectImageTo3D(disparity, points3D, Q, true);

	cv::Mat mask = disparity >1 & disparity<100;
	cv::Mat img_color = cv::imread("Resources//im0.png");
	cv::Mat img_color_resized;
	cv::resize(img_color, img_color_resized, disparity.size());

	write_to_ply("output.ply", points3D, img_color_resized, mask);

	bool retFlag;
	visualize_ply(retFlag);
	if (retFlag) return;





}
void write_to_ply(const std::string& filename, const cv::Mat& points, const cv::Mat& colors, const cv::Mat& mask) {
	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Failed to open PLY file." << std::endl;
		return;
	}

	int valid_points = cv::countNonZero(mask);
	file << "ply\nformat ascii 1.0\nelement vertex " << valid_points
		<< "\nproperty float x\nproperty float y\nproperty float z\n"
		<< "property uchar red\nproperty uchar green\nproperty uchar blue\nend_header\n";

	for (int y = 0; y < points.rows; ++y) {
		for (int x = 0; x < points.cols; ++x) {
			if (mask.at<uchar>(y, x)) {
				cv::Vec3f point = points.at<cv::Vec3f>(y, x);
				cv::Vec3b color = colors.at<cv::Vec3b>(y, x);
				file << point[0] << " " << point[1] << " " << point[2] << " "
					<< (int)color[2] << " " << (int)color[1] << " " << (int)color[0] << "\n";
			}
		}
	}

	file.close();
}
void visualize_ply(bool& retFlag)
{
	retFlag = true;
	auto pcd = std::make_shared<open3d::geometry::PointCloud>();

	if (open3d::io::ReadPointCloud("output.ply", *pcd)) {

		std::cout << "Successfully loaded point cloud. Visualizing..." << std::endl;
	}
	else return;
	pcd->RemoveNonFinitePoints();  // Clean NaNs and infs
	auto clean = std::make_shared<open3d::geometry::PointCloud>();
	for (size_t i = 0; i < pcd->points_.size(); ++i) {
		if (pcd->points_[i].z() > -10000) {
			clean->points_.push_back(pcd->points_[i]);
			if (!pcd->colors_.empty()) clean->colors_.push_back(pcd->colors_[i]);
		}
	}
	open3d::visualization::DrawGeometries({ clean }, "3D Point Cloud", 800, 600);

	verify_z_values(pcd);
	retFlag = false;
}

void verify_z_values(std::shared_ptr<open3d::geometry::PointCloud>& pcd)
{
	double z_min = std::numeric_limits<double>::max();
	double z_max = std::numeric_limits<double>::lowest();

	for (const auto& point : pcd->points_) {
		if (!std::isfinite(point.z())) continue;
		z_min = std::min(z_min, point.z());
		z_max = std::max(z_max, point.z());
	}
	std::cout << "Z Range: [" << z_min << ", " << z_max << "]" << std::endl;
}
