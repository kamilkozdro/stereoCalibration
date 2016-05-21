#pragma once
#include "opencv2\opencv.hpp"
#include <time.h>
#include <iostream>

#define RGB 1
#define HSV 2

using namespace std;

class CFilterCalibration
{
public:
	CFilterCalibration();
	~CFilterCalibration();

	int openCamera(int camID);
	int closeCamera();
	void saveFilterParameters(char* path, int method);
	int runFilterCalibration(int method);

	bool camOpened;
	int min[3];
	int max[3];
	cv::VideoCapture cam;
};

