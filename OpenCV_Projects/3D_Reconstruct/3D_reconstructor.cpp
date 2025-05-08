#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <algorithm>

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
	Size cropped_image_size; 

	double fx{}, fy{}, cx{}, cy{}; // Intrinsic parameters

};
void run_3d_reconstruction() {
	cv::Mat img1 = cv::imread("Resources//threed1.jpg");
	cv::Mat img2 = cv::imread("Resources//threed2.jpg");
	if (img1.empty() || img2.empty()) {
		std::cerr << "Could not open or find the image!\n";
		return;
	}
	cv::imshow("Display window", img1);
	cv::imshow("Display window", img2);
	//cv::waitKey(0);

	Ptr<ORB> orb = ORB::create();
	vector<KeyPoint> keypoints1, keypoints2;
	Mat descriptors1, descriptors2;
	orb->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
	orb->detectAndCompute(img2, noArray(), keypoints2, descriptors2);
	BFMatcher matcher(NORM_HAMMING);
	vector<DMatch> matches;
	matcher.match(descriptors1, descriptors2, matches);
	// Filter matches using the Lowe's ratio test
	vector<DMatch> good_matches;
	std::copy_if(matches.begin(), matches.end(), std::back_inserter(good_matches), [](const DMatch& match) {
		return match.distance > 80;
		});

	Mat img_matches;
	drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches);
	imshow("Matches", img_matches);
	waitKey(0);

	CameraParams xaomi_14_ultra;
	xaomi_14_ultra.focal_length = 8.72;
	xaomi_14_ultra.actual_image_size = Size(3072, 4096);
	xaomi_14_ultra.cropped_image_size = Size(640, 480);
	xaomi_14_ultra.sensor_width = 13.2;
	xaomi_14_ultra.sensor_height = 8.8;
	xaomi_14_ultra.scale_x = (double)xaomi_14_ultra.cropped_image_size.width / xaomi_14_ultra.actual_image_size.width;
	xaomi_14_ultra.scale_y = (double)xaomi_14_ultra.cropped_image_size.height / xaomi_14_ultra.actual_image_size.height;

	double fx = xaomi_14_ultra.focal_length * xaomi_14_ultra.scale_x * xaomi_14_ultra.cropped_image_size.width;
	double fy = xaomi_14_ultra.focal_length * xaomi_14_ultra.scale_y * xaomi_14_ultra.cropped_image_size.height;

	double cx = xaomi_14_ultra.cropped_image_size.width / 2;
	double cy = xaomi_14_ultra.cropped_image_size.height / 2;

	Mat K;
	K = (Mat_<float>(3, 3) << 
		fx, 0, cx,
		0, fy, cy,
		0, 0, 1);

	std::vector<cv::Point2f> pts1, pts2;
	for (auto& match : good_matches) {
		pts1.push_back(keypoints1[match.queryIdx].pt);
		pts2.push_back(keypoints2[match.trainIdx].pt);
	}
	Mat E = findEssentialMat(pts1, pts2, K, RANSAC);
	Mat R, T;
	recoverPose(E, pts1, pts2,K, R, T);
	cout << "R: " << R << endl;
	cout << "T: " << T << endl;
	cout << "K: " << K << endl;
	int i =10;



}