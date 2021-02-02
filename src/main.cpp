#include <opencv2/features2d.hpp>

#include "descriptors.hpp"
#include "localization.hpp"
#include "utility.hpp"

/**
 * Execute the actual main function from an helper function to be able
 * to see exceptions on MacOS too.
 */
int f(int argc, char **argv) {
    if (argc != 2) {
        cout << " Usage: " << argv[0] << " path to dataset" << endl;
        return -1;
    }

    char *datasetPath = argv[1];
    vector<string> imageNames = readDataset(datasetPath);

    vector<Mat> grayImages;
    vector<vector<Rect>> ROI;
    cout << "Detecting Regions of Interest (ROI) ...\n" << flush;
    detectROI(datasetPath, imageNames, grayImages, ROI, false);

    cout << "\nNormalizing input images (cropping and resizing) ...\n" << flush;
    vector<vector<Mat>> processedROI;
    cropAndResize(ROI, processedROI, imageNames, grayImages);

    cout << "\nApplying landmark detection ...\n" << flush;
    vector<vector<vector<Point2d>>> landmarks;
    detectLandmark(processedROI, landmarks, imageNames);

    cout << "\nAuto-aligning images ...\n" << flush;
    alignImages(processedROI, landmarks, imageNames);

    cout << "\nExtracting image descriptors ...\n" << flush;
    vector<vector<Mat>> descriptors;
    extractFeatures(processedROI, descriptors);

    int queryIdx = 0;
    logSimilarities(descriptors[queryIdx][0], descriptors, imageNames[queryIdx],
                    imageNames, true);

    return 0;
}

int main(int argc, char **argv) {
    try {
        return f(argc, argv);
    } catch (exception &e) {
        cout << "Unhandled exception thrown: " << e.what() << endl;
    }
    return -1;
}