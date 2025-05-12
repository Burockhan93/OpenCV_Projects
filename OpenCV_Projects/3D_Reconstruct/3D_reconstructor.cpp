#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <algorithm>
#include <fstream>

using namespace cv;
using namespace std;

struct CameraParams {

	Mat K; // Intrinsic camera matrix
	Mat D; // Distortion coefficients
	Mat R; // Rotation matrix
	Mat T; // Translation vector

	double focal_length{}; // Focal length
	double sensor_width{}; // Sensor width
	double sensor_height{}; // Sensor height
	double scale_x{};
	double scale_y{};
	Size actual_image_size;

	double fx{}, fy{}, cx{}, cy{}; // Intrinsic parameters

};
void run_3d_reconstruction() {
	cv::Mat img1 = cv::imread("Resources//threed12.jpg");
	cv::Mat img2 = cv::imread("Resources//threed13.jpg");
	if (img1.empty() || img2.empty()) {
		std::cerr << "Could not open or find the image!\n";
		return;
	}


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

	Mat img_matches;
	drawMatches(img1, keypoints1, img2, keypoints2, matches, img_matches);
	namedWindow("Matches", WINDOW_FREERATIO);
	imshow("Matches", img_matches);
	waitKey(0);
	img_matches.release();
	drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches);
	imshow("Matches", img_matches);
	waitKey(0);

	CameraParams xaomi_14_ultra;
	xaomi_14_ultra.focal_length = 8.72;
	//xaomi_14_ultra.actual_image_size = Size(3072, 4096);
	xaomi_14_ultra.actual_image_size = Size(3072, 4096);
	xaomi_14_ultra.sensor_width = 13.2;
	xaomi_14_ultra.sensor_height = 8.8;

	double fx = (xaomi_14_ultra.focal_length / xaomi_14_ultra.sensor_width) * xaomi_14_ultra.actual_image_size.width;
	double fy = (xaomi_14_ultra.focal_length / xaomi_14_ultra.sensor_height) * xaomi_14_ultra.actual_image_size.height;

	double cx = xaomi_14_ultra.actual_image_size.width / 2;
	double cy = xaomi_14_ultra.actual_image_size.height / 2;

	Mat K;
	K = (Mat_<double>(3, 3) <<
		fx, 0, cx,
		0, fy, cy,
		0, 0, 1);

	std::vector<cv::Point2f> pts1, pts2;
	for (auto& match : good_matches) {
		pts1.push_back(keypoints1[match.queryIdx].pt);
		pts2.push_back(keypoints2[match.trainIdx].pt);
	}
	//project the points on camera plane
	Mat E = findEssentialMat(pts1, pts2, K, RANSAC);
	Mat R, T, inliers;
	recoverPose(E, pts1, pts2, K, R, T, inliers);
	T /= norm(T);
	cout << "R: " << R << endl;
	cout << "T: " << T << endl;
	cout << "K: " << K << endl;

	std::vector<Point2f> inlierPts1, inlierPts2;
	for (int i = 0; i < inliers.rows; i++) {
		if (inliers.at<uchar>(i)) {
			inlierPts1.push_back(pts1[i]);
			inlierPts2.push_back(pts2[i]);
		}
	}

	Mat P1 = K * Mat::eye(3, 4, CV_64F); // trianguölaatepoints expects 3*4
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

	//generate3d points
	std::ofstream objFile("output.obj");
	for (const auto& pt : p3d) {
		objFile << "v " << pt.x << " " << pt.y << " " << pt.z << "\n";
	}
	objFile.close();

}