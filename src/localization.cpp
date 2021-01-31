#include "localization.hpp"

#define CROPPED_PATH "generated/cropped/"
#define LANDMARK_PATH "generated/landmark/"

void initializeCascade(CascadeClassifier &cascade, String name) {
    if (!cascade.load(name)) {
        cout << "--(!)Error loading " << name << " cascade classifier.\n";
        exit(1);
    };
}

int cropAndFlipImages(char *datasetPath, bool debugFlag) {
    // Function findFile always reminds us where it found these files, just
    // annoying to see
    freopen("/dev/null", "w", stderr);
    String leftEarCascadeName = findFile("haarcascade_mcs_leftear.xml");
    String rightEarCascadeName = findFile("haarcascade_mcs_rightear.xml");
    freopen("/dev/null", "w", stderr);

    CascadeClassifier leftEarCascade;
    CascadeClassifier rightEarCascade;

    initializeCascade(leftEarCascade, leftEarCascadeName);
    initializeCascade(rightEarCascade, rightEarCascadeName);

    double detectedNo = 0.0;
    double visitedNo = 0.0;

    bool displayImages = false;

    vector<string> imageNames = readDataset(datasetPath);
    for (String imageName : imageNames) {
        ostringstream imgPath;
        imgPath << datasetPath << imageName;

        Mat image = imread(imgPath.str(), IMREAD_COLOR);  // Read the file

        if (image.empty()) {  // Check for invalid input
            cout << "Could not open or find the image" << std::endl;
            return -1;
        }
        visitedNo++;
        // Checking left ear
        if (detectImage(image, leftEarCascade, false, displayImages,
                        imageName) > 0) {
            detectedNo++;
            if (debugFlag) cout << "Left ear found !\n" << endl;
        } else {
            // Checking right ear
            if (detectImage(image, rightEarCascade, true, displayImages,
                            imageName) > 0) {
                detectedNo++;
                if (debugFlag) cout << "Right ear found !\n" << endl;
            } else {
                /* Trying to flip the given image horizontally and interpet it
                    as the opposite ear*/
                Mat flipped;
                flip(image, flipped, 1);
                // Checking left ear
                if (detectImage(flipped, leftEarCascade, false, displayImages,
                                imageName) > 0) {
                    detectedNo++;
                    if (debugFlag) cout << "Left ear found !\n" << endl;
                } else {
                    // Checking right ear
                    if (detectImage(flipped, rightEarCascade, true,
                                    displayImages, imageName) > 0) {
                        detectedNo++;
                        if (debugFlag) cout << "Right ear found !\n" << endl;
                    }
                }
            }
        }
    }

    cout << "Detection rate ";
    printf("%f\n", detectedNo / visitedNo);

    return 0;
}

/**
 * Checks if the provided ROI area lies within the original frame.
 * @param BBox the bounding box denoting the region of interest
 * @param originalFrame the original image
 * @return whether the provided BBox is a valid area in the original image.
 */
bool isValidROI(Rect BBox, Mat originalFrame) {
    return (BBox.x >= 0 && BBox.y >= 0 && BBox.width >= 0 && BBox.height >= 0 &&
            BBox.x + BBox.width <= originalFrame.cols &&
            BBox.y + BBox.height <= originalFrame.rows);
}

int detectImage(Mat frame, CascadeClassifier &cascade, bool rightClassifier,
                bool display, String imageName) {
    Mat frameGray, resized;
    cvtColor(frame, frameGray, COLOR_BGR2GRAY);
    // equalizeHist( frameGray, frameGray );

    Mat croppedEar(frameGray);
    vector<Rect> ears;
    cascade.detectMultiScale(frameGray, ears);

    const int outputSize = 96;
    // Do not consider matching ROIs whose size is too small
    ears.erase(remove_if(ears.begin(), ears.end(),
                         [](const Rect &ear) {
                             return min(ear.width, ear.height) < outputSize;
                         }),
               ears.end());

    for (int i = 0; i < ears.size(); ++i) {
        Rect ear = ears[i];
        if (!isValidROI(ear, croppedEar)) {
            continue;
        }

        croppedEar = croppedEar(ear);
        Mat outputImg = croppedEar;
        // Flipping if ear is the right one. We only keep left ears for
        // recognition
        if (rightClassifier) {
            flip(croppedEar, outputImg, 1);
        }
        resize(outputImg, resized, Size(outputSize, outputSize));

        writeToFile(imageName, CROPPED_PATH, resized, i);

        vector<Point2d> landmarks;
        detectLandmarks(resized, landmarks);

        for (Point2d landmark : landmarks) {
            circle(resized, landmark, 5, Scalar(0, 0, 255), FILLED);
        }

        writeToFile(imageName, LANDMARK_PATH, resized, i);

        // Displaying image
        if (display) {
            displayDetected(resized);
        }
    }
    return ears.size();
}

void displayDetected(Mat croppedEar) {
    imshow("Ear detection", croppedEar);
    waitKey(0);
    destroyAllWindows();
}

/* ---------------------------------------------------------------------------------------
 */
/* Description: detects landmarks in a left oriented cropped ear image */
/* Parameters: */
/*     Mat img: 96x96 image with a cropped ear */
/*              - left oriented */
/*              - with or without pose variation */
/*     vector<Point2d> &ldmk: vector that receives the 2d coordinates of 55
 * landmarks      */
/*     stage s: FIRST locates landmarks in ears with large pose variations
 */
/*              SECOND locates landmarks in ears with coarse pose
 * normalization
 */
/* Return: none */
/* ---------------------------------------------------------------------------------------
 */
void detectLandmarks(Mat img, vector<Point2d> &ldmk) {
    static bool first = true;
    static Net net;

    if (first) {
        first = false;
        net = readNetFromTensorflow("model-stage1.pb");
        if (net.empty()) {
            cerr << "ERROR: Could not load the CNNs for landmark detection"
                 << endl
                 << flush;
            exit(1);
        }
    }
    Mat result, inputBlob = blobFromImage(img);
    inputBlob /= 255.0;
    net.setInput(inputBlob);
    result = net.forward("ear_ang45_3_sca20_r_tra20_r_e/out/MatMul");
    result *= 48;
    result += 48;

    ldmk.clear();
    for (int i = 0; i < 55; i++)
        ldmk.push_back(Point2d(result.at<float>(0, i * 2),
                               result.at<float>(0, i * 2 + 1)));
}