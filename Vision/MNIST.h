#pragma once
#include "AllIncludes.h"

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
