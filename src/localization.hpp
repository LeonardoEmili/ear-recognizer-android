#pragma once

#include "normalization.hpp"

using namespace samples;

void detectROI(char *datasetPath, vector<string> imageNames,
               vector<Mat> &images, vector<vector<Rect>> &ROI,
               bool debugFlag = false);

bool _detectROI(Mat &frame, CascadeClassifier &cascade,
                vector<vector<Rect>> &ROI, bool rightClassifier,
                String imageName, const int outputSize = NORMALIZATION_SIZE);

bool isValidROI(Rect, Mat);

void initializeCascade(CascadeClassifier &, String);
