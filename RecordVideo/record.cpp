#include <iostream>
#include <opencv2/opencv.hpp>
#include "record.h"

using namespace std;
using namespace cv;

void record_video(){
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
    vout.open("visitor_recording.avi", VideoWriter::fourcc('M','J','P','G'), fps, size, true);

    int frames = 0;
    int frame_cap = 10 * fps;

    while(frames < frame_cap){
        cap >> frame;
        vout << frame;
        if(frames > frame_cap)
            break;
        frames++;
        cout << frames << endl;
    }

    cap.release();
    vout.release();
    destroyAllWindows();
}

void play_video(string title){
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
