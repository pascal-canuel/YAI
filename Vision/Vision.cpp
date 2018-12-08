/*
YAI: Yet Another AI (from scratch)
By: Pascal Canuel
*/

// opencv
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

// eigen
#include <iostream>
#include <Eigen/Dense>

// others
#include "MNIST.h"

using namespace cv;
using namespace Eigen;

// member variables
std::vector<MatrixXf> weights;
std::vector<VectorXf> biases;

// hyper-parameters
int epoch = 30;
int batch_size = 10;
float learning_rate = 1.0f;

template<typename T>
std::vector<T> slice(std::vector<T> const &v, int m, int n)
{
	auto first = v.cbegin() + m;
	auto last = v.cbegin() + n + 1;

	std::vector<T> vec(first, last);
	return vec;
}

std::tuple<std::vector<MatrixXf>, std::vector<VectorXf>> Backpropagation(Mat image, int label)
{

}

void Update_batch(std::vector<Mat> images, std::vector<int> labels)
{
	std::vector<MatrixXf> sum_weights_error;
	std::vector<VectorXf> sum_biases_error;

	// layer 1
	sum_weights_error.push_back(MatrixXf::Zero(30, 784));
	sum_biases_error.push_back(VectorXf::Zero(30));

	// layer 2
	sum_weights_error.push_back(MatrixXf::Zero(10, 30));
	sum_biases_error.push_back(VectorXf::Zero(10));

	for (int i = 0; i < batch_size; i++)
	{
		auto weights_biases_error = Backpropagation(images[i], labels[i]);

		for (int l = 0; l < 2; l++)
		{
			sum_weights_error[l].array() += std::get<0>(weights_biases_error)[l].array();
			sum_biases_error[l].array() += std::get<1>(weights_biases_error)[l].array();
		}
	}

	for (int i = 0; i < 2; i++)
	{
		weights[i].array() -= (learning_rate / batch_size) * sum_weights_error[i].array();
		biases[i].array() -= (learning_rate / batch_size) * sum_biases_error[i].array();
	}
}

int Evaluate()
{
	for (int i = 0; i < 10000; i++)
	{
		
	}
}

int main()
{
	// load training and test data
	std::vector<Mat> train_images;
	MNIST::ReadImages("../MNIST/train-images.idx3-ubyte", train_images);
	std::vector<int> train_labels(60000);
	MNIST::ReadLabels("../MNIST/train-labels.idx1-ubyte", train_labels);

	std::vector<Mat> test_images;
	MNIST::ReadImages("../MNIST/t10k-images.idx3-ubyte", test_images);
	std::vector<int> test_labels(10000);
	MNIST::ReadLabels("../MNIST/t10k-labels.idx1-ubyte", test_labels);
	
	// initialize weights and biases with normal distribution
	std::default_random_engine generator;
	std::normal_distribution<float> distribution(0.0, 1.0);

	auto normal_bias = [&](float) {return distribution(generator); };

	// layer 2
	biases.push_back(VectorXf::NullaryExpr(30, normal_bias));
	auto normal_weight_l2 = [&](float) {return distribution(generator) / sqrt(784); };
	weights.push_back(MatrixXf::NullaryExpr(30, 784, normal_weight_l2));

	// layer 3
	biases.push_back(VectorXf::NullaryExpr(10, normal_bias));
	auto normal_weight_l3 = [&](float) {return distribution(generator) / sqrt(30); };
	weights.push_back(MatrixXf::NullaryExpr(10, 30, normal_weight_l3));

	// start training
	for (int e = 0; e < epoch; e++)
	{
		// shuffle training data at each epoch
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(train_images.begin(), train_images.end(), std::default_random_engine(seed));
		shuffle(train_labels.begin(), train_labels.end(), std::default_random_engine(seed));

		for (int b = 0; b < 6000; b++)
		{
			int i = b * 10;
			std::vector<Mat> batch_images = slice(train_images, i, i + 10);
			std::vector<int> batch_labels = slice(test_labels, i, i + 10);

			Update_batch(batch_images, batch_labels);
		}

		std::cout << "Epoch " << e << " Acc: " << Evaluate() << " / 10000" << std::endl;
	}

	waitKey(0);
	return 0;
}