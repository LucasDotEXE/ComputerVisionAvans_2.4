#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc, char* argv[])
{
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

    Mat frame;

    namedWindow("Source Image", CV_WINDOW_AUTOSIZE);

    while (true)
    {

        bool bSuccess = cap.read(frame);
        flip(frame, frame, 3);
        // Controlleer of het frame goed gelezen is.
        if (!bSuccess)
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }

        //Converting image from BGR to HSV color space.
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        Mat mask1, mask2;
            // Creating masks to detect the upper and lower red color.
        inRange(hsv, Scalar(20, 100, 100), Scalar(30, 255, 255), mask1);
        inRange(hsv, Scalar(20, 100, 100), Scalar(30, 255, 255), mask2);

        // Generating the final mask
        mask1 = mask1 + mask2;
        imshow("mask", mask1);


        Mat dist;
        distanceTransform(mask1, dist, DIST_L2, 3);
        // Normalize the distance image for range = {0.0, 1.0}
        // so we can visualize and threshold it
        normalize(dist, dist, 0, 1.0, NORM_MINMAX);
        imshow("Distance Transform Image", dist);
        //imshow("result", result);

        Moments m = moments(mask1, true);

        Point p(m.m10 / m.m00, m.m01 / m.m00);

        cout << Mat(p) << endl;
       
        circle(frame, p, 5, Scalar(128, 0, 0), -1);

        imshow("Source Image", frame);


        if (waitKey(1) == 27)
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    waitKey(0);


    return 0;
}