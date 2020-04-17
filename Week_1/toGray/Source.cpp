// ************************************************************************************************************************************************
// ********************************************** Omzetten naar een grijswaarde beeld *************************************************************
// ************************************************************************************************************************************************



// Laden, omzetten naar grijswaarde beeld en opslaan van een beeld 
// Uitleg: http://docs.opencv.org/doc/tutorials/introduction/load_save_image/load_save_image.html
// Jan Oostindie, dd 22-1-2015

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


int main(int argc, char** argv)
{
	// Controle of er een argument aan het programma is meegegeven.
	if (argc != 2)
	{
		cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}

	// Mat is een class voor objecten waarin een afbeelding kan worden opgeslagen.
	Mat image;

	// Lees de afbeelding in en sla deze op in image. 
	// De filenaam is het eerste argument dat meegegeven is bij aanroep van het programma.
	image = imread(argv[1], CV_LOAD_IMAGE_COLOR);

	// Controleer of alles goed is gegaan
	if (!image.data)
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// Mat object aanmaken voor grijswaarde beeld
	Mat gray_image;

	// Converteren van de ingelezen afbeelding naar een grijswaarde beeld.
	cvtColor(image, gray_image, CV_BGR2GRAY);

	// Opslaan van het grijswaarde beeld.
	string padnaam = "C:\\Users\\lucas\\Pictures\\Avatars\\gray\\gray.jpg";
	imwrite(padnaam, gray_image);

	/****** Resultaten naar het scherm sturen ******/

	// Grijswaarde beeld tonen op het scherm
	namedWindow("Gray image", CV_WINDOW_AUTOSIZE);
	imshow("Gray image", gray_image);
	waitKey(0);

	// Gebruikersinformatie
	cout << "Imagefile: " << argv[1] << " met succes geconverteerd naar grijswaarde beeld." << endl;
	cout << "Grijswaarde beeld is opgeslagen: " << padnaam << endl;
	cout << "Druk op ENTER voor vervolg ==> " << endl;

	string dummy;
	getline(cin, dummy);

	return 0;
}