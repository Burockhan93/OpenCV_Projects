// Created by Burakhan Dogan
#include "pointcloud.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <algorithm>
#include <fstream>
#include <Open3D/Open3D.h>

using namespace cv;
using namespace std;

void run_threed_from_point_cloud() {
	cv::Mat img1, img2;
	load_images(img1, img2);
	//points from first and second image for homography
	std::vector<cv::Point2f> pts1, pts2;
	feature_detection(img1, img2, pts1, pts2, true);

	CameraParams xaomi_14_ultra = get_xiaomi_camera_params();

	//find important matrices
	Mat K, R, T, inliers;
	find_matrices(xaomi_14_ultra, pts1, pts2, K, R, T, inliers);

	//3d points
	vector<Point3d> p3d = recover_3d_points(K, R, T, inliers, pts1, pts2);

	//create and visuaklise point cloud
	const std::string filename = "output.obj";
	write_to_obj(p3d, filename);
	visualise_obj_point_cloud(filename);
}

void feature_detection(Mat& img1, Mat& img2, std::vector<cv::Point2f>& pts1, std::vector<cv::Point2f>& pts2, bool show_result) {
	Ptr<ORB> orb = ORB::create(5000);
	vector<KeyPoint> keypoints1, keypoints2;
	Mat descriptors1, descriptors2;
	orb->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
	orb->detectAndCompute(img2, noArray(), keypoints2, descriptors2);
	BFMatcher matcher(NORM_HAMMING, true);
	//std::vector<vector<DMatch>> matches;
	vector<DMatch> matches;
	//matcher.knnMatch(descriptors1, descriptors2, matches, 2);
	matcher.match(descriptors1, descriptors2, matches);
	// Filter matches using the Lowe's ratio test
	vector<DMatch> good_matches;
	/*for (const auto& match : matches) {
		if (match[0].distance < 0.88 * match[1].distance) {
			good_matches.push_back(match[0]);
		}
	}*/
	for (const auto& match : matches) {

		if (std::abs(keypoints1[match.queryIdx].pt.y - keypoints2[match.trainIdx].pt.y) < 300)
		{
			good_matches.push_back(match);
		}
		else {
			continue;
		}
	}
	if (show_result) {

		Mat img_matches;
		drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
		namedWindow("Matches", WINDOW_FREERATIO);
		imshow("Matches", img_matches);
		waitKey(0);
		img_matches.release();
		drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches);
		imshow("Matches", img_matches);
		waitKey(0);
	}

	for (auto& match : good_matches) {
		pts1.push_back(keypoints1[match.queryIdx].pt);
		pts2.push_back(keypoints2[match.trainIdx].pt);
	}

}

void find_matrices(const CameraParams& params, const std::vector<cv::Point2f> pts1, const std::vector<cv::Point2f>& pts2, Mat& K, Mat& R, Mat& T, Mat& inliers) {

	K = (Mat_<double>(3, 3) <<
		params.fx, 0, params.cx,
		0, params.fy, params.cy,
		0, 0, 1);
	cout << "K: " << K << endl;

	/*Mat K = (Mat_<double>(3, 3) << 1733.74, 0, 792.27,
		0, 1733.74, 541.89,
		0, 0, 1);*/




		//project the points on camera plane
	Mat E = findEssentialMat(pts1, pts2, K, RANSAC);
	recoverPose(E, pts1, pts2, K, R, T, inliers);
	T /= norm(T);
	cout << "R: " << R << endl;
	cout << "T: " << T << endl;
	cout << "K: " << K << endl;
}

vector<Point3d> recover_3d_points(Mat& K, Mat& R, Mat& T, Mat& inliers, const std::vector<cv::Point2f> pts1, const std::vector<cv::Point2f>& pts2) {
	std::vector<Point2f> inlierPts1, inlierPts2;

	for (int i = 0; i < inliers.rows; i++) {
		if (inliers.at<uchar>(i)) {
			inlierPts1.push_back(pts1[i]);
			inlierPts2.push_back(pts2[i]);
		}
	}

	Mat P1 = K * Mat::eye(3, 4, CV_64F); // triangulaatepoints expects 3*4 matrice
	Mat Rt;
	hconcat(R, T, Rt);//rt 3*4 now
	Mat P2 = K * Rt;

	cv::Mat pnts3D(4, inlierPts1.size(), CV_64F);
	vector<Point3d> p3d;
	triangulatePoints(P1, P2, inlierPts1, inlierPts2, pnts3D);

	for (int i = 0; i < pnts3D.cols; i++) {
		Mat col = pnts3D.col(i);
		col.convertTo(col, CV_64F);
		col /= col.at<double>(3, 0);//homogenous coordinates
		p3d.push_back(Point3d(col.at<double>(0, 0), col.at<double>(1, 0), col.at<double>(2, 0)));
	}
	return p3d;
}
void write_to_obj(std::vector<cv::Point3d> p3d, const std::string& filename) {

	std::ofstream file(filename);

	for (auto p : p3d) {
		file << "v " << p.x << " " << p.y << " " << p.z << std::endl;
	}
	file.close();

}

void visualise_obj_point_cloud(const std::string& filename) {
	using namespace open3d;
	// Load point cloud
	auto pcd = std::make_shared<geometry::PointCloud>();

	std::ifstream infile(filename);
	std::string line;
	while (std::getline(infile, line)) {
		if (line.substr(0, 2) == "v ") {
			std::istringstream iss(line.substr(2));
			double x, y, z;
			iss >> x >> y >> z;
			pcd->points_.emplace_back(x, y, z);
		}
	}

	std::cout << "Loaded " << pcd->points_.size() << " points." << std::endl;

	// Estimate normals before reconstruction
	pcd->EstimateNormals();

	// Poisson surface reconstruction
	auto [mesh_ptr, densities] = geometry::TriangleMesh::CreateFromPointCloudPoisson(*pcd, 8);

	// Show the mesh
	visualization::DrawGeometries({ std::static_pointer_cast<const geometry::Geometry>(mesh_ptr) },
		"Mesh from manually loaded .obj");
}
