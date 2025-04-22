#pragma once
#include <vector>
#include <functional>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

class SingleLayerNN {

private:
	int inputSize{0};
	int outputSize{0};
	std::vector<float> weights;
	std::vector<float> biases;
	float learningRate;
	int epochs;
	int batchSize;
	float momentum;
	float regularization;
	// Activation function and its derivative
	std::function<float(float)> activationFunction;
	std::function<float(float)> activationFunctionDerivative;
	// Loss function and its derivative
	std::function<float(const std::vector<float>&, const std::vector<float>&)> lossFunction;
	std::function<std::vector<float>(const std::vector<float>&, const std::vector<float>&)> lossFunctionDerivative;

public:
	SingleLayerNN(int inputSize, int outputSize);
	void train(const std::vector<std::vector<float>>& inputs, const std::vector<std::vector<float>>& targets);
	std::vector<float> predict(const std::vector<float>& input);
	void setLearningRate(float learningRate);
	void setEpochs(int epochs);
	void setBatchSize(int batchSize);
	void setMomentum(float momentum);
	void setRegularization(float regularization);
	void setActivationFunction(std::function<float(float)> activationFunction);
	void setActivationFunctionDerivative(std::function<float(float)> activationFunctionDerivative);
	void setLossFunction(std::function<float(const std::vector<float>&, const std::vector<float>&)> lossFunction);
	void setLossFunctionDerivative(std::function<std::vector<float>(const std::vector<float>&, const std::vector<float>&)> lossFunctionDerivative);
};