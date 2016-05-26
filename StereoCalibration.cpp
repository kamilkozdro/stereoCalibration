#include "StereoCalibration.h"

using namespace cv;

CStereoCalibration::CStereoCalibration()
{
	chessboardSize.width = 9;
	chessboardSize.height = 6;
	squareSize = 25;
	timer = 0;
	samplesRequired = 20;
}

CStereoCalibration::CStereoCalibration(Size ChessboardSize, int SquareSize, int SamplesRequired)
{
	chessboardSize = ChessboardSize;
	squareSize = SquareSize;
	timer = 0;
	samplesRequired = SamplesRequired;
}

CStereoCalibration::~CStereoCalibration()
{
}

vector<vector<Point3f>> CStereoCalibration::calcObjectPoints(int imagesNumber)
{
	vector<vector<Point3f>> objectPoints;

	objectPoints.resize(imagesNumber);
	// zalozenie: wszystkie pola w osi Z = 0;
	for (int i = 0; i < imagesNumber; i++)
	{
		for (int j = 0; j < chessboardSize.height; j++)
		{ 
			for (int k = 0; k < chessboardSize.width; k++)
				objectPoints[i].push_back(Point3f(float(k*squareSize), float(j*squareSize), 0));
		}		
	}

	return objectPoints;
}

int CStereoCalibration::getCalibImagePoints(vector<Mat>& frames, int delay = 2)
{
	bool leftFound, rightFound;
	vector<Point2f>leftImagePointsBuffer, rightImagePointsBuffer;

	for (int i = 0; i < frames.size(); i++)
	{
		leftFound = findChessboardCorners(frames[i], chessboardSize, leftImagePointsBuffer,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS | CV_CALIB_CB_FAST_CHECK);//CV_CALIB_CB_NORMALIZE_IMAGE );
		drawChessboardCorners(frames[i], chessboardSize, leftImagePointsBuffer, leftFound);
		showImage("leftCam", frames[i], false);
		rightFound = findChessboardCorners(frames[++i], chessboardSize, rightImagePointsBuffer,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS | CV_CALIB_CB_FAST_CHECK);//CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FAST_CHECK);
		drawChessboardCorners(frames[i], chessboardSize, rightImagePointsBuffer, rightFound);
		showImage("rightCam", frames[i], false);
	
		if (frames[i - 1].size() != frames[i].size())
		{
			cout << "Rozne rozmiary obrazow!" << endl;
			return 0;
		}
				
		if (rightFound && leftFound)
		{
			if (timerElapsed() >= delay || timer == 0)
			{
				/*
				Mat grayFrame;
				cvtColor(frames[0], grayFrame, CV_BGR2GRAY);
				cornerSubPix(grayFrame, leftImagePointsBuffer, Size(11, 11),
					Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
				cvtColor(frames[1], grayFrame, CV_BGR2GRAY);
				cornerSubPix(grayFrame, rightImagePointsBuffer, Size(11, 11),
					Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
					*/
				leftImagePoints.push_back(leftImagePointsBuffer);
				leftCalibFrames.push_back(frames[i - 1]);
				rightImagePoints.push_back(rightImagePointsBuffer);
				rightCalibFrames.push_back(frames[i]);
				std::cout << "PROBKI: " << leftCalibFrames.size() << endl;
				timerStart();
			}
		}
		leftImagePointsBuffer.clear();
		rightImagePointsBuffer.clear();
	}

	return 1;
}

int CStereoCalibration::openCameras(int leftCamID, int rightCamID)
{
	leftCam.open(leftCamID);
	if (!leftCam.isOpened())
	{
		cout << "Nie mozna uruchomic kamery ID:" << leftCamID << endl;
		return 0;
	}
	rightCam.open(rightCamID);
	if (!rightCam.isOpened())
	{
		cout << "Nie mozna uruchomic kamery ID:" << rightCamID << endl;
		return 0;
	}

	camsOpened = true;
	return 1;
}

int CStereoCalibration::closeCameras()
{
	if (leftCam.isOpened())
		leftCam.release();
	if (rightCam.isOpened())
		rightCam.release();
	camsOpened = false;

	return 1;
}

void CStereoCalibration::saveSettings(char* path)
{
	FileStorage fileStream;
	time_t actualTime;

	fileStream.open(path, FileStorage::WRITE);
	time(&actualTime);
	fileStream << "calibrationDate" << asctime(localtime(&actualTime));
	fileStream << "leftCameraMat" << leftCameraMat;
	fileStream << "leftCameraDistorsion" << leftCameraDistorsion;
	fileStream << "rightCameraMat" << rightCameraMat;
	fileStream << "rightCameraDistorsion" << rightCameraDistorsion;
	fileStream << "rotationMat" << rotationMat;
	fileStream << "translationMat" << translationMat;
	fileStream << "leftRectificationMat" << leftRectificationMat;
	fileStream << "leftProjectionMat" << leftProjectionMat;
	fileStream << "rightRectificationMat" << rightRectificationMat;
	fileStream << "rightProjectionMat" << rightProjectionMat;
	fileStream << "disparity2DepthMat" << disparityToDepthMat;
	fileStream << "leftValidPixROI" << leftValidPixROI;
	fileStream << "rightValidPixROI" << rightValidPixROI;
	fileStream << "imageSize" << imageSize;
	fileStream << "errorRMS" << error_rms;
	fileStream.release();
}

void CStereoCalibration::showImage(Mat image, bool waitForKey = false)
{
	namedWindow("window");
	imshow("window", image);
	if (waitForKey)
		waitKey();
	destroyWindow("window");
}

void CStereoCalibration::showImage(char* windowName, Mat image, bool waitForKey = false)
{
	imshow(windowName, image);
	if (waitForKey)
		waitKey();
}

int CStereoCalibration::runStereoCalibration()
{

	if (!camsOpened)
		return 0;

	namedWindow("leftCam");
	namedWindow("rightCam");
	//string left = "C:/Users/Hp/Desktop/Air/praca mgr/testy/kalibracja/left";
	//string right = "C:/Users/Hp/Desktop/Air/praca mgr/testy/kalibracja/right";
	//string format = ".jpg";
	//int i = 1;
	vector<Mat> frames(2);
	while (leftCalibFrames.size() < samplesRequired)	//
	{
		waitKey(1);	// inaczej nie wyswietla podgladu
		//cout << "i = " << i << endl;
		leftCam >> frames[0];//frames[0] = imread(left + std::to_string(i) + format);	//
		rightCam >> frames[1];//frames[1] = imread(right + std::to_string(i) + format);	//
		getCalibImagePoints(frames,5);
		//++i;
	}

	imageSize = leftCalibFrames[0].size();
	vector<vector<Point3f>> objectPoints;
	objectPoints = calcObjectPoints(leftCalibFrames.size());
	leftCameraMat = initCameraMatrix2D(objectPoints, leftImagePoints, imageSize, 0);
	rightCameraMat = initCameraMatrix2D(objectPoints, rightImagePoints, imageSize, 0);

	double timerCalibrate = (double)getTickCount();

	error_rms = stereoCalibrate(objectPoints, leftImagePoints, rightImagePoints,
		leftCameraMat, leftCameraDistorsion, rightCameraMat, rightCameraDistorsion,
		imageSize, rotationMat, translationMat, essentialMat, fundamentalMat,
		CALIB_ZERO_TANGENT_DIST +
		CALIB_FIX_FOCAL_LENGTH +
		CALIB_FIX_ASPECT_RATIO +
		CALIB_SAME_FOCAL_LENGTH,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 100, 1e-5));
	stereoRectify(leftCameraMat, leftCameraDistorsion, rightCameraMat, rightCameraDistorsion,
		imageSize, rotationMat, translationMat,
		leftRectificationMat, rightRectificationMat, 
		leftProjectionMat, rightProjectionMat, disparityToDepthMat, 0, -1, imageSize, &leftValidPixROI, &rightValidPixROI);

	std::cout << "ZAKONCZONO KALIBRACJE!\nBLAD RMS = " << error_rms << endl;
	std::cout << "CZAS KALIBRACJI DLA " << samplesRequired << " PROBEK WYNIOSL: " <<
		((double)cv::getTickCount() - timerCalibrate) / cv::getTickFrequency() << endl << endl;

	return 1;
}
