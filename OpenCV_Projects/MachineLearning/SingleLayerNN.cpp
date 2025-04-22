#include "SingleLayerNN.h"

SingleLayerNN::SingleLayerNN(int inputSize, int outputSize) {
	std::cout << "SingleLayerNN constructor called\n";
	this->inputSize = inputSize;
	this->outputSize = outputSize;
}
void SingleLayerNN::train(const std::vector<std::vector<float>>& inputs, const std::vector<std::vector<float>>& targets) {

	// Initialize weights and biases
	int inputSize = inputs[0].size();
	int outputSize = targets[0].size();
	weights.resize(inputSize * outputSize);
	biases.resize(outputSize);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
	for (int i = 0; i < weights.size(); ++i) {
		weights[i] = dis(gen);
	}
	for (int i = 0; i < biases.size(); ++i) {
		biases[i] = dis(gen);
	}
	for (int epoch = 0; epoch < epochs; ++epoch) {
		for (size_t i = 0; i < inputs.size(); ++i) {
			const std::vector<float>& input = inputs[i];
			const std::vector<float>& target = targets[i];
			// Forward pass,
			//zero initalize output
			std::vector<float> output(outputSize, 0.0f);
			for (int j = 0; j < outputSize; ++j) {
				for (int k = 0; k < inputSize; ++k) {
					output[j] += input[k] * weights[j * inputSize + k];
				}
				output[j] += biases[j];
				output[j] = activationFunction(output[j]);
			}
			// Backward pass, w^jk <- (w^jk -n* sigmoid^j *x^k = weight update
			//zero initalize lossgradient vector
			std::vector<float> lossGradients(outputSize, 0.0f);
			for (int j = 0; j < outputSize; ++j) {
				lossGradients[j] = lossFunctionDerivative(output, target)[j];
			}
			for (int j = 0; j < outputSize; ++j) {
				float sigmoid = activationFunctionDerivative(output[j]);
				float lossGradient = lossGradients[j];
				for (int k = 0; k < inputSize; ++k) {
					weights[j * inputSize + k] -= learningRate * lossGradient * sigmoid * input[k];
				}
				biases[j] -= learningRate * lossGradient * sigmoid;
			}

		}
		// Print loss every 100 epochs
		if (epoch % 100 == 0) {
			float totalLoss = 0.0f;
			for (int i = 0; i < inputs.size(); i++) {
				totalLoss += lossFunction(predict(inputs[i]), targets[i]);
			}
			totalLoss /= inputs.size();
			std::cout << "Epoch: " << epoch << ", Loss: " << totalLoss << std::endl;
		}


	}
}
std::vector<float> SingleLayerNN::predict(const std::vector<float>& input) {

	std::vector<float> output(biases.size(), 0.0f);
	for (size_t i = 0; i < output.size(); i++)
	{
		for (size_t j = 0; j < input.size(); j++)
		{
			output[i] += input[j] * weights[i * input.size() + j];
		}
		output[i] += biases[i];
		output[i] = activationFunction(output[i]);

	}
	return output;
}
void SingleLayerNN::setBatchSize(int batchSize_) {
	this->batchSize = batchSize_;
}
void SingleLayerNN::setMomentum(float momentum_) {
	this->momentum = momentum_;
}
void SingleLayerNN::setRegularization(float regularization_) {
	this->regularization = regularization_;
}
void SingleLayerNN::setActivationFunction(std::function<float(float)> activationFunction_) {
	this->activationFunction = activationFunction_;
}
void SingleLayerNN::setActivationFunctionDerivative(std::function<float(float)> activationFunctionDerivative_) {
	this->activationFunctionDerivative = activationFunctionDerivative_;
}
void SingleLayerNN::setLossFunction(std::function<float(const std::vector<float>&, const std::vector<float>&)> lossFunction_) {
	this->lossFunction = lossFunction_;
}
void SingleLayerNN::setLossFunctionDerivative(std::function<std::vector<float>(const std::vector<float>&, const std::vector<float>&)> lossFunctionDerivative_) {
	this->lossFunctionDerivative = lossFunctionDerivative_;
}


void SingleLayerNN::setLearningRate(float learningRate_) {
	this->learningRate = learningRate_;
}
void SingleLayerNN::setEpochs(int epochs_) {
	this->epochs = epochs_;
}
