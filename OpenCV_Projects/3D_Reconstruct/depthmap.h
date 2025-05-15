#pragma once
#include "threedreconstructor.h"
#include <vector>

void run_threed_from_depth_map();
void visualize_ply(bool& retFlag);
void verify_z_values(std::shared_ptr<open3d::geometry::PointCloud>& pcd);
void write_to_ply(const std::string& filename, const cv::Mat& points, const cv::Mat& colors, const cv::Mat& mask);