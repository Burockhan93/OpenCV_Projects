#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>

#include <filesystem>

using namespace cv;
using namespace std;
const string windowName = "PANO";	

void load_images(vector<Mat>& images) {


	// Load images from the specified directory
	vector<string> files;
	string dir = "Resources/Panorama/*.jpeg";
	glob(dir, files);
	for (size_t i = 0; i < files.size(); i++) {
		Mat img = imread(files[i]);
		if (img.empty()) {
			cerr << "Could not read the image: " << files[i] << endl;
			continue;
		}
		images.push_back(img);
	}
	if (images.empty()) {
		cerr << "No images found in the specified directory." << endl;
		return;
	}
}
void estimate_homography(vector<Mat>& images) {

	Ptr<ORB> orb = ORB::create();
	vector<vector<KeyPoint>> keypoints(images.size());
	vector<Mat> descriptors((images.size()));
	vector<Mat> homographies;
	homographies.push_back(Mat::eye(3, 3, CV_64F));
	for (size_t i = 0; i < images.size(); i++){
		
		orb->detectAndCompute(images[i], noArray(), keypoints[i], descriptors[i]);
	}

	BFMatcher matcher(NORM_HAMMING);

	for (size_t i = 1; i < images.size(); i++) {
		vector<vector<DMatch>> matches;
		matcher.knnMatch(descriptors[0], descriptors[i], matches, 2);

		vector<DMatch> good_matches;
		for (size_t j = 0; j < matches.size(); j++) {
			if (matches[j][0].distance < 0.75 * matches[j][1].distance) {
				good_matches.push_back(matches[j][0]);
			}
		}

		// Spatial filtering (optional)
		vector<DMatch> filtered_matches;
		for (const auto& match : good_matches) {
			Point2f pt0 = keypoints[0][match.queryIdx].pt;
			Point2f pti = keypoints[i][match.trainIdx].pt;

			if (pt0.x > 0.6 * images[0].cols && pti.x < 0.4 * images[i].cols) {
				filtered_matches.push_back(match);
			}
		}

		vector<Point2f> points0, pointsi;
		for (const auto& match : filtered_matches) {
			points0.push_back(keypoints[0][match.queryIdx].pt);
			pointsi.push_back(keypoints[i][match.trainIdx].pt);
		}

		Mat H = findHomography(pointsi, points0, RANSAC); // warping img[i] -> img[0]
		homographies.push_back(H);
	}
	double minX = 0, minY = 0, maxX = images[0].cols, maxY = images[0].rows;

	for (size_t i = 1; i < images.size(); i++) {
		vector<Point2f> corners = {
			Point2f(0, 0),
			Point2f(images[i].cols, 0),
			Point2f(images[i].cols, images[i].rows),
			Point2f(0, images[i].rows)
		};
		vector<Point2f> warpedCorners;
		perspectiveTransform(corners, warpedCorners, homographies[i]);

		for (const auto& pt : warpedCorners) {
			minX = min(minX, (double)pt.x);
			minY = min(minY, (double)pt.y);
			maxX = max(maxX, (double)pt.x);
			maxY = max(maxY, (double)pt.y);
		}
	}

	Mat translation = (Mat_<double>(3, 3) << 1, 0, -minX, 0, 1, -minY, 0, 0, 1);
	Size panoSize((int)(maxX - minX), (int)(maxY - minY));
	Mat panorama(panoSize, CV_8UC3, Scalar::all(0));
	for (size_t i = 0; i < images.size(); i++) {
		Mat warped;
		warpPerspective(images[i], warped, translation * homographies[i], panoSize);

		for (int y = 0; y < panorama.rows; ++y) {
			for (int x = 0; x < panorama.cols; ++x) {
				Vec3b& panoPixel = panorama.at<Vec3b>(y, x);
				Vec3b warpedPixel = warped.at<Vec3b>(y, x);
				if (warpedPixel != Vec3b(0, 0, 0)) {
					panoPixel = warpedPixel;
				}
			}
		}
	}
	// Display the stitched image
	namedWindow(windowName, WINDOW_NORMAL);
	imshow(windowName, panorama);
	waitKey(0);


}
void stitch_images() {


	vector<Mat> images;
	load_images(images);
	estimate_homography(images);

	Mat pano;
	
	imshow(windowName,pano );
	waitKey(0);

}