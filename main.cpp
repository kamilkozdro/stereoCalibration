#include "StereoCalibration.h"
#include "opencv2\opencv.hpp"

int main()
{
	
	cv::Size chessBoardSize(9, 6);
	CStereoCalibration stereoCalib(chessBoardSize, 25, 25);
	//cv::namedWindow("okno");
	//cv::namedWindow("okno1");

	stereoCalib.openCameras(2, 1);
	/*
	while (cv::waitKey(5) == -1)
	{
		stereoCalib.leftCam >> stereoCalib.leftFrame;
		stereoCalib.rightCam >> stereoCalib.rightFrame;
		stereoCalib.showImage("okno", stereoCalib.leftFrame, false);
		//stereoCalib.showImage("okno1", stereoCalib.rightFrame, false);
	}	
	*/
	//stereoCalib.runStereoCalibration();
	stereoCalib.runFilterCalibration();
	//stereoCalib.saveSettings("calibrationParameters.xml");
	stereoCalib.closeCameras();
	
	//cv::waitKey();

	return 1;
}