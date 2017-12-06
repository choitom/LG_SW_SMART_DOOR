#include "smartdoor.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
using namespace std;
using namespace cv;
using namespace cv::face;

static int video_num = 0;

int main(int argc, const char** argv)
{
    record_video(video_num++);
    play_video("visitor_recording0.avi");

    int person;
	person = faceRecognition();
	cout << "Person: " << person << endl;
}

