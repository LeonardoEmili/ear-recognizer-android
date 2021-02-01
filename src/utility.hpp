#include <dirent.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

vector<string> readDataset(char *);

string exec(const char *);

int getDatasetSize(char *);

void writeToFile(String, String, Mat, String);

void printProgress(double percentage);

void displayImage(Mat croppedEar, String imageName);
