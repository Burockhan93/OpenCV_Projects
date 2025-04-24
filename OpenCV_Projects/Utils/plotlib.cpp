#include "plotlib.h"

//utils::PlotLib::PlotLib() = default;
//utils::PlotLib::PlotLib(int width, int height) {
//	plotXMin = plotPadding / 2;
//	plotXMax = window_width - plotPadding / 2;
//	plotXCenter = (plotXMax + plotXMin) / 2;
//	plotYMin = plotPadding / 2;
//	plotYMax = window_height - plotPadding / 2;
//	plotYCenter = (plotYMin + plotYMax) / 2;
//}

void utils::PlotLib::prepare_plot(int w_width, int w_height) {
	plotXMin = plotPadding / 2;
	plotXMax = window_width - plotPadding / 2;
	plotXCenter = (plotXMax + plotXMin) / 2;
	plotYMin = plotPadding / 2;
	plotYMax = window_height - plotPadding / 2;
	plotYCenter = (plotYMin + plotYMax) / 2;

	if (show_negative) {
		origin = cv::Point(w_width / 2, w_height / 2);

		//cv::rectangle(plotBackground, cv::Point(plotXMin, plotYMin), cv::Point(plotXMax, plotYMax), cv::Scalar(255, 255, 255), 1);

		cv::arrowedLine(plotBackground, origin, cv::Point(plotXMax, plotYCenter), cv::Scalar(255, 255, 255), 1, cv::LINE_8, 0, 0.025);
		cv::arrowedLine(plotBackground, origin, cv::Point(plotXMin, plotYCenter), cv::Scalar(255, 255, 255), 1, cv::LINE_8, 0, 0.025);
		cv::arrowedLine(plotBackground, origin, cv::Point(plotXCenter, plotYMin), cv::Scalar(255, 255, 255), 1, cv::LINE_8, 0, 0.025);
		cv::arrowedLine(plotBackground, origin, cv::Point(plotXCenter, plotYMax), cv::Scalar(255, 255, 255), 1, cv::LINE_8, 0, 0.025);

		cv::putText(plotBackground, "x", cv::Point(plotXMax + 10, plotYCenter), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
		cv::putText(plotBackground, "y", cv::Point(plotXCenter, plotYMin - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);

		cv::circle(plotBackground, cv::Point(plotXMax - 5, plotYCenter), 2, cv::Scalar(0, 0, 255), -1);
		cv::putText(plotBackground, "(1,0)", cv::Point(plotXMax - 15, plotYCenter + 15), cv::FONT_HERSHEY_SIMPLEX, 0.25, cv::Scalar(0, 0, 255), 1);
		cv::circle(plotBackground, cv::Point(plotXCenter, plotYMin + 5), 2, cv::Scalar(0, 0, 255), -1);
		cv::putText(plotBackground, "(0,1)", cv::Point(plotXCenter + 5, plotYMin + 5), cv::FONT_HERSHEY_SIMPLEX, 0.25, cv::Scalar(0, 0, 255), 1);
		cv::circle(plotBackground, cv::Point(plotXMin + 5, plotYCenter), 2, cv::Scalar(0, 0, 255), -1);
		cv::putText(plotBackground, "(-1,0)", cv::Point(plotXMin + 15, plotYCenter + 15), cv::FONT_HERSHEY_SIMPLEX, 0.25, cv::Scalar(0, 0, 255), 1);
		cv::circle(plotBackground, cv::Point(plotXCenter, plotYMax - 5), 2, cv::Scalar(0, 0, 255), -1);
		cv::putText(plotBackground, "(0,-1)", cv::Point(plotXCenter + 5, plotYMax - 5), cv::FONT_HERSHEY_SIMPLEX, 0.25, cv::Scalar(0, 0, 255), 1);

	}
	else {

		//cv::rectangle(plotBackground, cv::Point(plotPadding, plotPadding), cv::Point(plotXMax, plotYMax), cv::Scalar(255, 255, 255), 1,cv::LINE_4);
		origin = cv::Point(plotXMin, plotYMax);
		cv::arrowedLine(plotBackground, origin, cv::Point(plotXMax, plotYMax), cv::Scalar(255, 255, 255), 1, cv::LINE_8, 0, 0.025);
		cv::arrowedLine(plotBackground, origin, cv::Point(plotXMin, plotYMin), cv::Scalar(255, 255, 255), 1, cv::LINE_8, 0, 0.025);

		cv::putText(plotBackground, "x", cv::Point(plotXMax + 10, plotYMax), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
		cv::putText(plotBackground, "y", cv::Point(plotXMin, plotYMin - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);

		cv::circle(plotBackground, cv::Point(plotXMax - 5, plotYMax), 2, cv::Scalar(0, 0, 255), -1);
		cv::putText(plotBackground, "(1,0)", cv::Point(plotXMax - 15, plotYMax + 15), cv::FONT_HERSHEY_SIMPLEX, 0.25, cv::Scalar(0, 0, 255), 1);
		cv::circle(plotBackground, cv::Point(plotXMin, plotYMin + 5), 2, cv::Scalar(0, 0, 255), -1);
		cv::putText(plotBackground, "(0,1)", cv::Point(plotXMin + 5, plotYMin + 5), cv::FONT_HERSHEY_SIMPLEX, 0.25, cv::Scalar(0, 0, 255), 1);


	}
}

void utils::PlotLib::transform(std::vector<std::vector<float>>& points) {

	if (show_negative) {

		//normalize values between -1 and 1 between plotMin and plotMax

		for (auto& point : points) {
			std::cout << point[0] << " " << point[1] << std::endl;
			point[0] = (point[0] * (plotXMax - plotXMin) / 2) + plotXCenter;
			point[1] = ((point[1] * (plotYMin - plotYMax) / 2) + plotYCenter);
			std::cout << point[0] << " " << point[1] << std::endl;
		}


	}
	else {
		for (auto& point : points) {
			std::cout << point[0] << " " << point[1] << std::endl;
			point[0] = plotXMin + (point[0] * (plotXMax - plotXMin));
			point[1] = plotYMax - (point[1] * (plotYMax - plotYMin));
			std::cout << point[0] << " " << point[1] << std::endl;
		}
	}

}
void utils::PlotLib::plot(std::vector<std::vector<float>>& points, PlotType type, const cv::Scalar& color, int thickness) {

	transform(points);


	for (auto iter = points.begin(); iter != points.end(); ++iter) {
		const auto& point = *iter;
		cv::Point p{ (int)point[0],(int)point[1] };


		switch (type) {
		case PlotType::dot:
			cv::circle(plotBackground, p, thickness, color, -1);
			break;
		case PlotType::line:
			if (std::next(iter) != points.end()) {
				cv::line(plotBackground, p,
					cv::Point(points[iter - points.begin() + 1][0], points[iter - points.begin() + 1][1]), color,
					thickness);
			}
			break;
			/*case PlotType::dot_line:
			cv::circle(plotBackground, p, thickness * 2.5f, color, -1);
			if (&point != &points.back()) {
				cv::line(plotBackground, p,
					cv::Point(points[&point - &points[0] + 1].x + origin.x - plotPadding / 2,
						origin.y - points[&point - &points[0] + 1].y + plotPadding / 2), color,
					thickness);
			}
			break;


		case PlotType::dot_line_fill:
			cv::circle(plotBackground, p, thickness * 2.5f, color, -1);
			if (&point != &points.back()) {
				cv::line(plotBackground, p,
					cv::Point(points[&point - &points[0] + 1].x + origin.x - plotPadding / 2,
						origin.y - points[&point - &points[0] + 1].y + plotPadding / 2), color,
					thickness);
			}
			if (&point == &points.back()) {
				cv::fillConvexPoly(plotBackground, points, color);
			}
			break;
		case PlotType::dot_fill:
			cv::circle(plotBackground, p, thickness * 2.5f, color, -1);
			break;
		case PlotType::line_fill:
			if (&point != &points.back()) {
				cv::line(plotBackground, p,
					cv::Point(points[&point - &points[0] + 1].x + origin.x - plotPadding / 2,
						origin.y - points[&point - &points[0] + 1].y + plotPadding / 2), color,
					thickness);
			}
			if (&point == &points.back()) {
				cv::fillConvexPoly(plotBackground, points, color);
			}
			break;*/
		default:
			break;
		}
	}
}
void utils::PlotLib::plot_show() {
	cv::imshow(window_name, plotBackground);
	cv::waitKey(0);
}

