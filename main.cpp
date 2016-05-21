#include "StereoCalibration.h"
#include "FilterCalibration.h"
#include "opencv2\opencv.hpp"

bool checkRange(int number, int min, int max)
{
	if (number >= min && number <= max)
		return true;
	else
	{
		cout << "Liczba musi sie zawierac w przedziale: <" << min << ";" << max << ">\n";
		return false;
	}
}

int inputNumber(int minNumber, int maxNumber)
{
	int number;
	do
	{
		cin >> number;
		while (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
			cout << "Zla liczba, wprowadz ponownie: ";
			cin >> number;
		}
	} while (!checkRange(number, minNumber, maxNumber));

	return number;
}

int main()
{
	int choice = 0;
	cout << "PROGRAM KALIBRUJACY SYSTEM WIZYJNY\n 1) Kalibracja kamer\n 2) Ustawienie parametrow filtrowania\n";
	choice = inputNumber(1,2);
	if (choice == 1)
	{
		cv::Size chessBoardSize; //(9,6)
		int squareSize, minSamples, leftCamID, rightCamID;

		cout << "Podaj szerokosc szachownicy: ";
		chessBoardSize.width = inputNumber(1, 100);
		cout << "Podaj wysokosc szachownicy: ";
		chessBoardSize.height = inputNumber(1, 100);
		cout << "Podaj dlugosc boku pojedynczego pola szachownicy [mm]: ";
		squareSize = inputNumber(1, 100);
		cout << "Podaj wymagana liczbe probek: ";
		minSamples = inputNumber(1, 40);

		CStereoCalibration stereoCalib(chessBoardSize, squareSize, minSamples);

		do
		{
			cout << "Podaj ID lewej kamery: ";
			leftCamID = inputNumber(0, 10);
			cout << "Podaj ID prawej kamery: ";
			rightCamID = inputNumber(0, 10);
		} while (stereoCalib.openCameras(leftCamID, rightCamID) == 0);

		cout << "Uruchamiana jest procedura kalibracji\n";

		stereoCalib.runStereoCalibration();

		cout << "Zapisac parametry do pliku? ( 1-TAK, 2-NIE)";
		choice = inputNumber(1, 2);
		if (choice == 1)
			stereoCalib.saveSettings("calibrationParameters.xml");
		stereoCalib.closeCameras();
	}
	else if(choice == 2)
	{
		CFilterCalibration filterCalib;
		int method, camID;

		do
		{
			cout << "Podaj ID kamery: ";
			camID = inputNumber(0, 10);
		} while (filterCalib.openCamera(camID) == 0);

		cout << "Podaj metode filtrowania:\n 1) RGB\n 2) HSV";
		method = inputNumber(1, 2);

		filterCalib.runFilterCalibration(method);
		cout << "Zapisac parametry do pliku? ( 1-TAK, 2-NIE)";
		choice = inputNumber(1, 2);
		if (choice == 1)
			filterCalib.saveFilterParameters("filterParameters.xml", method);
		filterCalib.closeCamera();
	}

	/*
	CStereoCalibration stereoCalib(chessBoardSize, 25, 25);
	//cv::namedWindow("okno");
	//cv::namedWindow("okno1");

	stereoCalib.openCameras(2, 1);
	
	while (cv::waitKey(5) == -1)
	{
		stereoCalib.leftCam >> stereoCalib.leftFrame;
		stereoCalib.rightCam >> stereoCalib.rightFrame;
		stereoCalib.showImage("okno", stereoCalib.leftFrame, false);
		//stereoCalib.showImage("okno1", stereoCalib.rightFrame, false);
	}	
	
	//stereoCalib.runStereoCalibration();
	stereoCalib.runFilterCalibration();
	//stereoCalib.saveSettings("calibrationParameters.xml");
	stereoCalib.closeCameras();
	
	//cv::waitKey();
	*/
	return 1;
}