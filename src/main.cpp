#include "descriptors.hpp"
#include "localization.hpp"
#include "utility.hpp"

/**
 *  Evaluates the system. 
 */
void evaluate()
{
    vector<Mat> descriptors;
    vector<string> imageNames;
    importFeatures(descriptors, imageNames);
    /*double threshold = 1.0;
    for (int i = 0; i < 10; i++)
    {
        cout << "Threshold: " << threshold << "\n";
        double gar = calculateVerificationGAR(descriptors, imageNames, threshold);
        cout << "GAR: " << gar << "\n";
        double far = calculateVerificationFAR(descriptors, imageNames, threshold);
        cout << "FAR: " << far << "\n\n";
        threshold -= 0.02;
    }*/
    vector<double> thresholds;
    vector<double> GARs;
    vector<double> FARs;
    double threshold = 1.00;
    while (threshold >= 0.0)
    {
        cout << "Threshold: " << threshold << "\n";
        thresholds.push_back(threshold);
        double gar = calculateVerificationGAR(descriptors, imageNames, threshold);
        cout << "GAR: " << gar << "\n";
        GARs.push_back(gar);
        double far = calculateVerificationFAR(descriptors, imageNames, threshold);
        cout << "FAR: " << far << "\n\n";
        FARs.push_back(far);
        threshold -= 0.01;
    }
    cout << "Thresholds: ";
    for (double threshold : thresholds)
    {
        cout << threshold << ", ";
    }
    cout << "\nGARs: ";
    for (double gar : GARs)
    {
        cout << gar << ", ";
    }
    cout << "\nFARs: ";
    for (double far : FARs)
    {
        cout << far << ", ";
    }
    cout << "\n";
}

/**
 * Execute the actual main function from an helper function to be able
 * to see exceptions on MacOS too.
 */
int f(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << " Usage: " << argv[0] << " path to dataset" << endl;
        return -1;
    }
    if (argc == 3)
    {
        evaluate();
        return 0;
    }

    char *datasetPath = argv[1];
    vector<string> imageNames = readDataset(datasetPath);

    vector<Mat> grayImages;
    vector<vector<Rect>> ROI;
    cout << "Detecting Regions of Interest (ROI) ...\n"
         << flush;
    detectROI(datasetPath, imageNames, grayImages, ROI, false);

    cout << "\nNormalizing input images (cropping and resizing) ...\n"
         << flush;
    vector<vector<Mat>> processedROI;
    vector<vector<double>> paddingPercentages;
    cropAndResize(ROI, processedROI, paddingPercentages, imageNames, grayImages);

    cout << "\nApplying landmark detection ...\n"
         << flush;
    vector<vector<vector<Point2d>>> landmarks;
    extractFeatures(processedROI, landmarks, imageNames);
    // detectLandmark(processedROI, landmarks, imageNames);

    cout << "\nAuto-aligning images ...\n"
         << flush;
    alignImages(processedROI, paddingPercentages, landmarks, imageNames);

    cout << "\nExtracting image descriptors ...\n"
         << flush;
    vector<Mat> descriptors;
    extractFeatures(processedROI, descriptors, imageNames);

    exportFeatures(descriptors, imageNames);

    int queryIdx = 1;
    logSimilarities(descriptors[queryIdx], descriptors, imageNames[queryIdx],
                    imageNames, false);

    return 0;
}

int main(int argc, char **argv)
{
    try
    {
        return f(argc, argv);
    }
    catch (exception &e)
    {
        cout << "Unhandled exception thrown: " << e.what() << endl;
    }
    return -1;
}