#pragma once
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <random>
#include <iostream>


namespace utils {

	enum class PlotType
	{
		dot = 0,
		line = 1,
		dot_line = 2,
		dot_line_fill = 3,
		dot_fill = 4,
		line_fill = 5

	};

	class PlotLib {


	private:
		const std::string window_name = "Plot";
		cv::Mat plotBackground;
		cv::Point origin;
		int plotPadding = 50;// padding for the plot area in total. so every side is half of it
		int plotXMin = 0;
		int plotXMax = 0;
		int plotXCenter = 0;
		int plotYMin = 0;
		int plotYMax = 0;
		int plotYCenter = 0;
	public:
		int window_width = 800;
		int window_height = 600;
		bool showGrid = true;
		bool show_negative = false;
		void prepare_plot(int w_width, int w_height);
		void transform(std::vector<std::vector<float>>& points);
		void plot(std::vector<std::vector<float>>& points, PlotType type = PlotType::dot, const cv::Scalar& color = cv::Scalar(255, 255, 255), int thickness = 1);
		void plot_show();

		PlotLib() = default;
		PlotLib(int w_width, int w_height, std::vector<std::vector<float>>& points, bool showNegative = true);
		PlotLib(int w_width, int w_height, bool showNegative = true);

	};
}
