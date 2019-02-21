#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <string>

// Camera which is being used
# define camera_no 0

using namespace std;

int serialTx(void);
int i = 0;

int main(int argc, char** argv)
{

    cv::VideoCapture camera(camera_no);
    cv::Mat src;

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
    serialTx(); // serial comm call

    cv::imshow("Image", src);
    cv::waitKey(1);
    }

    return 0;
}

int serialTx()
{
    FILE *file;
    file = fopen("/dev/ttyACM0","w");  //Opening device file

    fprintf(file,"%d",i); //Writing to the file
    fprintf(file,"%c",','); //To separate digits

    if(i>99)
    {
        i = 0;
    }
    else
    {
        i++;
    }

    fclose(file); //closing device file is important. Else data will not be transmitted
}
