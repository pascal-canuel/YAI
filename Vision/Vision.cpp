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

int main()
{
	// load training and test data
	std::vector<cv::Mat> train_images;
	MNIST::ReadImages("../MNIST/train-images.idx3-ubyte", train_images);
	std::vector<int> train_labels(60000);
	MNIST::ReadLabels("../MNIST/train-labels.idx1-ubyte", train_labels);

	std::vector<cv::Mat> test_images;
	MNIST::ReadImages("../MNIST/t10k-images.idx3-ubyte", test_images);
	std::vector<int> test_labels(10000);
	MNIST::ReadLabels("../MNIST/t10k-labels.idx1-ubyte", test_labels);
	
	
	waitKey(0);
	return 0;
}