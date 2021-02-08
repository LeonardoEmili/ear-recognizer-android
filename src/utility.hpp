#pragma once

#include <numeric>
#include <algorithm>

#include <dirent.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <chrono>
#include "descriptors.hpp"

using namespace cv;
using namespace std;

vector<string> readDataset(char *);

string exec(const char *);

int getDatasetSize(char *);

void writeToFile(String, String, Mat, String);

void printProgress(int iteration, int total);

void displayImage(Mat croppedEar, String imageName);

bool startsWith(String s, String prefix);

chrono::steady_clock::time_point getCurrentTime();

float getElapsedSeconds(chrono::steady_clock::time_point start,
                        chrono::steady_clock::time_point end);

vector<size_t> argSort(const vector<float> &v, bool ascending = true);

void drawLandmarks(Mat image, const vector<Point2d> landmarks, Mat &outImage,
                   const Scalar color = Scalar(0, 0, 255), int radius = 3);

Point2d computeCentroid(vector<Point2d> points);

void computeMeanAndStd(vector<double> values, double &mean, double &std);

bool equalMats(Mat a, Mat b);

void evaluate();