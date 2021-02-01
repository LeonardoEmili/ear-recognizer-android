
#include <opencv2/dnn.hpp>

#include "opencv2/imgproc.hpp"
#include "utility.hpp"

#define LANDMARK_PATH "generated/landmark/"

using namespace cv;
using namespace cv::dnn;
using namespace std;

void _detectLandmarks(Mat img, vector<Point2d> &ldmk);

void detectLandmark(vector<vector<Mat>> processedROI,
                    vector<vector<vector<Point2d>>> &landmarks,
                    vector<string> imageNames);