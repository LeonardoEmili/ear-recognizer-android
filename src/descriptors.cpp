#include "descriptors.hpp"

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
void _detectLandmarks(Mat img, vector<Point2d> &ldmk) {
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

void detectLandmark(vector<vector<Mat>> processedROI,
                       vector<vector<vector<Point2d>>> &landmarks,
                       vector<string> imageNames) {
    for (int i = 0; i < processedROI.size(); i++) {
        float progress = (float)i / (float)(processedROI.size() - 1);
        printProgress(progress);

        auto images = processedROI[i];
        auto imageName = imageNames[i];

        vector<vector<Point2d>> ldmks;
        for (auto imageROI : images) {
            vector<Point2d> ldmk;
            _detectLandmarks(imageROI, ldmk);
            ldmks.push_back(ldmk);
            Mat outImg = imageROI.clone();

            for (Point2d l : ldmk) {
                circle(outImg, l, 5, Scalar(0, 0, 255), FILLED);
            }
            // Exporting landmark image
            writeToFile(imageName, LANDMARK_PATH, outImg, to_string(i));
        }
        landmarks.push_back(ldmks);
    }
}