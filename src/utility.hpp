#include <dirent.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>

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