#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>

using namespace cv;
using namespace std;

/**
 * Crops images by localizing ears and excluding
 * external part.
 */
void crop_ears();

void localize_ear();

void initializeCascade(CascadeClassifier&, String);

bool isValidROI(Rect, Mat);