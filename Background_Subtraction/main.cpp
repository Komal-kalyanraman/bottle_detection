// http://study.marearts.com/2017/04/opencv-background-subtraction-32.html

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <iostream>
#include <sstream>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <string>

#include <iostream>
#include <time.h>

# define camera_no 1

using namespace std;
using namespace cv;

Mat src, erosion1_dst, dilation_dst, erosion2_dst, fgMaskMOG2, ContourImg;

int erosion1_elem = 0, dilation_elem = 0, erosion2_elem = 0;
int erosion1_size = 0, dilation_size = 0, erosion2_size = 0;
int const max_elem = 2, max_kernel_size = 21;
int i=0, j=0;
stringstream temp;

/** Function Headers */
void Erosion1( int, void* );
void Dilation( int, void* );
void Erosion2( int, void* );

// Image Type identification
//+--------+----+----+----+----+------+------+------+------+
//|        | C1 | C2 | C3 | C4 | C(5) | C(6) | C(7) | C(8) |
//+--------+----+----+----+----+------+------+------+------+
//| CV_8U  |  0 |  8 | 16 | 24 |   32 |   40 |   48 |   56 |
//| CV_8S  |  1 |  9 | 17 | 25 |   33 |   41 |   49 |   57 |
//| CV_16U |  2 | 10 | 18 | 26 |   34 |   42 |   50 |   58 |
//| CV_16S |  3 | 11 | 19 | 27 |   35 |   43 |   51 |   59 |
//| CV_32S |  4 | 12 | 20 | 28 |   36 |   44 |   52 |   60 |
//| CV_32F |  5 | 13 | 21 | 29 |   37 |   45 |   53 |   61 |
//| CV_64F |  6 | 14 | 22 | 30 |   38 |   46 |   54 |   62 |
//+--------+----+----+----+----+------+------+------+------+

// Image Depth identification
//CV_8U   0
//CV_8S   1
//CV_16U  2
//CV_16S  3
//CV_32S  4
//CV_32F  5
//CV_64F  6
//CV_USRTYPE1 7

int main(int argc, char* argv[])
{
    char key = 'r';
    float rect_detection_ratio = 0;
    double rect_area, contour_area;
    bool bottle_detected = false;

    //////////// Background subtraction parameter setting Set threshold high, History need not be varied much
    Ptr< BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
    pMOG2 = createBackgroundSubtractorMOG2(1, 400.0, false);

    cv::VideoCapture camera(camera_no);

    camera.set(3,640);	//for setting o/p image resolution
    camera.set(4,480);

    camera >> src;

    while (true) {

        camera >> src;

        /////////////////////////////// Check if image is loaded fine
        if(src.empty()){
            printf(" Error opening image\n");
            printf(" Failed to open camera number: %d \n", camera_no);
            return -1;
        }

        /////////////////////////////// Background subtraction algorithm

        pMOG2->apply(src, fgMaskMOG2, 0);
        Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);

        /////////////////////////////// Tracker Bar
        if(1)
        {
            /// Create windows
            namedWindow( "Erosion1 Demo", CV_WINDOW_AUTOSIZE );
            namedWindow( "Dilation Demo", CV_WINDOW_AUTOSIZE );
            namedWindow( "Erosion2 Demo", CV_WINDOW_AUTOSIZE );

            /// Create Erosion1 Trackbar
            createTrackbar( "Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Erosion1 Demo",
                            &erosion1_elem, max_elem,
                            Erosion1 );

            createTrackbar( "Kernel size:\n 2n +1", "Erosion1 Demo",
                            &erosion1_size, max_kernel_size,
                            Erosion1 );

            /// Create Dilation Trackbar
            createTrackbar( "Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Dilation Demo",
                            &dilation_elem, max_elem,
                            Dilation );

            createTrackbar( "Kernel size:\n 2n +1", "Dilation Demo",
                            &dilation_size, max_kernel_size,
                            Dilation );

            /// Create Erosion2 Trackbar
            createTrackbar( "Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "Erosion2 Demo",
                            &erosion2_elem, max_elem,
                            Erosion2 );

            createTrackbar( "Kernel size:\n 2n +1", "Erosion2 Demo",
                            &erosion2_size, max_kernel_size,
                            Erosion2 );

            /// Default start
            Erosion1( 0, 0 );
            Dilation( 0, 0 );
            Erosion2( 0, 0 );
        }

        /////////////////////////////// Normal Rectangle
        if(1)
        {
            /*
            ContourImg = erosion2_dst.clone();

            /// Finding Image Depth and Type
            //cout<< "Image type = " << erosion2_dst.type() << endl;
            //cout<< "Image depth = " << erosion2_dst.depth() << endl;

            //Find contour
            vector< vector< Point> > contours;
            findContours(ContourImg,
                      contours, // a vector of contours
                      CV_RETR_EXTERNAL, // retrieve the external contours
                      CV_CHAIN_APPROX_NONE); // all pixels of each contours

            Rect bounding_rect;
            vector< Rect > output;

            for( int i = 0; i< contours.size(); i++ ) // iterate through each contou
            {
             //Create bounding rect of object
             //rect draw on origin image
             double a = contourArea(contours[i]);
             cout<< "Contour area of " << "is" << a << endl;

             if(a > 5000.0 && a < 300000.0) // to detect objects of a particular range
             {
                 bounding_rect=boundingRect(contours[i]);
                 Rect mr= boundingRect(contours[i]);
                 rectangle(src, mr, CV_RGB(255,0,0), 2);
             }

            }*/

        }

        /////////////////////////////// Rotated Rectangle & Drawing contour
        if(1)
        {
            ContourImg = erosion2_dst.clone();

            //Find Contours
            vector< vector< Point> > contours;
            findContours(ContourImg,
                      contours, // a vector of contours
                      CV_RETR_EXTERNAL, // retrieve the external contours
                      CV_CHAIN_APPROX_NONE); // all pixels of each contours

            vector<RotatedRect> minRect( contours.size() );

            double rect_width, rect_height, x_width, y_width, x_height, y_height;

            if(contours.size() == 0)
            {
                bottle_detected = false;
            }

            for( int i = 0; i < contours.size(); i++ )
               {
                    bottle_detected = true;
                    contour_area = contourArea(contours[i]);

                    if(contour_area > 5000.0 && contour_area < 300000.0)  //To detect objects of particular range
                    {
                        cv::drawContours(dst, contours, i, Scalar(255,0,255), 2, 8);
                        minRect[i] = minAreaRect( Mat(contours[i]) );

                        // rotated rectangle
                        Point2f rect_points[4];
                        minRect[i].points( rect_points );

                        for( int j = 0; j < 4; j++ )
                           line( src, rect_points[j], rect_points[(j+1)%4], CV_RGB(0,255,0), 3, 8 );

                        x_width = rect_points[0].x - rect_points[1].x;
                        y_width = rect_points[0].y - rect_points[1].y;
                        rect_width = sqrt((x_width * x_width) + (y_width * y_width));

                        x_height = rect_points[0].x - rect_points[3].x;
                        y_height = rect_points[0].y - rect_points[3].y;
                        rect_height = sqrt((x_height * x_height) + (y_height * y_height));

                        rect_area = rect_width*rect_height;

//                        cout << "Bounded rect area = " << rect_area << endl;
//                        cout << "Contour area = " << contour_area << endl;

                        rect_detection_ratio = contour_area/rect_area;
//                        cout << "Rect ratio = " << rect_detection_ratio << endl;
                        }

                  }

               }

        /////////////////////////////// Can, Bottle segregation & volume detection
        if(1)
        {
            cv::Point text_detection = cv::Point(20, 75);

            if( bottle_detected == false)
            {
                cv::putText(src, "No Bottles or Tin Can detected" , text_detection,
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cvScalar(255,0,0), 2.5, CV_AA);
            }

            else if(rect_detection_ratio > 0.89)
            {
                cv::putText(src, "Tin Can" , text_detection,
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cvScalar(0,0,255), 2.5, CV_AA);
            }

            else if(rect_detection_ratio < 0.89)
            {
                if(contour_area > 15000.0 && contour_area < 28000.0)
                {
                    cv::putText(src, "Bottle - 250ml to 500ml" , text_detection,
                                cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cvScalar(0,0,255), 2.5, CV_AA);
                }

                else if(contour_area > 29500.0 && contour_area < 43000.0)
                {
                    cv::putText(src, "Bottle - 500ml to 1ltr" , text_detection,
                                cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cvScalar(0,0,255), 2.5, CV_AA);
                }

                else if(contour_area > 47500.0 && contour_area < 55000.0)
                {
                    cv::putText(src, "Bottle - 1ltr to 1.5ltr" , text_detection,
                                cv::FONT_HERSHEY_COMPLEX_SMALL, 1.5, cvScalar(0,0,255), 2.5, CV_AA);
                }

                else
                {

                }
            }

            else
            {

            }

        }

        imshow("Original image", src);
        imshow("Detected Contour", dst);

        key=cv::waitKey(10);

        if(key == 'q')
        break;
    }

    return 0;
}


/**  @function Erosion  */
void Erosion1( int, void* )
{
  int erosion1_type;
  if( erosion1_elem == 0 ){ erosion1_type = MORPH_RECT; }
  else if( erosion1_elem == 1 ){ erosion1_type = MORPH_CROSS; }
  else if( erosion1_elem == 2) { erosion1_type = MORPH_ELLIPSE; }

  //// Set "erosion1_type" to 2, set "erosion1_size" to 1
  Mat element = getStructuringElement( erosion1_type,
                                       Size( 2*erosion1_size + 1, 2*erosion1_size+1 ),
                                       Point( erosion1_size, erosion1_size ) );

  /// Apply the erosion operation
  erode( fgMaskMOG2, erosion1_dst, element );
  imshow( "Erosion1 Demo", erosion1_dst );
}

/** @function Dilation */
void Dilation( int, void* )
{
  int dilation_type;
  if( dilation_elem == 0 ){ dilation_type = MORPH_RECT; }
  else if( dilation_elem == 1 ){ dilation_type = MORPH_CROSS; }
  else if( dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

  //// Set "dilation_type" to 2, set "dilation_size" to 21
  Mat element = getStructuringElement( dilation_type,
                                       Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       Point( dilation_size, dilation_size ) );

  /// Apply the dilation operation
  dilate( erosion1_dst, dilation_dst, element );
  imshow( "Dilation Demo", dilation_dst );
}

/**  @function Erosion  */
void Erosion2( int, void* )
{
  int erosion2_type;
  if( erosion2_elem == 0 ){ erosion2_type = MORPH_RECT; }
  else if( erosion2_elem == 1 ){ erosion2_type = MORPH_CROSS; }
  else if( erosion2_elem == 2) { erosion2_type = MORPH_ELLIPSE; }

  //// Set "erosion2_type" to 2, set "erosion2_size" to 16
  Mat element = getStructuringElement( erosion2_type,
                                       Size( 2*erosion2_size + 1, 2*erosion2_size+1 ),
                                       Point( erosion2_size, erosion2_size ) );

  /// Apply the erosion operation
  erode( dilation_dst, erosion2_dst, element );
  imshow( "Erosion2 Demo", erosion2_dst );
}
