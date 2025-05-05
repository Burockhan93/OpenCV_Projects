#include "SingleLayerNN.h"
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include "..\Utils\plotlib.h"

//plot class will be used to plot the data, need to create a class for it

const std::string windowName_test = "Test Data";
const std::string windowName_result = "Result";

void testNeuralNetwork() {


	//inputs are 2d vectors, output will classify them into 0 or 1(binary)
	SingleLayerNN nn(2, 1);
	//test values
	nn.setLearningRate(0.01);
	nn.setEpochs(1000);
	nn.setBatchSize(1);
	nn.setMomentum(0.9);
	nn.setRegularization(0.01);
	// Set activation function to sigmoid, since its a binary problem
	nn.setActivationFunction([](float x) { return 1 / (1 + exp(-x)); });
	nn.setActivationFunctionDerivative([](float x) { return x * (1 - x); });
	// Set loss function to mean squared error, binary problem
	nn.setLossFunction([](const std::vector<float>& output, const std::vector<float>& target) {

		float loss = 0;
		for (size_t i = 0; i < output.size(); ++i) {

			loss += pow(output[i] - target[i], 2);
		}

		return loss / output.size();
		});

	nn.setLossFunctionDerivative([](const std::vector<float>& output, const std::vector<float>& target) {
		std::vector<float> gradients(output.size());
		for (size_t i = 0; i < output.size(); ++i) {
			gradients[i] = 2 * (output[i] - target[i]);
		}
		std::transform(gradients.begin(), gradients.end(), gradients.begin(), [output](float i) {return i / output.size(); });
		return gradients;
		});
	std::vector<std::vector<float>> inputs(10000);
	std::vector<std::vector<float>> targets(10000);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
	for (size_t i = 0; i < 10000; i++)
	{
		float x = dis(gen);
		float y = dis(gen);
		inputs[i] = { x, y };
		targets[i] = { x > y ? 1.0f : 0.0f };
	}
	nn.train(inputs, targets);

	// Test the neural network with some inputs
	std::vector<std::vector<float>> testInputs(100);
	std::vector<float> predictedOutputs(101);
	cv::Mat test_image(400, 400, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat result_image(400, 400, CV_8UC3, cv::Scalar(0, 0, 0));
	for (size_t i = 0; i < 100; i++)
	{
		float x = dis(gen);
		float y = dis(gen);
		testInputs[i] = { x, y };
		cv::circle(test_image, cv::Point(x * 400, y * 400), 2, cv::Scalar(255, 255, 255), -1);
	}


	auto iter = predictedOutputs.begin();
	std::vector<std::vector<float>> output_1;
	std::vector<std::vector<float>> output_0;
	int i = 0;
	for (const auto& input : testInputs) {
		i++;
		
		auto output = nn.predict(input);
		if (iter != predictedOutputs.end()) {
			*iter = output[0];
			iter++;
		}
		
		std::cout << "Input: ";
		for (const auto& val : input) {
			std::cout << val << " ";
		}
		std::cout << "Output: ";
		for (const auto& val : output) {
			std::cout << val << " ";
		}

		output[0] > 0.5f ? output_1.push_back(input):output_0.push_back(input);
		
	}

	using namespace utils;
	PlotLib plot(800, 800,true);
	output_0.shrink_to_fit();
	output_1.shrink_to_fit();
	plot.plot(output_0, utils::PlotType::dot, cv::Scalar(0, 255, 0));
	plot.plot(output_1, utils::PlotType::dot, cv::Scalar(255, 0, 0));

	plot.plot_show();

	cv::destroyAllWindows();
}