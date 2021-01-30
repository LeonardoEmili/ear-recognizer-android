#include <dirent.h>

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/objdetect.hpp>
#include <opencv2/core/utility.hpp>

#include "utility.hpp"

using namespace cv;
using namespace std;
using namespace samples;

int cropAndFlipImages(char *datasetPath);

void initializeCascade(CascadeClassifier &, String);

bool isValidROI(Rect, Mat);

void displayDetected(Mat croppedEar);

int detectImage(Mat frame, CascadeClassifier &cascade, bool display);