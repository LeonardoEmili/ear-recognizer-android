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
        printProgress(i, processedROI.size());

        auto images = processedROI[i];
        auto imageName = imageNames[i];

        vector<vector<Point2d>> ldmks;
        for (int j = 0; j < images.size(); j++) {
            auto imageROI = images[j];
            vector<Point2d> ldmk;
            _detectLandmarks(imageROI, ldmk);
            ldmks.push_back(ldmk);

            Mat outImage;
            drawLandmarks(imageROI, ldmk, outImage);
            // Exporting landmark image
            String id = to_string(i) + "_" + to_string(j);
            writeToFile(imageName, LANDMARK_PATH, outImage, id);
        }
        landmarks.push_back(ldmks);
    }
}

void extractFeatures(vector<vector<Mat>> images,
                     vector<vector<vector<Point2d>>> &landmarks,
                     vector<string> imageNames) {
    vector<vector<vector<KeyPoint>>> keypoints;
    vector<vector<Mat>> _;
    extractFeatures(images, keypoints, _);

    // Translate KeyPoint -> Point2d
    for (auto image : keypoints) {
        vector<vector<Point2d>> imgLdmks;
        for (auto kpts : image) {
            vector<Point2d> ldmks;
            for (auto k : kpts) {
                ldmks.push_back(Point2d((double)k.pt.x, (double)k.pt.y));
            }
            imgLdmks.push_back(ldmks);
        }
        landmarks.push_back(imgLdmks);
    }

    for (int i = 0; i < images.size(); i++) {
        auto image = images[i];
        auto imageName = imageNames[i];
        for (int j = 0; j < image.size(); j++) {
            Mat outImage;
            drawLandmarks(image[j], landmarks[i][j], outImage);
            // Exporting landmark image
            String id = to_string(i) + "_" + to_string(j);
            writeToFile(imageName, LANDMARK_PATH, outImage, id);
        }
    }
}

void extractFeatures(vector<vector<Mat>> images,
                     vector<vector<vector<KeyPoint>>> &keypoints,
                     vector<vector<Mat>> &descriptors, int edgeThreshold,
                     InputArray mask) {
    Ptr<FeatureDetector> detector = ORB::create(500, 1.2f, 8, edgeThreshold);

    for (int i = 0; i < images.size(); i++) {
        printProgress(i, images.size());
        auto image = images[i];
        vector<Mat> imageDescriptors;
        vector<vector<KeyPoint>> imageKeypoints;

        for (auto roi : image) {
            vector<KeyPoint> ROIkeypoints;
            Mat roiDescriptors;

            detector->detectAndCompute(roi, mask, ROIkeypoints, roiDescriptors);
            imageDescriptors.push_back(roiDescriptors);
            imageKeypoints.push_back(ROIkeypoints);
        }
        descriptors.push_back(imageDescriptors);
        keypoints.push_back(imageKeypoints);
    }
}

float computeSimilarity(Mat queryDescriptors, Mat objectDescriptors,
                        int normType, float ratio, bool crossCheck) {
    vector<vector<DMatch>> matches;
    Ptr<DescriptorMatcher> matcher = BFMatcher::create(normType, crossCheck);
    matcher->knnMatch(queryDescriptors, objectDescriptors, matches, 2);

    vector<DMatch> goodMatches;
    for (auto match : matches) {
        if (match[0].distance < ratio * match[1].distance) {
            goodMatches.push_back(match[0]);
        }
    }
    return (float)goodMatches.size() / (float)matches.size();
}

void logSimilarities(Mat queryDescriptor, vector<vector<Mat>> imageDescriptors,
                     String queryName, vector<string> imageNames,
                     bool filterByPrefix) {
    cout << "\nSimilarities with " << queryName << endl;
    float bestScore = 0;
    int bestCandidate = 0;

    vector<string> outNames;
    vector<float> outScores;
    for (int i = 0; i < imageDescriptors.size(); i++) {
        auto image = imageDescriptors[i];
        auto imageName = imageNames[i];
        for (auto objectDescriptor : image) {
            float score = computeSimilarity(queryDescriptor, objectDescriptor);
            if (score > bestScore && score < 1) {
                bestScore = score;
                bestCandidate = i;
            }
            string queryPrefix = queryName.substr(0, 3);
            if (!filterByPrefix || startsWith(imageName, queryPrefix)) {
                outScores.push_back(score);
                outNames.push_back(imageName);
            }
        }
    }
    for (auto i : argSort(outScores, false)) {
        printf("%.6f - %s\n", outScores[i], outNames[i].c_str());
    }
}