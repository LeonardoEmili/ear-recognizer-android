#include "localization.hpp"

#define CROPPED_PATH "generated/cropped/"
#define LANDMARK_PATH "generated/landmark/"
#define ROTATED_PATH "generated/rotated/"

void initializeCascade(CascadeClassifier &cascade, String name) {
    if (!cascade.load(name)) {
        cout << "--(!)Error loading " << name << " cascade classifier.\n";
        exit(1);
    };
}

void rotate() {}

vector<vector<Rect>> cropAndFlipImages(char *datasetPath,
                                       vector<string> imageNames,
                                       bool debugFlag) {
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

    vector<vector<Rect>> ROI;

    // for (String imageName : imageNames) {
    for (int i = 0; i < imageNames.size(); i++) {
        // if (i > 50) break;
        String imageName = imageNames[i];
        float progress = (float)i / (float)(imageNames.size() - 1);
        printProgress(progress);

        ostringstream imgPath;
        imgPath << datasetPath << imageName;

        Mat image = imread(imgPath.str(), IMREAD_COLOR);  // Read the file

        if (image.empty()) {  // Check for invalid input
            cerr << "Could not open or find the image" << std::endl;
            continue;
        }
        visitedNo++;

        // Checking left ear
        if (detectROI(image, leftCascade, ROI, false, imageName)) {
            detectedNo++;
            if (debugFlag) cout << "Left ear found !\n" << endl;
            continue;
        }
        // Checking right ear
        if (detectROI(image, rightCascade, ROI, true, imageName)) {
            detectedNo++;
            if (debugFlag) cout << "Right ear found !\n" << endl;
            continue;
        }

        // Horizontally flip the image and interpret it as the opposite ear
        Mat flipped;
        flip(image, flipped, 1);

        // Checking left (flipped) ear
        if (detectROI(flipped, leftCascade, ROI, false, imageName)) {
            detectedNo++;
            if (debugFlag) cout << "Left (flipped) ear found !\n" << endl;
            continue;
        }

        // Checking right (flipped) ear
        if (detectROI(flipped, rightCascade, ROI, true, imageName)) {
            detectedNo++;
            if (debugFlag) cout << "Right (flipped) ear found !\n" << endl;
            continue;
        }
        ROI.push_back({});
    }

    cout << "\nDetection rate " << (float)detectedNo / (float)visitedNo << endl;
    return ROI;
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

bool detectROI(Mat frame, CascadeClassifier &cascade, vector<vector<Rect>> &ROI,
               bool rightClassifier, String imageName) {
    bool display = false;  // debug flag
    Mat frameGray, resized;
    cvtColor(frame, frameGray, COLOR_BGR2GRAY);
    // equalizeHist( frameGray, frameGray );

    Mat croppedEar(frameGray);
    vector<Rect> ears;
    cascade.detectMultiScale(frameGray, ears);

    const int outputSize = 96;
    // Do not consider matching ROIs whose size is too small
    ears.erase(remove_if(ears.begin(), ears.end(),
                         [&croppedEar](const Rect &ear) {
                             return !isValidROI(ear, croppedEar) ||
                                    min(ear.width, ear.height) < outputSize;
                         }),
               ears.end());

    for (int i = 0; i < ears.size(); ++i) {
        Rect ear = ears[i];
        // if (!isValidROI(ear, croppedEar)) {continue;}

        croppedEar = croppedEar(ear);
        Mat outputImg = croppedEar;
        // Flipping if ear is the right one. We only keep left ears for
        // recognition
        if (rightClassifier) {
            flip(croppedEar, outputImg, 1);
        }
        resize(outputImg, resized, Size(outputSize, outputSize));

        writeToFile(imageName, CROPPED_PATH, resized, i);

        // Detecting landmarks
        vector<Point2d> landmarks;
        detectLandmarks(resized, landmarks);

        // Fitting line among landmark points
        Vec4f line;
        fitLine(landmarks, line, DIST_L2, 0, 0.01, 0.01);
        double radians = atan2(line[1], line[0]);
        // vector -> radians = atan2(vy,vx)
        // Ears tilted counter clockwise wrt vertical line -> vector of type (y
        // = 0.381557, x = 0.924345), radians angle of type 1.1793 (starting
        // from left and going clockwise) Ears tilted clockwise wrt vertical
        // line -> vector of type (y = 0.148791, x = -0.988869), radians angle
        // of type -1.4214 (starting from left and going clockwise)
        double angle = (radians >= 0 ? -1 : 1) * 90.0 +
                       radians * (180.0 / 3.141592653589793238463);

        // get rotation matrix for rotating the image around its center in pixel
        // coordinates
        cv::Point2f center((resized.cols - 1) / 2.0, (resized.rows - 1) / 2.0);
        cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
        // determine bounding rectangle, center not relevant
        cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), resized.size(), angle)
                              .boundingRect2f();
        // adjust transformation matrix
        rot.at<double>(0, 2) += bbox.width / 2.0 - resized.cols / 2.0;
        rot.at<double>(1, 2) += bbox.height / 2.0 - resized.rows / 2.0;

        // Rotating ear image with interpolation
        cv::Mat rotated;
        cv::warpAffine(resized, rotated, rot, bbox.size(), INTER_LANCZOS4,
                       BORDER_REPLICATE);

        // Drawing landmarks for displaying purposes
        for (Point2d landmark : landmarks) {
            circle(resized, landmark, 5, Scalar(0, 0, 255), FILLED);
        }
        // Exporting landmark image
        writeToFile(imageName, LANDMARK_PATH, resized, i);

        // Resizing rotated image
        resize(rotated, resized, Size(outputSize, outputSize));

        // Exporting rotated image
        writeToFile(imageName, ROTATED_PATH, resized, i);

        /*
        cv::line(resized, Point2d(line[2] - line[0] * 90, line[3] - line[1] *
        90), Point2d(line[2] + line[0] * 90, line[3] + line[1] * 90), Scalar(0,
        255, 0), 10, LINE_8);
        */

        // Displaying image
        if (display)
        // if (true)
        {
            displayDetected(resized);
        }
    }
    if (ears.size() > 0) {
        ROI.push_back(ears);
    }
    return ears.size() > 0;
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
