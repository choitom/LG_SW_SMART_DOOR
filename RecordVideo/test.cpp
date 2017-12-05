#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "record.h"

using namespace std;
using namespace cv;

int main(int, char**){
    record_video();

    string title = "visitor_recording.avi";
    play_video(title);
    return 0;
}
