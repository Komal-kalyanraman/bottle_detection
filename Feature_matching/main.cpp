#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"

# define camera_no 0
# define dataset_size 1021

int main( int argc, char** argv )
{
    cv::Mat src, img_scene, H, img_object;
    std::vector< std::vector< cv::Point> > contours;
    int total_matches=0, RailNeer=0, Sprite=0, Pepsi=0;
    char key = 'r';

    cv::VideoCapture camera(camera_no);

    camera.set(3,640);      //-------------- for setting o/p image resolution
    camera.set(4,480);

    cv::Rect Rec(150, 0, 250, 480);     //-------------- ROI selection

    camera >> src;

    int n = 1001;
    std::ostringstream os;
    std::string s;

    while (true)
    {

        if(src.empty()){
            printf(" Error opening image\n");
            printf(" Failed to open camera number: %d \n", camera_no);
            return -1;
        }       // Check if image is loaded fine

        if(n > dataset_size)
        {
            n = 1001;
        }       // Reset dataset template numbner

        for( n; n < (dataset_size+1); n++ )
        {
            os << "/home/phoenix/Desktop/Bottle_detection/2.Trial_software/Feature_matching/test_images/" << n << ".jpg";
            s = os.str();
            goto L1;
        }

        L1:
        img_object = cv::imread( s , cv::IMREAD_GRAYSCALE );
        n++;

        s.clear();
        os.seekp(0);    // Clean the string so that new string can be loaded for the next template

        camera >> src;
        cv::Mat Roi = src(Rec);
        cv::cvtColor(Roi, img_scene, cv::COLOR_RGB2GRAY);

        if( !img_object.data || !img_scene.data )
        {
            std::cout<< " --(!) Error reading images " << std::endl;
            return -1;
        }

        //-------------------- Step 1: Detect the keypoints and extract descriptors using SURF
        int minHessian = 200;
        cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create( minHessian, 4, 3, false, false );
        std::vector<cv::KeyPoint> keypoints_object, keypoints_scene;
        cv::Mat descriptors_object, descriptors_scene;

        detector->detectAndCompute( img_object, cv::Mat(), keypoints_object, descriptors_object );
        detector->detectAndCompute( img_scene, cv::Mat(), keypoints_scene, descriptors_scene );

        //-------------------- Step 2: Matching descriptor vectors using FLANN matcher
        cv::FlannBasedMatcher matcher;
        std::vector< cv::DMatch > matches;
        matcher.match( descriptors_object, descriptors_scene, matches );
        double max_dist = 0; double min_dist = 100;

        //-------------------- Quick calculation of max and min distances between keypoints
        for( int i = 0; i < descriptors_object.rows; i++ )
        {
            double dist = matches[i].distance;
            if( dist < min_dist ) min_dist = dist;
            if( dist > max_dist ) max_dist = dist;
        }

        //-------------------- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
        std::vector< cv::DMatch > good_matches;
        for( int i = 0; i < descriptors_object.rows; i++ )
        { if( matches[i].distance <= 3*min_dist )
             { good_matches.push_back( matches[i]); }
        }

        cv::Mat img_matches, filter, contourImg;

        //-------------------- Contour area calculation
        int contour_area;
        cv::drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
                     good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
                     std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

        //-------------------- Localize the object
        std::vector<cv::Point2f> obj;
        std::vector<cv::Point2f> scene;
        for( size_t i = 0; i < good_matches.size(); i++ )
        {
          //------------------ Get the keypoints from the good matches
          obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
          scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
        }

        //-------------------- This loop is need to avoid crashing the code when there are no key points
        if(obj.empty())
        {
            std::cout << "No key points" << std::endl;
        }
        else
        {
              H = cv::findHomography( obj, scene, cv::RANSAC );
        }

        //-------------------- This loop is need to avoid crashing the code when there are no good match
        if(H.empty())
        {
            std::cout << "No good match" << std::endl;
        }
        else
        {
            //---------------- Get the corners from the template
            std::vector<cv::Point2f> obj_corners(4);
            obj_corners[0] = cvPoint(0,0);
            obj_corners[1] = cvPoint( img_object.cols, 0 );
            obj_corners[2] = cvPoint( img_object.cols, img_object.rows );
            obj_corners[3] = cvPoint( 0, img_object.rows );

            std::vector<cv::Point2f> scene_corners(4);
            cv::perspectiveTransform( obj_corners, scene_corners, H);

            //---------------- Draw lines between the corners (the mapped object in the scene)
            line( img_matches, scene_corners[0] + cv::Point2f( img_object.cols, 0), scene_corners[1] + cv::Point2f( img_object.cols, 0), cv::Scalar(0, 255, 0), 4 );
            line( img_matches, scene_corners[1] + cv::Point2f( img_object.cols, 0), scene_corners[2] + cv::Point2f( img_object.cols, 0), cv::Scalar( 0, 255, 0), 4 );
            line( img_matches, scene_corners[2] + cv::Point2f( img_object.cols, 0), scene_corners[3] + cv::Point2f( img_object.cols, 0), cv::Scalar( 0, 255, 0), 4 );
            line( img_matches, scene_corners[3] + cv::Point2f( img_object.cols, 0), scene_corners[0] + cv::Point2f( img_object.cols, 0), cv::Scalar( 0, 255, 0), 4 );

            //---------------- Filtering out bounding box lines
            cv::inRange(img_matches, cv::Scalar(0, 254, 0), cv::Scalar(0, 255, 0), filter);

            contourImg = filter.clone();

            cv::findContours(contourImg, contours, // a vector of contours
                         CV_RETR_EXTERNAL, // retrieve the external contours
                         CV_CHAIN_APPROX_NONE); // all pixels of each contours

            for( int i = 0; i < contours.size(); i++ )
            {
                contour_area = cv::contourArea(contours[i]);
                int template_number = n-1;

                if(template_number>1000 && n<1009)
                {
                    if(contour_area > 35000)
                    {
                        RailNeer++;
                        total_matches++;
                    }
                }       //--------------  RailNeer
                else if(n>1008 && n<1015)
                {
                    if(contour_area > 4000)
                    {
                        Sprite++;
                        total_matches++;
                    }
                }       //--------------  Sprite
                else if(n>1014 && n<1022)
                {
                    if(contour_area > 20000)
                    {
                        Pepsi++;
                        total_matches++;
                    }
                }       //--------------  Pepsi
                else{}
            }

            if(n == (dataset_size+1))
            {
                if(total_matches == 0)
                {
                    cv::Point text_detection = cv::Point(5, 400);
                    cv::putText(Roi, "No Bottle Detected" , text_detection,
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 2.0, cvScalar(0,255,255), 2.5, CV_AA);
                }
                else if(total_matches != 0 && RailNeer>=Sprite && RailNeer>=Pepsi)
                {
                    cv::Point text_detection = cv::Point(5, 400);
                    cv::putText(Roi, "RailNeer" , text_detection,
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 2.0, cvScalar(0,255,255), 2.5, CV_AA);
                }
                else if(total_matches != 0 && Sprite>=RailNeer && Sprite>=Pepsi)
                {
                    cv::Point text_detection = cv::Point(5, 440);
                    cv::putText(Roi, "Sprite" , text_detection,
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 2.5, cvScalar(0,255,255), 2.5, CV_AA);
                }
                else if(total_matches != 0 && Pepsi>=RailNeer && Pepsi>=Sprite)
                {
                    cv::Point text_detection = cv::Point(5, 400);
                    cv::putText(Roi, "Pepsi" , text_detection,
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 2.0, cvScalar(0,255,255), 2.5, CV_AA);
                }
                else{}

                //---------------- Resetting the search to zero
                RailNeer = 0;
                Sprite = 0;
                Pepsi = 0;
                total_matches = 0;
            }
            cv::imshow( "Good Matches & Object detection", img_matches );
            cv::imshow( "Filtered Image", filter );
            cv::imshow("Source Image", Roi);
        }

        key=cv::waitKey(1);

        if(key == 'q')
        break;
    }
    return 0;
}
