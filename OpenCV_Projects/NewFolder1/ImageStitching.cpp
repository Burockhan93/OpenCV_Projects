#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>

#include <filesystem>

using namespace cv;
using namespace std;
const string windowName = "PANO";	

vector<Mat> load_images() {

	vector<Mat> images;
	// Load images from resources folder.( I have placed an extra panorama folder)
	vector<string> files;
	string dir = "Resources/Panorama/*.jpeg"; // opencv can detect extensions
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
		return vector<Mat>{};//return empty if nothing found
	}
	return images;

}
//normal homography has too much skewedness when the image aparture is a little high.
vector<Mat> estimate_cumulative_homographies(vector<Mat>& images) {
	//not much differnce between sift and orb
    Ptr<ORB> orb = ORB::create();
    vector<vector<KeyPoint>> keypoints(images.size());
    vector<Mat> descriptors(images.size());
    for (size_t i = 0; i < images.size(); i++) {
        orb->detectAndCompute(images[i], noArray(), keypoints[i], descriptors[i]);
    }
    //no need for flann.
    BFMatcher matcher(NORM_HAMMING);
    vector<Mat> pairwiseH;
	pairwiseH.push_back(Mat::eye(3, 3, CV_64F)); //first image is the reference image 

    for (size_t i = 1; i < images.size(); i++) {
        vector<vector<DMatch>> matches;
        matcher.knnMatch(descriptors[i - 1], descriptors[i], matches, 2);//knnmatch returns a vector of vector. its good since we can eliminate some matrches thrpough distance metric

        vector<DMatch> good_matches;
        for (size_t j = 0; j < matches.size(); j++) {
            if (matches[j][0].distance < 0.75 * matches[j][1].distance) {
                good_matches.push_back(matches[j][0]);
            }
        }

        vector<Point2f> pts_prev, pts_curr;
        for (const auto& match : good_matches) {
            pts_prev.push_back(keypoints[i - 1][match.queryIdx].pt);
            pts_curr.push_back(keypoints[i][match.trainIdx].pt);
        }
        //find homographies between image pairs
        Mat H = findHomography(pts_curr, pts_prev, RANSAC);
        pairwiseH.push_back(H);
    }

  
	//normal homography has too much skewedness when theere are more than 4 images(or panorama is too wide).
    int N = images.size();
    int centerIdx = N / 2;
    vector<Mat> cumulativeH(N);
    cumulativeH[centerIdx] = Mat::eye(3, 3, CV_64F);

    for (int i = centerIdx - 1; i >= 0; --i) {
		//project the image to the left
        cumulativeH[i] = cumulativeH[i + 1] * pairwiseH[i + 1].inv();
        //normalize
        cumulativeH[i] /= cumulativeH[i].at<double>(2, 2);
    }
    for (int i = centerIdx + 1; i < N; ++i) {
		//project the image to the right
        cumulativeH[i] = cumulativeH[i - 1] * pairwiseH[i];
        cumulativeH[i] /= cumulativeH[i].at<double>(2, 2);
    }

    return cumulativeH;
}

Mat warp_and_stitch(vector<Mat>& images, vector<Mat>& cumulativeH) {
    // Compute panorama size
    double minX = 0, minY = 0, maxX = images[0].cols, maxY = images[0].rows;

    for (size_t i = 0; i < images.size(); i++) {
        vector<Point2f> corners = {
            Point2f(0, 0),//top-left
			Point2f(images[i].cols, 0),//top-right
			Point2f(images[i].cols, images[i].rows),//bottom-right
			Point2f(0, images[i].rows)//bottom-left
        };
        vector<Point2f> warpedCorners;
        perspectiveTransform(corners, warpedCorners, cumulativeH[i]);

        //minx and miny can be negative. its okay
        for (const auto& pt : warpedCorners) {
            minX = min(minX, (double)pt.x);
            minY = min(minY, (double)pt.y);
            maxX = max(maxX, (double)pt.x);
            maxY = max(maxY, (double)pt.y);
        }
    }

    // Translation to keep coordinates positive
    Mat translation = (Mat_<double>(3, 3) << 1, 0, -minX, 0, 1, -minY, 0, 0, 1);
    Size panoSize((int)(maxX - minX), (int)(maxY - minY));
    Mat panorama(panoSize, CV_8UC3, Scalar::all(0));

    for (size_t i = 0; i < images.size(); i++) {
        Mat warped;
		// Warp the image using the cumulative homography
        warpPerspective(images[i], warped, translation * cumulativeH[i], panoSize);

		// Blend the images into the panorama. simple blending
        for (int y = 0; y < panorama.rows; ++y) {
            for (int x = 0; x < panorama.cols; ++x) {
                Vec3b warpedPixel = warped.at<Vec3b>(y, x);
                if (warpedPixel != Vec3b(0, 0, 0)) {
                    panorama.at<Vec3b>(y, x) = warpedPixel;
                }
            }
        }
    }

    return panorama;
}

void stitch_images() {
    vector<Mat> images = load_images(); 
    if (images.empty()) {
        cerr << "No images to stitch." << endl;
        return;
    }

    vector<Mat> cumulativeH = estimate_cumulative_homographies(images);
    Mat panorama = warp_and_stitch(images, cumulativeH);

    namedWindow(windowName, WINDOW_FREERATIO);
    imshow(windowName, panorama);
    imwrite("resources/panorama/pano.jpg", panorama);
    waitKey(0);
}