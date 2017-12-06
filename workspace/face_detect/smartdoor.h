#include <opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"

//#ifdef __cpluscplus
extern "C" {
//#endif
#include <vector>

extern int faceRecognition();
extern void  
detectAndDisplay(cv::Mat &frame, 
cv::CascadeClassifier &face_cascade, std::vector<int> & temp_out);
//#ifdef __cplusplus
}
//#endif
static void read_csv(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator = ';');
