#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
//#include <opencv2/ximgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;
Mat src_gray;

int thresh = 100;
RNG rng(12345);

int erosion_size = 3;
int dilation_size = 3;

VideoCapture cap(0);

void thresh_callback(int, void*);
Mat open(Mat);
Mat close(Mat);

void ThinSubiteration1(Mat& pSrc, Mat& pDst) {
    int rows = pSrc.rows;
    int cols = pSrc.cols;
    pSrc.copyTo(pDst);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (pSrc.at<float>(i, j) == 1.0f) {
                /// get 8 neighbors
                /// calculate C(p)
                int neighbor0 = (int)pSrc.at<float>(i - 1, j - 1);
                int neighbor1 = (int)pSrc.at<float>(i - 1, j);
                int neighbor2 = (int)pSrc.at<float>(i - 1, j + 1);
                int neighbor3 = (int)pSrc.at<float>(i, j + 1);
                int neighbor4 = (int)pSrc.at<float>(i + 1, j + 1);
                int neighbor5 = (int)pSrc.at<float>(i + 1, j);
                int neighbor6 = (int)pSrc.at<float>(i + 1, j - 1);
                int neighbor7 = (int)pSrc.at<float>(i, j - 1);
                int C = int(~neighbor1 & (neighbor2 | neighbor3)) +
                    int(~neighbor3 & (neighbor4 | neighbor5)) +
                    int(~neighbor5 & (neighbor6 | neighbor7)) +
                    int(~neighbor7 & (neighbor0 | neighbor1));
                if (C == 1) {
                    /// calculate N
                    int N1 = int(neighbor0 | neighbor1) +
                        int(neighbor2 | neighbor3) +
                        int(neighbor4 | neighbor5) +
                        int(neighbor6 | neighbor7);
                    int N2 = int(neighbor1 | neighbor2) +
                        int(neighbor3 | neighbor4) +
                        int(neighbor5 | neighbor6) +
                        int(neighbor7 | neighbor0);
                    int N = min(N1, N2);
                    if ((N == 2) || (N == 3)) {
                        /// calculate criteria 3
                        int c3 = (neighbor1 | neighbor2 | ~neighbor4) & neighbor3;
                        if (c3 == 0) {
                            pDst.at<float>(i, j) = 0.0f;
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    if (!cap.isOpened())
    {
        std::cout << "Cannot open the video cam" << std::endl;
        return -1;
    }

    const char* source_window = "Source";
    namedWindow(source_window);

    const int max_thresh = 255;
    createTrackbar("Canny thresh:", source_window, &thresh, max_thresh, thresh_callback);

    Mat src, frame, mask1, mask2;
    cap.read(src);

    cvtColor(src, src_gray, COLOR_BGR2GRAY);
    blur(src_gray, src_gray, Size(3, 3));
    imshow(source_window, src);

    thresh_callback(0, 0);

    while (true)
    {
        bool bSuccess = cap.read(frame);

        if (!bSuccess)
        {
            std::cout << "Cannot read a frame from video stream" << std::endl;
        }
        else
        {
            flip(frame, frame, 3);

            Mat hsv, clFiltered, cleared;
            cvtColor(frame, hsv, COLOR_BGR2HSV);

            inRange(hsv, Scalar(14, 0, 100), Scalar(64, 255, 255), mask1);
            inRange(hsv, Scalar(14, 0, 100), Scalar(64, 255, 255), mask1);

            clFiltered = mask1 + mask2;
            imshow("Color filter", clFiltered);

            Mat opened = open(clFiltered);

            cleared = close(opened);


            //cvtColor(cleared, src_gray, COLOR_BGR2GRAY);
            src_gray = cleared;
            blur(src_gray, src_gray, Size(3, 3));

            imshow("open&closed", cleared);

            //ximgproc::thinning()
           
            //thinning(src_gray,_gray);
            /*            cv::Mat skel(src_gray.size(), CV_8UC1, cv::Scalar(0));
            cv::Mat temp(src_gray.size(), CV_8UC1);

            cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

            bool done;
            do
            {
                cv::morphologyEx(src_gray, temp, cv::MORPH_OPEN, element);
                cv::bitwise_not(temp, temp);
                cv::bitwise_and(src_gray, temp, temp);
                cv::bitwise_or(skel, temp, skel);
                cv::erode(src_gray, src_gray, element);

                double max;
                cv::minMaxLoc(src_gray, 0, &max);
                done = (max == 0);
            } while (!done);
            
            cv::imshow("Skeleton", skel);
            
            */
            Mat dist;
            distanceTransform(cleared, dist, DIST_L2, 3);
            normalize(dist, dist, 0, 1.0, NORM_RELATIVE);
            //threshold(dist, dist, 100, 155, CV_THRESH_TRUNC);

            imshow("Help", dist);
            
            Mat canny_output;
            Canny(cleared, canny_output, thresh, thresh * 2);
            vector<vector<Point> > contours;
            findContours(canny_output, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
            if (contours.size() != 0) {
                vector<vector<Point> >hull(contours.size());
                for (size_t i = 0; i < contours.size(); i++)
                {
                    convexHull(contours[i], hull[i]);
                }
                Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
                int largestContourIndex = 0;
                int largestArea = 0;
                for (size_t i = 0; i < contours.size(); i++)
                {
                    int area = contourArea(contours[i]);
                    if (area >= largestArea) {
                        largestArea = area;
                        largestContourIndex = i;
                    }

                }
                Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
                drawContours(drawing, contours, (int)largestContourIndex, color);
                drawContours(drawing, hull, (int)largestContourIndex, color);

                int count = static_cast<int>(contours[largestContourIndex].size());
                stringstream ss;
                ss << count;


                putText(drawing, "Finger Count:" + ss.str(), Point(0, 40), 1, 1, color, 1, 8, false);
                //imshow("Hull demo", drawing);
            }
            

            

        }
        if (waitKey(1) == 27)
        {
            std::cout << "esc key is pressed by user" << std::endl;
            break;
        }
    }

    waitKey();
    return 0;
}

Mat open(Mat input) {
    Mat eroded, dialate;

    Mat erode_elem = getStructuringElement(MORPH_ELLIPSE,
        Size(2 * erosion_size + 1, 2 * erosion_size + 1),
        Point(erosion_size, erosion_size));
    erode(input, eroded, erode_elem);

    Mat dialate_elem = getStructuringElement(MORPH_ELLIPSE,
        Size(2 * dilation_size + 1, 2 * dilation_size + 1),
        Point(dilation_size, dilation_size));
    dilate(eroded, dialate, dialate_elem);

    return dialate;
}

Mat close(Mat input) {
    Mat eroded, dialate;

    Mat dialate_elem = getStructuringElement(MORPH_ELLIPSE,
        Size(2 * dilation_size + 1, 2 * dilation_size + 1),
        Point(dilation_size, dilation_size));
    dilate(input, dialate, dialate_elem);

    Mat erode_elem = getStructuringElement(MORPH_ELLIPSE,
        Size(2 * erosion_size + 1, 2 * erosion_size + 1),
        Point(erosion_size, erosion_size));
    erode(dialate, eroded, erode_elem);

    return eroded;
}

void thresh_callback(int, void*)
{

}
/*
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>


using namespace cv;
using namespace std;

Mat src, src_gray;
Mat dst, detected_edges;
int lowThreshold = 0;

const int max_lowThreshold = 100;
const int ratio = 3;
const int kernel_size = 3;
const char* window_name = "Edge Map";

Mat clear_mask;
int erosion_size = 3;
int dilation_size = 3;

int thresh = 100;
RNG rng(12345);

VideoCapture cap(0);
Mat image;
Mat frame, frame_gry, mask1, mask2;

static void CannyThreshold(int, void*)
{
    blur(frame_gry, detected_edges, Size(3, 3));
    Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
    //vector<vector<Point> > contours;
    //findContours(canny_output, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    dst = Scalar::all(0);
    frame.copyTo(dst, detected_edges);
    imshow(window_name, dst);
}

Mat open(Mat input) {
    Mat eroded, dialate;

    Mat erode_elem = getStructuringElement(MORPH_ELLIPSE,
        Size(2 * erosion_size + 1, 2 * erosion_size + 1),
        Point(erosion_size, erosion_size));
    erode(input, eroded, erode_elem);

    Mat dialate_elem = getStructuringElement(MORPH_ELLIPSE,
        Size(2 * dilation_size + 1, 2 * dilation_size + 1),
        Point(dilation_size, dilation_size));
    dilate(eroded, dialate, dialate_elem);

    return dialate;
}

Mat close(Mat input) {
    Mat eroded, dialate;

    Mat dialate_elem = getStructuringElement(MORPH_ELLIPSE,
        Size(2 * dilation_size + 1, 2 * dilation_size + 1),
        Point(dilation_size, dilation_size));
    dilate(input, dialate, dialate_elem);

    Mat erode_elem = getStructuringElement(MORPH_ELLIPSE,
        Size(2 * erosion_size + 1, 2 * erosion_size + 1),
        Point(erosion_size, erosion_size));
    erode(dialate, eroded, erode_elem);

    return eroded;
}

int main(int argc, char** argv)
{
    if (!cap.isOpened())
    {
        std::cout << "Cannot open the video cam" << std::endl;
        return -1;
    }


while (true)
{
    bool bSuccess = cap.read(frame);

    if (!bSuccess)
    {
        std::cout << "Cannot read a frame from video stream" << std::endl;
    }
    else
    {
        flip(frame, frame, 3);

        Mat hsv, clFiltered, cleared;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        inRange(hsv, Scalar(14, 0, 100), Scalar(64, 255, 255), mask1);
        inRange(hsv, Scalar(14, 0, 100), Scalar(64, 255, 255), mask1);

        clFiltered = mask1 + mask2;
        imshow("Color filter", clFiltered);

        Mat opened = open(clFiltered);

        cleared = close(opened);

        imshow("After dialate&erode", cleared);
    }

    cv::cvtColor(frame, frame_gry, CV_BGR2GRAY);
    dst.create(frame.size(), frame.type());


    cv::namedWindow(window_name, WINDOW_AUTOSIZE);
    cv::createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);
    CannyThreshold(0, 0);

    if (waitKey(1) == 27)
    {
        std::cout << "esc key is pressed by user" << std::endl;
        break;
    }
}






return 0;
}

*/
