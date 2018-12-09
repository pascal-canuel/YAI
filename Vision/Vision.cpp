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
#include <iostream>
#include <chrono>
#include <random>

#include "MNIST.h"

using namespace cv;
using namespace Eigen;

// member variables
std::vector<Mat> train_images;
std::vector<int> train_labels(60000);

std::vector<Mat> test_images;
std::vector<int> test_labels(10000);

std::vector<MatrixXf> weights;
std::vector<VectorXf> biases;

// hyper-parameters
int epoch = 30;
int batch_size = 10;
float learning_rate = 1.0f;

VectorXf Vectorize_image(cv::Mat image)
{
	Map<Matrix<float, Dynamic, Dynamic, RowMajor>> mat(reinterpret_cast<float*>(image.data), image.rows, image.cols * image.channels());
	Map<RowVectorXf> vectorized_image(mat.data(), mat.size());

	return vectorized_image;
}

VectorXf Vectorize_label(int label)
{
	VectorXf vectorized_label = VectorXf::Zero(10);
	vectorized_label(label) = 1;

	return vectorized_label;
}

VectorXf Mean_squared_error(Eigen::VectorXf y, Eigen::VectorXf a)
{
	return (y.array() - a.array()).pow(2) / 2.0;
}

VectorXf Mean_squared_error_prime(Eigen::VectorXf y, Eigen::VectorXf a)
{
	return a.array() - y.array();
}

VectorXf Sigmoid(VectorXf z)
{
	return 1.0 / (1 + exp(-1 * z.array()));
}

VectorXf Sigmoid_prime(VectorXf z)
{
	return Sigmoid(z).array() * (1 - Sigmoid(z).array());
}

VectorXf Forward(VectorXf image)
{
	VectorXf a = image;
	for (int i = 0; i < 2; i++)
	{
		a = Sigmoid(weights[i] * a + biases[i]);
	}

	return a;
}

template<typename T>
std::vector<T> slice(std::vector<T> const &v, int m, int n)
{
	auto first = v.cbegin() + m;
	auto last = v.cbegin() + n + 1;

	std::vector<T> vec(first, last);
	return vec;
}

std::tuple<std::vector<MatrixXf>, std::vector<VectorXf>> Backpropagation(VectorXf image, int label)
{
	std::vector<VectorXf> activations;
	activations.push_back(image);

	std::vector<VectorXf> zs;

	// forward pass
	VectorXf a = image;
	for (int i = 0; i < 2; i++)
	{
		VectorXf z = weights[i] * a + biases[i];
		a = Sigmoid(z);

		zs.push_back(z);
		activations.push_back(a);
	}

	std::vector<MatrixXf> delta_weights;
	std::vector<VectorXf> delta_biases;

	// error on last layer
	VectorXf delta = Mean_squared_error_prime(Vectorize_label(label), activations.back()).array() * Sigmoid_prime(zs.back()).array();
	delta_biases.push_back(delta);
	delta_weights.push_back(delta * activations[1].transpose());

	// error on hidden layer
	VectorXf sp = Sigmoid_prime(zs[0]);
	delta = (weights[1].transpose() * delta).array() * sp.array();

	delta_biases.insert(delta_biases.begin(), delta);
	delta_weights.insert(delta_weights.begin(), delta * activations[0].transpose());

	return { delta_weights, delta_biases };
}

void Update_batch(std::vector<Mat> images, std::vector<int> labels)
{
	std::vector<MatrixXf> sum_delta_weights;
	std::vector<VectorXf> sum_delta_biases;

	// layer 1
	sum_delta_weights.push_back(MatrixXf::Zero(30, 784));
	sum_delta_biases.push_back(VectorXf::Zero(30));

	// layer 2
	sum_delta_weights.push_back(MatrixXf::Zero(10, 30));
	sum_delta_biases.push_back(VectorXf::Zero(10));

	for (int i = 0; i < batch_size; i++)
	{
		auto weights_biases_error = Backpropagation(Vectorize_image(images[i]), labels[i]);

		for (int l = 0; l < 2; l++)
		{
			sum_delta_weights[l].array() += std::get<0>(weights_biases_error)[l].array();
			sum_delta_biases[l].array() += std::get<1>(weights_biases_error)[l].array();
		}
	}

	for (int i = 0; i < 2; i++)
	{
		weights[i].array() -= (learning_rate / batch_size) * sum_delta_weights[i].array();
		biases[i].array() -= (learning_rate / batch_size) * sum_delta_biases[i].array();
	}
}

std::tuple <int, float> Evaluate()
{
	int result = 0;
	float sum_error = 0;
	for (int i = 0; i < 10000; i++)
	{
		VectorXf image = Vectorize_image(test_images[i]);
		VectorXf a = Forward(image);

		float max_value = a(0);
		int max_index = 0;
		for (int v = 1; v < 10; v++)
		{
			if (max_value < a(v))
			{
				max_value = a(v);
				max_index = v;
			}
		}

		if (max_index == test_labels[i])
			result++;
		
		sum_error += Mean_squared_error(Vectorize_label(test_labels[i]), a).sum();
	}

	return { result, sum_error };
}

int main()
{
	// load training and test data
	MNIST::ReadImages("../MNIST/train-images.idx3-ubyte", train_images);	
	MNIST::ReadLabels("../MNIST/train-labels.idx1-ubyte", train_labels);
	
	MNIST::ReadImages("../MNIST/t10k-images.idx3-ubyte", test_images);	
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
		auto evaluation = Evaluate();
		std::cout << "Epoch " << e << " Acc: " << std::get<0>(evaluation) << " / 10000  Loss: " << std::get<1>(evaluation) <<  std::endl;
	}

	waitKey(0);
	return 0;
}