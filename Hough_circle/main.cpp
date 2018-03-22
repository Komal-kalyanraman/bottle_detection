#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <string>

// Camera which is being used
# define camera_no 1

// Tune values to detect circles
# define minRadius 30
# define maxRadius 39

using namespace std;

int main(int argc, char** argv)
{
    char key = 'r';

    cv::VideoCapture camera(camera_no);

    cv::Mat src, gray, filename;

    camera.set(3,640);	//for setting o/p image resolution
    camera.set(4,480);

    camera >> src;

    while(1)
    {
    camera >> src;

    // Check if image is loaded fine
    if(src.empty()){
        printf(" Error opening image\n");
        printf(" Failed to open camera number: %d \n", camera_no);
        return -1;
    }

    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    cv::medianBlur(gray, gray, 5);
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1,
                 gray.rows/16,  // change this value to detect circles with different distances to each other
                 100, 30, minRadius, maxRadius // change the last two parameters
            // (min_radius & max_radius) to detect larger circles
    );

    if(circles.size() == 0)
    {
        cv::Point frame_center = cv::Point(80, 250);

        cv::putText(src, "No bottles detected!" , frame_center,
            cv::FONT_HERSHEY_COMPLEX_SMALL, 2, cvScalar(255, 0, 0), 1.5, CV_AA);
    }

    for( size_t i = 0; i < circles.size(); i++ )
    {
        stringstream temp;
        temp << "Bottle " << i+1 ;

        cv::Vec3i c = circles[i];
        cv::Point center = cv::Point(c[0], c[1]);
        // circle center
        cv::circle( src, center, 1, cv::Scalar(0,255,0), 9, cv::LINE_AA);

        // circle outline
        int radius = c[2];
        cv::circle( src, center, radius, cv::Scalar(255,0,255), 3, cv::LINE_AA);

        cv::putText(src, temp.str() , center,
            cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2, cvScalar(0,0,0), 1, CV_AA);

    }
    cv::imshow("Detected circles", src);
    key=cv::waitKey(1);

    if(key == 'q')
    break;
    }

    return 0;
}
