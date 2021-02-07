#include "localization.hpp"

#include "utility.hpp"

void initializeCascade(CascadeClassifier &cascade, String name)
{
    if (!cascade.load(name))
    {
        cout << "--(!)Error loading " << name << " cascade classifier.\n";
        exit(1);
    };
}

void detectROI(char *datasetPath, vector<string> imageNames,
               vector<Mat> &images, vector<vector<Rect>> &ROI, bool debugFlag)
{
    // Suppress findFile annoying reminder
    freopen("/dev/null", "w", stderr);
    String leftEarCascadeName = findFile("haarcascade_mcs_leftear.xml");
    String rightEarCascadeName = findFile("haarcascade_mcs_rightear.xml");
    freopen("/dev/null", "w", stderr);

    CascadeClassifier leftCascade;
    CascadeClassifier rightCascade;

    initializeCascade(leftCascade, leftEarCascadeName);
    initializeCascade(rightCascade, rightEarCascadeName);

    double detectedNo = 0.0;
    double visitedNo = 0.0;

    for (int i = 0; i < imageNames.size(); i++)
    {
        String imageName = imageNames[i];
        printProgress(i, imageNames.size());

        ostringstream imgPath;
        imgPath << datasetPath << imageName;

        Mat image = imread(imgPath.str(), IMREAD_COLOR); // Read the file

        if (image.empty())
        { // Check for invalid input
            cerr << "Could not open or find the image" << std::endl;
            continue;
        }
        visitedNo++;
        Mat grayImage;
        cvtColor(image, grayImage, COLOR_BGR2GRAY);
        images.push_back(grayImage);

        // Checking left ear
        if (_detectROI(grayImage, leftCascade, ROI, false, imageName))
        {
            detectedNo++;
            if (debugFlag)
                cout << "Left ear found !\n"
                     << endl;
            continue;
        }
        // Checking right ear
        if (_detectROI(grayImage, rightCascade, ROI, true, imageName))
        {
            detectedNo++;
            if (debugFlag)
                cout << "Right ear found !\n"
                     << endl;
            continue;
        }

        // Horizontally flip the image and interpret it as the opposite ear
        Mat flipped;
        flip(image, flipped, 1);
        cvtColor(flipped, grayImage, COLOR_BGR2GRAY);
        images[images.size() - 1] = grayImage;

        // Checking left (flipped) ear
        if (_detectROI(grayImage, leftCascade, ROI, false, imageName))
        {
            detectedNo++;
            if (debugFlag)
                cout << "Left (flipped) ear found !\n"
                     << endl
                     << flush;
            continue;
        }

        // Checking right (flipped) ear
        if (_detectROI(grayImage, rightCascade, ROI, true, imageName))
        {
            detectedNo++;
            if (debugFlag)
                cout << "Right (flipped) ear found !\n"
                     << endl
                     << flush;
            continue;
        }
        ROI.push_back({});
    }

    cout << "Detection rate " << (float)detectedNo / (float)visitedNo << endl
         << flush;
    return;
}

/**
 * Checks if the provided ROI area lies within the original frame.
 * @param BBox the bounding box denoting the region of interest
 * @param originalFrame the original image
 * @return whether the provided BBox is a valid area in the original image.
 */
bool isValidROI(Rect BBox, Mat originalFrame)
{
    return (BBox.x >= 0 && BBox.y >= 0 && BBox.width >= 0 && BBox.height >= 0 &&
            BBox.x + BBox.width <= originalFrame.cols &&
            BBox.y + BBox.height <= originalFrame.rows);
}

bool _detectROI(Mat &frameGray, CascadeClassifier &cascade,
                vector<vector<Rect>> &ROI, bool rightClassifier,
                String imageName, const int outputSize)
{
    bool display = false; // debug flag
    Mat resultImage = frameGray, resized;

    vector<Rect> ears;
    cascade.detectMultiScale(frameGray, ears);

    // Do not consider matching ROIs whose size is too small
    ears.erase(remove_if(ears.begin(), ears.end(),
                         [&frameGray, outputSize](const Rect &ear) {
                             return !isValidROI(ear, frameGray) ||
                                    min(ear.width, ear.height) < outputSize;
                         }),
               ears.end());

    // Interpret right ears as left ears for the recognition process
    if (rightClassifier)
    {
        flip(resultImage, frameGray, 1);
    }

    if (ears.size() > 0)
    {
        ROI.push_back(ears);
    }
    if (display)
    {
        displayImage(frameGray, imageName);
    }
    return ears.size() > 0;
}
