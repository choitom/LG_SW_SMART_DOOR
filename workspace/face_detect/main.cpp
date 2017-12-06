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


int main(int argc, const char** argv)
{
	int person;
	person = faceRecognition();
	cout << "Person: " << person << endl;
}

