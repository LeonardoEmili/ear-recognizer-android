#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <dirent.h>

using namespace cv;
using namespace std;

/**
 * Crops images by localizing ears and excluding
 * external part.
 */
void crop_ears();

void localize_ear();