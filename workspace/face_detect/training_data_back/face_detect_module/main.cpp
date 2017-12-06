#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void detectAndDisplay(Mat &frame, CascadeClassifier &face_cascade);
int main(int argc, const char** argv)
{
	VideoCapture cap(0);
	CascadeClassifier face_cascade;

	if (!face_cascade.load("./haarcascade_frontalface_alt.xml")) { 
		cout << "Error loading face_cascade" << endl;
        	return -1;
	}

	if (!cap.isOpened()) {
		cout << "Error opening camera" << endl;
		return -2;
	}

	cap.set(CAP_PROP_FRAME_WIDTH, 320);
	cap.set(CAP_PROP_FRAME_HEIGHT, 240);
    	
	Mat frame;

	while (true) {
		cap >> frame;
		detectAndDisplay(frame, face_cascade);

		if (waitKey(20) == 27) { 
			break; 
        }
	}

	cap.release();
}

void detectAndDisplay(Mat &frame, CascadeClassifier &face_cascade) {
	vector<Rect> faces;
	Mat frame_gray;
	static int frame_num = 0;
	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (size_t i = 0; i < faces.size(); i++) {
		//Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height * 0.5);
		//ellipse(frame, center, Size(faces[i].width * 0.5, faces[i].height * 0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
		rectangle(frame, faces[i], Scalar(255, 0, 255), 4, 8, 0);
		if ( i % 3 == 0) {
			Mat frame_out(frame(faces[i]));
			ostringstream name_out;
			name_out << "training_" << setfill('0') << setw(5) 
				<< frame_num++ << ".jpg";
			cout << name_out.str() << endl;
			imwrite(name_out.str(), frame_out);
		}
	}

	imshow("OpenCV Face Detect", frame);
}
