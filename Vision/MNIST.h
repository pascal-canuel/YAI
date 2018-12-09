#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include <iostream>
#include <vector>

class MNIST
{
public:
	MNIST();
	~MNIST();
	static void ReadImages(std::string filename, std::vector<cv::Mat> &vec);
	static void ReadLabels(std::string filename, std::vector<int> &vec);
private:
	static int ReverseInt(int i);
};
