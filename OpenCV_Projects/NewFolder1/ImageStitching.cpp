#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2/stitching.hpp>

using namespace cv;
using namespace std;

const string windowName = "PANO";	

void stitch_images() {

	Ptr<Stitcher> stitcher = Stitcher::create();
	vector<Mat> images;

	images.push_back(imread("Resources/1.jpg"));


}