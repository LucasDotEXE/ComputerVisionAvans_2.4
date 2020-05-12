// Dit programma leest continue een camera beeld in en toont deze op het scherm
// Bij sommige camera's is het beeld gespiegeld. Dit kan worden opgelost 
// met de flip-functie van OpenCV.
// 
// Jan Oostindie, dd 22-2-2015

#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <string>
#include "blobdetectionavans.h"

using namespace cv;
using namespace std;

Mat binary16S;

const int threshold_slider_max = 20000;
int sliderthereshhold = 50;
int thresholdvalue = 50;
int slider_min = 3000;
int slider_max = 8000;

Mat labeledImage3;
vector<Point2d*> firstpixelVec3;
vector<Point2d*> posVec3;
vector<int> areaVec3;

int minArea = 6000;
int maxArea = 16000;


void on_trackbar(int, void*)
{
	thresholdvalue = sliderthereshhold;
}

void on_trackbar_min(int, void*)
{
	// waarde ophalen van de slider
	minArea = slider_min;
}

void on_trackbar_max(int, void*)
{
	// waarde ophalen van de slider
	maxArea = slider_max;
}

int main(int argc, char* argv[])
{
	// Open de camera met nummer 1 in lijst (red.: nr 0 was bij mij de camera in de klep van mijn laptop)  
	VideoCapture cap(0);
	// Controle of de camera wordt herkend.
	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	// Breedte en hooogte van de frames die de camera genereert ophalen. 
	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	// Window maken waarin de beelden "live" getoond worden
	namedWindow("MyVideo", CV_WINDOW_AUTOSIZE);
	//namedWindow("GrayVersion", CV_WINDOW_AUTOSIZE);

	// Continue loop waarin een beeld wordt opgehaald en wordt getoond in het window

	createTrackbar("Threshold Trackbar", "MyVideo", &sliderthereshhold, 200, on_trackbar);
	createTrackbar("Min Trackbar", "MyVideo", &slider_min, threshold_slider_max, on_trackbar_min);
	createTrackbar("Max Trackbar", "MyVideo", &slider_max, threshold_slider_max, on_trackbar_max);
	// Rechtstreeks aanroepen van de callback functie om het eerste beeld te krijgen
	on_trackbar_min(slider_min, 0);
	on_trackbar_max(slider_max, 0);

	Mat frame;

	while (1)	{
		// Lees een nieuw frame
		bool bSuccess = cap.read(frame);
		flip(frame, frame, 3);
		// Controlleer of het frame goed gelezen is.
		if (!bSuccess)
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		Mat gray_image;
		cvtColor(frame, gray_image, CV_BGR2GRAY);
		// Het tonen van grijswaarde beeld
		Mat binaryImage;
		threshold(gray_image, binaryImage, thresholdvalue, 1, CV_THRESH_BINARY_INV);
		imshow("MyVideo", frame);

		cout << "Snapshot Made" << endl;

		binaryImage.convertTo(binary16S, CV_16S);

		Mat labeledImage;
		labelBLOBs(binary16S, labeledImage);
		Mat labeledImage2;
		vector<Point2d*> firstpixelVec2;
		vector<Point2d*> posVec2;
		vector<int> areaVec2;
		labelBLOBsInfo(binary16S, labeledImage2, firstpixelVec2, posVec2, areaVec2, minArea, maxArea);

		cout << "Number of objects: "<< firstpixelVec2.size() << endl;
		show16SImageStretch(labeledImage2, "GrayVersion");

		//  Wacht 30 ms op ESC-toets. Als ESC-toets is ingedrukt verlaat dan de loop
		if (waitKey(1) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
		waitKey();
	} 
	return 0;
}
