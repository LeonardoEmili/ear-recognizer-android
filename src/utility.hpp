#include <dirent.h>
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;

vector<string> readDataset(char*);
string exec(const char*);
int getDatasetSize(char*);