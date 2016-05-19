#pragma once
#include "opencv2\opencv.hpp"
#include <time.h>
#include <iostream>

#define RGB 1
#define HSV 2
#define KEY_y 121
#define KEY_n 110
#define KEY_1 49
#define KEY_2 50

using namespace std;

class CStereoCalibration
{
public:
	CStereoCalibration();
	CStereoCalibration(cv::Size chessboardSize, int squareSize, int samplesRequired);
	~CStereoCalibration();

	vector<vector<cv::Point3f>> calcObjectPoints(int imagesNumber);
	int getCalibImagePoints(vector<cv::Mat>& frames, int delay);
	int openCameras(int leftCamID, int rightCamID);
	int closeCameras();
	void saveSettings(char* path);
	void saveFilterParameters(char* path, int method, vector<int> mins, vector<int> maxs);
	void showImage(cv::Mat image, bool waitForKey);
	void showImage(char* windowName, cv::Mat image, bool waitForKey);
	int runStereoCalibration();
	int runFilterCalibration();

	inline void timerStart() { timer = (double)cv::getTickCount(); };
	inline double timerElapsed() { return ((double)cv::getTickCount() - timer) / cv::getTickFrequency(); };

	bool camsOpened;
	int samplesRequired;
	double timer;
	double error_rms;
	cv::VideoCapture leftCam, rightCam;
	vector<cv::Mat> leftCalibFrames, rightCalibFrames;
	vector<vector<cv::Point2f>> leftImagePoints, rightImagePoints;
	cv::Mat leftCameraMat, leftCameraDistorsion, rightCameraMat, rightCameraDistorsion;
	cv::Mat rotationMat, translationMat, essentialMat, fundamentalMat,
		leftRectificationMat, leftProjectionMat,
		rightRectificationMat, rightProjectionMat;
	cv::Mat disparityToDepthMat;
	cv::Mat leftFrame, rightFrame;
	cv::Mat leftFilteredFrame, rightFilteredFrame;
	cv::Mat leftTransformedFrame, rightTransformedFrame;
	cv::Rect leftValidPixROI, rightValidPixROI;
	cv::Size imageSize;
	cv::Size chessboardSize;
	int squareSize;

};

