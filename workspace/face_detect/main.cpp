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

// Get the height from the first image. We'll need this
// later in code to reshape the images to their original
// size AND we need to reshape incoming faces to this size:
int im_width = 64;
int im_height = 64;

// Create a FaceRecognizer and train it on the given images:
Ptr<FaceRecognizer> model = createFisherFaceRecognizer();

void detectAndDisplay(Mat &frame, CascadeClassifier &face_cascade);
static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';');

int main(int argc, const char** argv)
{
	/* TRAINING */
	// These vectors hold the images and corresponding labels:
	vector<Mat> images;
	vector<int> labels;

	// Read in the data (fails if no valid input filename is given, but you'll get an error message):
	string fn_csv = "training_data.csv";
	try {
		read_csv(fn_csv, images, labels);
	} catch (cv::Exception& e) {
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		// nothing more we can do
		exit(1);
	}
	model->train(images, labels);

	model->save("face_model.xml");

	/* PREDICTION */
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
		// Process face by face:
		Rect face_i = faces[i];
		// Crop the face from the image. So simple with OpenCV C++:
		Mat face = frame_gray(face_i);
		// Resizing the face is necessary for Eigenfaces and Fisherfaces. You can easily
		// verify this, by reading through the face recognition tutorial coming with OpenCV.
		// Resizing IS NOT NEEDED for Local Binary Patterns Histograms, so preparing the
		// input data really depends on the algorithm used.
		//
		// I strongly encourage you to play around with the algorithms. See which work best
		// in your scenario, LBPH should always be a contender for robust face recognition.
		//
		// Since I am showing the Fisherfaces algorithm here, I also show how to resize the
		// face you have just found:
		Mat face_resized;
		cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
		// Now perform the prediction, see how easy that is:
		int prediction = model->predict(face_resized);
		// And finally write all we've found out to the original image!
		// First of all draw a green rectangle around the detected face:
		rectangle(frame, face_i, CV_RGB(0, 255,0), 1);
		// Create the text we will annotate the box with:
		string box_text = format("Prediction = %d", prediction);
		// Calculate the position for annotated text (make sure we don't
		// put illegal values in there):
		int pos_x = std::max(face_i.tl().x - 10, 0);
		int pos_y = std::max(face_i.tl().y - 10, 0);
		// And now put it into the image:
		putText(frame, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,255,0), 2.0);
	}

	imshow("OpenCV Face Detect", frame);
}

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator) {
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		Mat image, image_resized;
		if(!path.empty() && !classlabel.empty()) {
			image = imread(path, 0);
			cv::resize(image, image_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
			// Now perform the prediction, see how easy that is:
			images.push_back(image_resized);
			labels.push_back(atoi(classlabel.c_str()));
			cout << path << endl;
		}
	}
}

