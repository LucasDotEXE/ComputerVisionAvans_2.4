#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace std;
using namespace cv;

Mat clear_mask;
int erosion_elem = 0;
int erosion_size = 4;
int dilation_elem = 0;
int dilation_size = 5;
int const max_elem = 2;
int const max_kernel_size = 21;
void Erosion(int, void*);
void Dilation(int, void*);

Mat frame, mask1, mask2;

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

            // Creating masks to detect the upper and lower red color.
        inRange(hsv, Scalar(20, 100, 100), Scalar(30, 255, 255), mask1);
        inRange(hsv, Scalar(20, 100, 100), Scalar(30, 255, 255), mask2);

        // Generating the final mask
        mask1 = mask1 + mask2;
        imshow("mask", mask1);

        Moments m = moments(mask1, true);

        Point p(m.m10 / m.m00, m.m01 / m.m00);

        cout << Mat(p) << endl;
       
        circle(frame, p, 5, Scalar(128, 0, 0), -1);

        Mat dialate_elem = getStructuringElement(MORPH_ELLIPSE,
            Size(2 * dilation_size + 1, 2 * dilation_size + 1),
            Point(dilation_size, dilation_size));
        dilate(mask1, clear_mask, dialate_elem);
        Mat erode_elem = getStructuringElement(MORPH_ELLIPSE,
            Size(2 * erosion_size + 1, 2 * erosion_size + 1),
            Point(erosion_size, erosion_size));
        erode(clear_mask, clear_mask, erode_elem);

 




        Mat canny_output;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        // detect edges using canny
        Canny(mask1, canny_output, 50, 150, 3);

        // find contours
        findContours(canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

        // get the moments
        vector<Moments> mu(contours.size());
        for (int i = 0; i < contours.size(); i++)
        {
            mu[i] = moments(contours[i], false);
        }

        // get the centroid of figures.
        vector<Point2f> mc(contours.size());
        for (int i = 0; i < contours.size(); i++)
        {
            mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
        }


        // draw contours
        Mat drawing(canny_output.size(), CV_8UC3, Scalar(255, 255, 255));
        for (int i = 0; i < contours.size(); i++)
        {
            Scalar color = Scalar(167, 151, 0); // B G R values
            drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
            drawContours(frame, contours, i, color, 2, 8, hierarchy, 0, Point());
            //circle(drawing, mc[i], 4, color, -1, 8, 0);
        }

        // show the resultant image
        //namedWindow("Contours", WINDOW_AUTOSIZE);
        //imshow("Contours", drawing);



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

/*



#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace std;
using namespace cv;
using namespace std;

int main(int argc, char** argv)
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

    bool bSuccess = cap.read(frame);
    flip(frame, frame, 3);


    src = frame;

    namedWindow("Erosion Demo", WINDOW_AUTOSIZE);
    namedWindow("Dilation Demo", WINDOW_AUTOSIZE);
    moveWindow("Dilation Demo", src.cols, 0);
    createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Erosion Demo",
        &erosion_elem, max_elem,
        Erosion);
    createTrackbar("Kernel size:\n 2n +1", "Erosion Demo",
        &erosion_size, max_kernel_size,
        Erosion);
    createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Dilation Demo",
        &dilation_elem, max_elem,
        Dilation);
    createTrackbar("Kernel size:\n 2n +1", "Dilation Demo",
        &dilation_size, max_kernel_size,
        Dilation);
    Erosion(0, 0);
    Dilation(0, 0);
    waitKey(0);
    return 0;
}

*/

void Erosion(int, void*)
{
    int erosion_type = 0;
    if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
    else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
    else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
    Mat element = getStructuringElement(MORPH_ELLIPSE,
        Size(2 * erosion_size + 1, 2 * erosion_size + 1),
        Point(erosion_size, erosion_size));
    erode(mask1, clear_mask, element);
    imshow("Erosion Demo", clear_mask);
}
void Dilation(int, void*)
{
    int dilation_type = 0;
    if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
    else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
    else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
    Mat element = getStructuringElement(MORPH_ELLIPSE,
        Size(2 * dilation_size + 1, 2 * dilation_size + 1),
        Point(dilation_size, dilation_size));
    dilate(mask1, clear_mask, element);
    imshow("Dilation Demo", clear_mask);
}
