#pragma once

#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>

#include "opencv2/imgproc.hpp"
#include "utility.hpp"

#define NORMALIZATION_SIZE 96
#define CROPPED_PATH "generated/cropped/"
#define ROTATED_PATH "generated/rotated/"

using namespace cv;
using namespace std;

void cropAndResize(vector<vector<Rect>> &ROI, vector<vector<Mat>> &outputImages, vector<vector<double>> &paddingPercentages,
                   vector<string> imageNames, vector<Mat> grayImages,
                   int outputSize = NORMALIZATION_SIZE);

void alignImages(vector<vector<Mat>> &processedROI, vector<vector<double>> &paddingPercentages,
                 vector<vector<vector<Point2d>>> landmarks,
                 vector<string> imageNames);

void alignImage(Mat &image, double paddingPercentage, vector<Point2d> landmarks, String imageName,
                String id, int outputSize = NORMALIZATION_SIZE);
