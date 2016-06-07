#include "FilterCalibration.h"

using namespace cv;

CFilterCalibration::CFilterCalibration()
{
	for (int i = 0; i < 3; i++)
	{
		min[i] = 0;
		max[i] = 255;
	}
}

CFilterCalibration::~CFilterCalibration()
{
}

int CFilterCalibration::openCamera(int camID)
{
	cam.open(camID);
	if (!cam.isOpened())
	{
		cout << "Nie mozna uruchomic kamery ID:" << camID << endl;
		return 0;
	}

	camOpened = true;
	return 1;
}

int CFilterCalibration::closeCamera()
{
	if (cam.isOpened())
		cam.release();
	camOpened = false;

	return 1;
}

void CFilterCalibration::saveFilterParameters(char* path, int method)
{
	FileStorage fileStream;
	time_t actualTime;
	string minsString = "min";
	string maxsString = "max";

	fileStream.open(path, FileStorage::WRITE);
	time(&actualTime);
	fileStream << "Date" << asctime(localtime(&actualTime));
	if (method == RGB)
		fileStream << "method" << RGB;
	else if (method == HSV)
		fileStream << "method" << HSV;
	else
		fileStream << "method" << "unknown";
	fileStream << "min1" << min[0];
	fileStream << "min2" << min[1];
	fileStream << "min3" << min[2];
	fileStream << "max1" << max[0];
	fileStream << "max2" << max[1];
	fileStream << "max3" << max[2];
}

int CFilterCalibration::runFilterCalibration(int method)
{
	if (!camOpened)
		return -1;

	Mat frame, filteredFrame;

	namedWindow("cam");
	namedWindow("filtered");

	String trackbarNames[6];

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
		createTrackbar(trackbarNames[i + 3], "filtered", &max[i], 255);
	}

	while (waitKey(5) == -1)
	{
		cam >> frame;
		imshow("cam", frame);

		if (method == HSV)
			cvtColor(frame, frame, CV_BGR2HSV);
		inRange(frame, Scalar(min[0], min[1], min[2]), Scalar(max[0], max[1], max[2]), filteredFrame);

		imshow("filtered", filteredFrame);
	}

	return 1;
}