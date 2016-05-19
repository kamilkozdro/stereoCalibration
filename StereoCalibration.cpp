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
	// zalozenie: wszystkie pola w osi Z = 0; rownolegle do obiektywu
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
			imwrite("kalibracja.png", frames[0]);
		leftFound = findChessboardCorners(frames[i], chessboardSize, leftImagePointsBuffer,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FAST_CHECK);	//CV_CALIB_CB_FILTER_QUADS
		drawChessboardCorners(frames[i], chessboardSize, leftImagePointsBuffer, leftFound);
			imwrite("kalibracja_zaznacz.png", frames[0]);
		showImage("leftCam", frames[i], false);
		rightFound = findChessboardCorners(frames[++i], chessboardSize, rightImagePointsBuffer,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FAST_CHECK);	//CV_CALIB_CB_FILTER_QUADS
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
				Mat grayFrame;
				cvtColor(frames[0], grayFrame, CV_BGR2GRAY);
				cornerSubPix(grayFrame, leftImagePointsBuffer, Size(11, 11),
					Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
				cvtColor(frames[1], grayFrame, CV_BGR2GRAY);
				cornerSubPix(grayFrame, rightImagePointsBuffer, Size(11, 11),
					Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
				leftImagePoints.push_back(leftImagePointsBuffer);
				leftCalibFrames.push_back(frames[i - 1]);
				rightImagePoints.push_back(rightImagePointsBuffer);
				rightCalibFrames.push_back(frames[i]);
				std::cout << "FRAMES: " << leftCalibFrames.size() << endl;
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
		return 0;
	rightCam.open(rightCamID);
	if (!rightCam.isOpened())
		return 0;

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

void CStereoCalibration::saveFilterParameters(char* path, int method, vector<int> mins, vector<int> maxs)
{
	FileStorage fileStream;
	time_t actualTime;
	string minsString = "min";
	string maxsString = "max";

	fileStream.open(path, FileStorage::WRITE);
	time(&actualTime);
	fileStream << "Date" << asctime(localtime(&actualTime));
	if(method == RGB)
		fileStream << "method" << "RGB";
	else if(method == HSV)
		fileStream << "method" << "HSV";
	else
		fileStream << "method" << "unknown";
	fileStream << "min1" << mins[0];
	fileStream << "min2" << mins[1];
	fileStream << "min3" << mins[2];
	fileStream << "max1" << maxs[0];
	fileStream << "max2" << maxs[1];
	fileStream << "max3" << maxs[2];
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

	vector<Mat> frames(2);
	while (leftCalibFrames.size() < samplesRequired)
	{
		waitKey(1);	// inaczej nie wyswietla podgladu
		leftCam >> frames[0];
		rightCam >> frames[1];
		getCalibImagePoints(frames);
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
		CALIB_SAME_FOCAL_LENGTH,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 100, 1e-5));
	stereoRectify(leftCameraMat, leftCameraDistorsion, rightCameraMat, rightCameraDistorsion,
		imageSize, rotationMat, translationMat,
		leftRectificationMat, rightRectificationMat, 
		leftProjectionMat, rightProjectionMat, disparityToDepthMat, 0, -1, imageSize, &leftValidPixROI, &rightValidPixROI);

	std::cout << "CZAS KALIBRACJI DLA " << samplesRequired << " PROBEK WYNIOSL: " <<
		((double)cv::getTickCount() - timerCalibrate) / cv::getTickFrequency() << endl << endl;

	std::cout << "ZAKONCZONO KALIBRACJE!\nBLAD RMS = "<< error_rms << endl;

	return 1;
}

int CStereoCalibration::runFilterCalibration()
{
	if (!camsOpened)
		return -1;

	Mat frame, filteredFrame;

	vector<int> min(3,0);
	vector<int> max(3,255);
	char keyPressed = 0;
	int method = 0;

	namedWindow("cam");
	namedWindow("filtered");

	String trackbarNames[6];

	leftCam >> frame;
	putText(frame, "RGB/HSV? (1/2)", Point(frame.cols / 3, frame.rows / 2), 1, 3, CvScalar(0, 255, 0), 2);
	imshow("cam", frame);

	while (keyPressed != KEY_1 && keyPressed != KEY_2)
		keyPressed = waitKey();
	if (keyPressed == KEY_1)
		method = RGB;
	else if (keyPressed == KEY_2)
		method = HSV;
	else
		method = 0;

	std::cout << "metoda: " << method << endl;
	if (method == RGB)
	{
		trackbarNames[0] = "Bmin";
		trackbarNames[1] = "Gmin";
		trackbarNames[2] = "Rmin";
		trackbarNames[3] = "Bmax";
		trackbarNames[4] = "Gmax";
		trackbarNames[5] = "Rmax";
	}
	else if (method == HSV)
	{
		trackbarNames[0] = "Hmin";
		trackbarNames[1] = "Smin";
		trackbarNames[2] = "Vmin";
		trackbarNames[3] = "Hmax";
		trackbarNames[4] = "Smax";
		trackbarNames[5] = "Vmax";
	}
	else
		return 0;

	for (int i = 0; i < 3; i++)
	{
		createTrackbar(trackbarNames[i], "filtered", &min[i], 255);
		createTrackbar(trackbarNames[i+3], "filtered", &max[i], 255);
	}

	while (waitKey(20) == -1)
	{
		leftCam >> frame;
		imshow("cam", frame);

		if (method == HSV)
			cvtColor(frame, frame, CV_BGR2HSV);
		inRange(frame, Scalar(min[0], min[1], min[2]), Scalar(max[0], max[1], max[2]), filteredFrame);
		
		imshow("filtered", filteredFrame);
	}
	putText(frame, "SAVE? (y/n)", Point(frame.cols / 3, frame.rows / 2), 1, 3, CvScalar(0, 255, 0), 3);
	imshow("cam", frame);

	while (keyPressed != KEY_y && keyPressed != KEY_n)
		keyPressed = waitKey();
	if (keyPressed == KEY_y)
		saveFilterParameters("filterParameters.xml", method, min, max);

	return 1;
}