// ************************************************************************************************************************************************
// ********************************************** Laden en tonen van een afbeelding ***************************************************************
// ************************************************************************************************************************************************


// Laden en tonen van een afbeelding
// uitleg: http://docs.opencv.org/doc/tutorials/introduction/display_image/display_image.html
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
	// Mat is een class voor objecten waarin een afbeelding kan worden opgeslagen.
	Mat image;

	// Lees de afbeelding in en sla deze op in image. 
	// De filenaam is het eerste argument dat meegegeven is bij aanroep van het programma.
	image = imread("C:\\Users\\lucas\\Pictures\\Avatars\\Magic.png", CV_LOAD_IMAGE_COLOR);

	// Controleer of alles goed is gegaan
	if (!image.data)
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// Laat de afbeelding zien in een apart window
	namedWindow("Display window", WINDOW_AUTOSIZE);
	imshow("Display window", image);

	// Wacht op een muiskklik in het window van de afbeelding
	waitKey(0);

	// Ruim alle aangemaakte windows weer op.
	destroyAllWindows();

	return 0;
}
