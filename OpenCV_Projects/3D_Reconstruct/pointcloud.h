#pragma once
#include "threedreconstructor.h"
#include <vector>



void run_threed_from_point_cloud();
void load_images(cv::Mat& image1, cv::Mat& image2);
void feature_detection(cv::Mat& img1, cv::Mat& img2, std::vector<cv::Point2f>& pts1, std::vector<cv::Point2f>& pts2, bool show_result);
CameraParams get_camera_params();
void find_matrices(const CameraParams& params, const std::vector<cv::Point2f> pts1, const std::vector<cv::Point2f>& pts2, cv::Mat& K, cv::Mat& R, cv::Mat& T, cv::Mat& inliers);
std::vector<cv::Point3d> recover_3d_points(cv::Mat& K, cv::Mat& R, cv::Mat& T, cv::Mat& inliers, const std::vector<cv::Point2f> pts1, const std::vector<cv::Point2f>& pts2);
void write_to_obj(std::vector<cv::Point3d> p3d, const std::string& filename);
void visualise_point_cloud(const std::string& filename);
