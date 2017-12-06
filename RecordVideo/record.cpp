#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <string>
#include "record.hpp"

using namespace std;
using namespace cv;


void record_video(int video_num){
	ostringstream ss;
    ss << video_num;
    string video_title = "visitor_recording" + ss.str() + ".avi";
    
    VideoCapture cap(0);
    if (!cap.isOpened()){
		cout << "Error opening camera" << endl;
		return;
	}
	cap.set(CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CAP_PROP_FRAME_HEIGHT, 480);

	Mat frame;

	Size size = Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT));
	int fps = cap.get(CAP_PROP_FPS);

	VideoWriter vout;
	vout.open(video_title, VideoWriter::fourcc('M','J','P','G'), fps, size, true);

	int frames = 0;
	int frame_cap = 10 * fps;

	while(frames < frame_cap){
		cap >> frame;
		vout << frame;
		if(frames > frame_cap)
			break;
		frames++;
    }
	cap.release();
	vout.release();
    cout << "Title: " << video_title << endl;
}

void play_video(const char* title){
	VideoCapture cap(title);
	int fps = cap.get(CAP_PROP_FPS);
	int delay = 1000/fps;

	if(!cap.isOpened()){
		cout << "Error opening " << title << endl;
		return;
	}

	while(1){
		Mat frame;
		cap >> frame;
		if(frame.empty())
			break;
		imshow(title, frame);
		char c = (char)waitKey(delay);
		if(c==27)
			break;
	}
	cap.release();
	destroyAllWindows();
}
