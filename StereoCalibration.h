#pragma once
#include "opencv2\opencv.hpp"
#include <time.h>
#include <iostream>

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
	void showImage(cv::Mat image, bool waitForKey);
	void showImage(char* windowName, cv::Mat image, bool waitForKey);
	int runStereoCalibration();

	inline void timerStart() { timer = (double)cv::getTickCount(); };
	inline double timerElapsed() { return ((double)cv::getTickCount() - timer) / cv::getTickFrequency(); };

	bool camsOpened;
	int samplesRequired;
	int samples;
	double timer;
	double error_rms;
	cv::VideoCapture leftCam, rightCam;
	vector<vector<cv::Point2f>> leftImagePoints, rightImagePoints;
	cv::Mat leftCameraMat, leftCameraDistorsion, rightCameraMat, rightCameraDistorsion;
	cv::Mat rotationMat, translationMat, essentialMat, fundamentalMat,
		leftRectificationMat, leftProjectionMat,
		rightRectificationMat, rightProjectionMat;
	cv::Mat disparityToDepthMat;
	cv::Mat leftFrame, rightFrame;
	cv::Rect leftValidPixROI, rightValidPixROI;
	cv::Size imageSize;
	cv::Size chessboardSize;
	int squareSize;

};

