#include <dirent.h>

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/objdetect.hpp>
#include <opencv2/core/utility.hpp>

#include "localization.hpp"
#include "utility.hpp"

using namespace cv;
using namespace std;
using namespace samples;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << " Usage: " << argv[0] << " path to dataset" << endl;
        return -1;
    }

    int datasetSize = getDatasetSize(argv[1]);

    char *datasetPath = argv[1];

    cropAndFlipImages(datasetPath);

    return 0;
}