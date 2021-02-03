#pragma once

#include <opencv2/dnn.hpp>
#include <opencv2/features2d.hpp>

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

void extractFeatures(vector<vector<Mat>> images,
                     vector<vector<Mat>> &descriptors, int edgeThreshold = 10,
                     InputArray mask = noArray());

float computeSimilarity(Mat queryDescriptors, Mat objectDescriptors,
                        int normType = NORM_HAMMING, float ratio = 0.75,
                        bool crossCheck = false);

void logSimilarities(Mat queryDescriptor, vector<vector<Mat>> imageDescriptors,
                     String queryName, vector<string> imageNames,
                     bool filterByPrefix = false);