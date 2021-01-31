#include <dirent.h>
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utils/filesystem.hpp>

using namespace cv;
using namespace std;

vector<string> readDataset(char *);
string exec(const char *);
int getDatasetSize(char *);
void writeToFile(String, String, Mat, int);