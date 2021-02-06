#pragma once
#include <map>

#include <opencv2/dnn.hpp>
#include <opencv2/features2d.hpp>

#include "opencv2/imgproc.hpp"
#include "utility.hpp"

#define LANDMARK_PATH "generated/landmark/"
#define FEATURES_PATH "generated/descriptors.xml"
#define FEATURES_KEY "features"
#define NAMES_KEY "names"

#define STD_DEFAULT 3

using namespace cv;
using namespace cv::dnn;
using namespace std;

void _detectLandmarks(Mat img, vector<Point2d> &ldmk);

void detectLandmark(vector<vector<Mat>> processedROI,
                    vector<vector<vector<Point2d>>> &landmarks,
                    vector<string> imageNames);

void extractFeatures(vector<vector<Mat>> images,
                     vector<vector<vector<KeyPoint>>> &keypoints,
                     vector<vector<Mat>> &descriptors, int edgeThreshold = 10,
                     InputArray mask = noArray());

void extractFeatures(vector<vector<Mat>> images, vector<Mat> &descriptors,
                     vector<string> &imageNames, int edgeThreshold = 10,
                     InputArray mask = noArray());

void extractFeatures(vector<vector<Mat>> images,
                     vector<vector<vector<Point2d>>> &landmarks,
                     vector<string> imageNames);

float computeSimilarity(Mat queryDescriptors, Mat objectDescriptors,
                        int normType = NORM_HAMMING, float ratio = 0.75,
                        bool crossCheck = false);

void logSimilarities(Mat queryDescriptor, vector<Mat> imageDescriptors,
                     String queryName, vector<string> imageNames,
                     bool filterByPrefix = false);

void reduceDataSparsity(vector<Point2d> points, vector<Point2d> &outPoints,
                        int k = STD_DEFAULT);

void exportFeatures(vector<Mat> descriptors, vector<string> imageNames,
                    const string path = FEATURES_PATH);

void importFeatures(vector<Mat> &descriptors, vector<string> &imageNames,
                    const string path = FEATURES_PATH);

double calculateVerificationGAR(vector<Mat> imageDescriptors, vector<string> imageNames,
                                double threshold);

double calculateVerificationFAR(vector<Mat> imageDescriptors, vector<string> imageNames,
                                double threshold);