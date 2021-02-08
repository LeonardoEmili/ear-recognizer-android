#include "utility.hpp"

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

static chrono::steady_clock::time_point START_TIMER;
void printProgress(int iteration, int total) {
    if (iteration == 0) START_TIMER = getCurrentTime();
    double percentage = (float)iteration / (float)(total - 1);
    int val = (int)(percentage * 100);
    int lpad = (int)(percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    auto end = getCurrentTime();
    double elapsed = getElapsedSeconds(START_TIMER, end);
    printf("\r%3d%% [%.*s%*s] %.1fs", val, lpad, PBSTR, rpad, "", elapsed);
    if (iteration == total - 1) printf("\n");
    fflush(stdout);
}

chrono::steady_clock::time_point getCurrentTime() {
    return chrono::steady_clock::now();
}

float getElapsedSeconds(chrono::steady_clock::time_point start,
                        chrono::steady_clock::time_point end) {
    return chrono::duration_cast<std::chrono::microseconds>(end - start)
               .count() /
           (float)(1000 * 1000);
}

/**
 * Read a dataset and return the list of images.
 * @param path where the dataset is located
 * @param outArray the list of images returned as an array
 */
vector<string> readDataset(char *path) {
    vector<string> fnames;
    if (DIR *dir = opendir(path)) {
        int i = 0;
        while (struct dirent *entry = readdir(dir)) {
            // Skip special directories
            if (strcmp(entry->d_name, ".") == 0) continue;
            if (strcmp(entry->d_name, "..") == 0) continue;
            if (entry->d_name[0] == '.') continue;

            fnames.push_back(entry->d_name);
        }
    }
    return fnames;
}

/**
 * Execute a command a return the result.
 * @param cmd is the command to be executed
 * @return the output of the command
 */
string exec(const char *cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

/**
 * Run 'wc -l' to see the size of the dataset.
 * @param path the path to the dataset
 * @return the size of the dataset
 */
int getDatasetSize(char *path) {
    ostringstream cmd;
    cmd << "ls -l1 " << path << " | wc -l";
    return stoi(exec(cmd.str().c_str()));
}

/**
 * Utility function to store images using format imageName_ID.[ext].
 * @param imageName the relative path of the image
 * @param dirName path to the output directory
 * @param image image to be saved
 * @param id the index of the provided ROI within the image (serves as an
 * identifier)
 */
void writeToFile(String imageName, String dirName, Mat image, String id) {
    int extIndex = imageName.find_last_of(".");
    string prefix = imageName.substr(0, extIndex);
    string extension = imageName.substr(extIndex, imageName.length());

    String outputPath = dirName + prefix + "_" + id + extension;
    cv::utils::fs::createDirectories(dirName);
    imwrite(outputPath, image);
}

void displayImage(Mat image, String imageName) {
    imshow(imageName, image);
    waitKey(0);
    destroyAllWindows();
}

bool startsWith(String s, String prefix) { return s.rfind(prefix, 0) == 0; }

vector<size_t> argSort(const vector<float> &v, bool ascending) {
    // initialize original index locations
    vector<size_t> idx(v.size());
    iota(idx.begin(), idx.end(), 0);

    if (ascending) {
        stable_sort(idx.begin(), idx.end(),
                    [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });
    } else {
        stable_sort(idx.begin(), idx.end(),
                    [&v](size_t i1, size_t i2) { return v[i1] > v[i2]; });
    }

    return idx;
}

void drawLandmarks(Mat image, const vector<Point2d> landmarks, Mat &outImage,
                   const Scalar color, int radius) {
    if (image.channels() != 3) cvtColor(image, outImage, COLOR_GRAY2RGB);
    for (Point2d landmark : landmarks) {
        circle(outImage, landmark, radius, color);
    }
}

Point2d computeCentroid(vector<Point2d> points) {
    return accumulate(points.begin(), points.end(), Point2d(0, 0)) *
           (1.0f / points.size());
}

void computeMeanAndStd(vector<double> values, double &mean, double &std) {
    mean = 0.0f, std = 0.0f;
    for (double v : values) mean += v;
    mean = mean * (1.0f / values.size());
    for (double v : values) std += (v - mean) * (v - mean);
    std = sqrt(std * (1.0f / values.size()));
}

bool equalMats(Mat a, Mat b) { return countNonZero((a != b)) == 0; }

/**
 *  Evaluates the system.
 */
void evaluate() {
    vector<Mat> descriptors;
    vector<string> imageNames;
    importFeatures(descriptors, imageNames);
    /*double threshold = 1.0;
    for (int i = 0; i < 10; i++)
    {
        cout << "Threshold: " << threshold << "\n";
        double gar = calculateVerificationGAR(descriptors, imageNames,
    threshold); cout << "GAR: " << gar << "\n"; double far =
    calculateVerificationFAR(descriptors, imageNames, threshold); cout << "FAR:
    " << far << "\n\n"; threshold -= 0.02;
    }*/
    vector<double> thresholds;
    vector<double> GARs;
    vector<double> FARs;
    double threshold = 1.00;
    while (threshold >= 0.0) {
        cout << "Threshold: " << threshold << "\n";
        thresholds.push_back(threshold);
        double gar =
            calculateVerificationGAR(descriptors, imageNames, threshold);
        cout << "GAR: " << gar << "\n";
        GARs.push_back(gar);
        double far =
            calculateVerificationFAR(descriptors, imageNames, threshold);
        cout << "FAR: " << far << "\n\n";
        FARs.push_back(far);
        threshold -= 0.01;
    }
    cout << "Thresholds: ";
    for (double threshold : thresholds) {
        cout << threshold << ", ";
    }
    cout << "\nGARs: ";
    for (double gar : GARs) {
        cout << gar << ", ";
    }
    cout << "\nFARs: ";
    for (double far : FARs) {
        cout << far << ", ";
    }
    cout << "\n";
}

/*
int detectImageAndDrawLine(Mat frame, CascadeClassifier &cascade, bool
rightClassifier, bool display, String imageName)
{
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

    for (int i = 0; i < ears.size(); ++i)
    {
        Rect ear = ears[i];
        if (!isValidROI(ear, croppedEar))
        {
            continue;
        }

        croppedEar = croppedEar(ear);
        Mat outputImg = croppedEar;
        // Flipping if ear is the right one. We only keep left ears for
        // recognition
        if (rightClassifier)
        {
            flip(croppedEar, outputImg, 1);
        }
        resize(outputImg, resized, Size(outputSize, outputSize));

        writeToFile(imageName, CROPPED_PATH, resized, i);

        vector<Point2d> landmarks;
        detectLandmarks(resized, landmarks);


        for (Point2d landmark : landmarks)
        {
            circle(grayBGR, landmark, 5, Scalar(0, 0, 255), FILLED);
        }

Vec4f line;
fitLine(landmarks, line, DIST_L2, 0, 0.01, 0.01);
printf("%f, %f\n", line[0], line[1]);
double radians = atan2(line[1], line[0]);
cout << radians << "\n";
// vector -> radians = atan2(vy,vx)
// Ears tilted counter clockwise wrt vertical line -> vector of type (y =
0.381557, x = 0.924345),
// radians angle of type 1.1793 (starting from left and going clockwise)
// Ears tilted clockwise wrt vertical line -> vector of type (y = 0.148791, x =
-0.988869),
// radians angle of type -1.4214 (starting from left and going clockwise)

double angle = (radians >= 0 ? -1 : 1) * 90.0 + radians * (180.0
/ 3.141592653589793238463); cout << angle << "\n";

// get rotation matrix for rotating the image around its center in pixel
coordinates cv::Point2f center((resized.cols - 1) / 2.0, (resized.rows - 1)
/ 2.0); cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
// determine bounding rectangle, center not relevant
cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), resized.size(),
angle).boundingRect2f();
// adjust transformation matrix
rot.at<double>(0, 2) += bbox.width / 2.0 - resized.cols / 2.0;
rot.at<double>(1, 2) += bbox.height / 2.0 - resized.rows / 2.0;

cv::Mat dst;
cv::warpAffine(resized, dst, rot, bbox.size(), INTER_LANCZOS4,
BORDER_REPLICATE); resize(dst, resized, Size(outputSize, outputSize));

vector<Point2d> l2;

detectLandmarks(resized, l2);

for (Point2d landmark : l2)
{
    circle(resized, landmark, 5, Scalar(0, 0, 255), FILLED);
}

Mat3b grayBGR;
cvtColor(resized, grayBGR, COLOR_GRAY2BGR);

Vec4f line2;
fitLine(l2, line2, DIST_L2, 0, 0.01, 0.01);
cv::line(grayBGR, Point2d(line2[2] - line2[0] * 90, line2[3] - line2[1] * 90),
Point2d(line2[2] + line2[0] * 90, line2[3] + line2[1] * 90), Scalar(0, 255, 0),
5, LINE_8);

writeToFile(imageName, LANDMARK_PATH, grayBGR, i);

// Displaying image
//if (display)
if (true)
{
    displayImage(grayBGR, imageName);
}
}
return ears.size();
}
*/