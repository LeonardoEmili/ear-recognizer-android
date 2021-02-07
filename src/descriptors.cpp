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
void _detectLandmarks(Mat img, vector<Point2d> &ldmk)
{
    static bool first = true;
    static Net net;

    if (first)
    {
        first = false;
        net = readNetFromTensorflow("model-stage1.pb");
        if (net.empty())
        {
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
                    vector<string> imageNames)
{
    for (int i = 0; i < processedROI.size(); i++)
    {
        printProgress(i, processedROI.size());

        auto images = processedROI[i];
        auto imageName = imageNames[i];

        vector<vector<Point2d>> ldmks;
        for (int j = 0; j < images.size(); j++)
        {
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

void reduceDataSparsity(vector<Point2d> points, vector<Point2d> &outPoints,
                        int k)
{
    Point2d centroid(computeCentroid(points));
    // vector<Point2d> _points(points);
    // points.clear();
    outPoints.clear();

    vector<double> distances;
    double mean, std;
    for (auto p : points)
        distances.push_back(norm(p - centroid));
    computeMeanAndStd(distances, mean, std);

    for (int i = 0; i < points.size(); i++)
    {
        if (distances[i] < k * std)
            outPoints.push_back(points[i]);
    }
    if (outPoints.size() < 2)
        outPoints = points;
}

void extractFeatures(vector<vector<Mat>> images,
                     vector<vector<vector<Point2d>>> &landmarks,
                     vector<string> imageNames)
{
    vector<vector<vector<KeyPoint>>> keypoints;
    vector<vector<Mat>> _;
    extractFeatures(images, keypoints, _);

    // Translate KeyPoint -> Point2d
    for (auto image : keypoints)
    {
        vector<vector<Point2d>> imgLdmks;
        for (auto kpts : image)
        {
            vector<Point2d> ldmks;
            for (auto k : kpts)
            {
                ldmks.push_back(Point2d((double)k.pt.x, (double)k.pt.y));
            }
            imgLdmks.push_back(ldmks);
        }
        landmarks.push_back(imgLdmks);
    }

    vector<vector<vector<Point2d>>> outLandmarks;
    for (auto image : landmarks)
    {
        vector<vector<Point2d>> outLdmks;
        for (auto ldmk : image)
        {
            vector<Point2d> outLdmk;
            reduceDataSparsity(ldmk, outLdmk, 3);
            outLdmks.push_back(outLdmk);
        }
        outLandmarks.push_back(outLdmks);
    }

    for (int i = 0; i < images.size(); i++)
    {
        auto image = images[i];
        auto imageName = imageNames[i];
        for (int j = 0; j < image.size(); j++)
        {
            Mat outImage;
            drawLandmarks(image[j], landmarks[i][j], outImage);
            drawLandmarks(outImage, outLandmarks[i][j], outImage,
                          Scalar(0, 255, 0), 5);
            landmarks[i][j] = outLandmarks[i][j];
            // Exporting landmark image
            String id = to_string(i) + "_" + to_string(j);
            writeToFile(imageName, LANDMARK_PATH, outImage, id);
        }
    }
}

void exportFeatures(vector<Mat> descriptors, vector<string> imageNames,
                    const string path)
{
    FileStorage fs(path, FileStorage::WRITE);
    fs << FEATURES_KEY << descriptors << NAMES_KEY << imageNames;
    fs.release();
}

void importFeatures(vector<Mat> &descriptors, vector<string> &imageNames,
                    const string path)
{
    FileStorage fs(path, FileStorage::READ);
    fs[FEATURES_KEY] >> descriptors;
    fs[NAMES_KEY] >> imageNames;
    fs.release();
}

void extractFeatures(vector<vector<Mat>> images, vector<Mat> &descriptors,
                     vector<string> &imageNames, int edgeThreshold,
                     InputArray mask)
{
    vector<vector<vector<KeyPoint>>> keypoints;
    vector<vector<Mat>> tmpDescriptors;
    extractFeatures(images, keypoints, tmpDescriptors);

    vector<string> outNames(imageNames);
    imageNames.clear();
    descriptors.clear();

    // Flatten the descriptor matrix to a 1D vector, where each
    // element corresponds 1:1 to an element of vector imageNames
    for (int i = 0; i < tmpDescriptors.size(); i++)
    {
        int idx = outNames[i].rfind(".");
        for (auto descriptor : tmpDescriptors[i])
        {
            descriptors.push_back(descriptor);
            imageNames.push_back(outNames[i].substr(0, idx));
        }
    }
}

void extractFeatures(vector<vector<Mat>> images,
                     vector<vector<vector<KeyPoint>>> &keypoints,
                     vector<vector<Mat>> &descriptors, int edgeThreshold,
                     InputArray mask)
{
    Ptr<FeatureDetector> detector = ORB::create(500, 1.2f, 8, edgeThreshold);

    for (int i = 0; i < images.size(); i++)
    {
        printProgress(i, images.size());
        auto image = images[i];
        vector<Mat> imageDescriptors;
        vector<vector<KeyPoint>> imageKeypoints;

        for (auto roi : image)
        {
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
                        int normType, float ratio, bool crossCheck)
{
    vector<vector<DMatch>> matches;
    Ptr<DescriptorMatcher> matcher = BFMatcher::create(normType, crossCheck);
    matcher->knnMatch(queryDescriptors, objectDescriptors, matches, 2);

    vector<DMatch> goodMatches;
    for (auto match : matches)
    {
        if (match[0].distance < ratio * match[1].distance)
        {
            goodMatches.push_back(match[0]);
        }
    }
    return (float)goodMatches.size() / (float)matches.size();
}

void logSimilarities(Mat queryDescriptor, vector<Mat> imageDescriptors,
                     String queryName, vector<string> imageNames,
                     bool filterByPrefix)
{
    cout << "\nSimilarities with " << queryName << endl;
    vector<string> outNames;
    vector<float> outScores;
    for (int i = 0; i < imageDescriptors.size(); i++)
    {
        auto objectDescriptor = imageDescriptors[i];
        auto imageName = imageNames[i];
        float score = computeSimilarity(queryDescriptor, objectDescriptor);

        string queryPrefix = queryName.substr(0, 3);
        if (!filterByPrefix || startsWith(imageName, queryPrefix))
        {
            outScores.push_back(score);
            outNames.push_back(imageName);
        }
    }
    for (auto i : argSort(outScores, false))
    {
        printf("%.6f - %s\n", outScores[i], outNames[i].c_str());
    }
}

double calculateVerificationGAR(vector<Mat> imageDescriptors, vector<string> imageNames,
                                double threshold)
{
    // Since we only attempt with the given identity templates, this corresponds to the
    // number of true acceptances and false rejections (so false acceptances and true
    // rejections are not considered).
    double attemptsNo = 0.0;
    double acceptancesNo = 0.0;

    // Number of templates per identity
    std::map<std::string, int> templatesNoPerIdentity;

    for (int i = 0; i < imageNames.size(); i++)
    {
        string identityPrefix = imageNames[i].substr(0, 3);

        if (templatesNoPerIdentity.find(identityPrefix) == templatesNoPerIdentity.end())
        {
            templatesNoPerIdentity[identityPrefix] = 0;
        }
        templatesNoPerIdentity[identityPrefix] += 1;

        bool accepted = false;
        for (int j = 0; j < imageNames.size(); j++)
        {
            // Avoiding self comparison
            if (i == j)
            {
                continue;
            }
            if (!startsWith(imageNames[j], identityPrefix))
            {
                continue;
            }
            if (1 - computeSimilarity(imageDescriptors[i], imageDescriptors[j]) <= threshold)
            {
                accepted = true;
            }
        }
        if (accepted)
        {
            acceptancesNo++;
        }
    }

    for (auto x : templatesNoPerIdentity)
    {
        if (x.second > 1)
        {
            attemptsNo += x.second;
        }
    }

    //cout << acceptancesNo << "\n";
    //cout << attemptsNo << "\n";

    return acceptancesNo / attemptsNo;
}

double calculateVerificationFAR(vector<Mat> imageDescriptors, vector<string> imageNames,
                                double threshold)
{
    // Since we only attempt with templates belonging to identities different from
    //  the given one, this corresponds to the number of false acceptances and true
    //  rejections (so true acceptances and false rejections are not considered).
    double attemptsNo = 0.0;
    double acceptancesNo = 0.0;

    for (int i = 0; i < imageNames.size(); i++)
    {
        // map for this probe: identity -> acceptance status
        // initially it is not accepted by any identity
        // when one identity accepts it, it turns to true
        std::map<std::string, bool> acceptedForIdentity;

        string identityPrefix = imageNames[i].substr(0, 3);
        for (int j = 0; j < imageNames.size(); j++)
        {
            if (startsWith(imageNames[j], identityPrefix))
            {
                continue;
            }
            string templateIdentityPrefix = imageNames[j].substr(0, 3);
            // Initializing map entry for this identity
            if (acceptedForIdentity.find(templateIdentityPrefix) == acceptedForIdentity.end())
            {
                acceptedForIdentity[templateIdentityPrefix] = false;
                attemptsNo++;
            }
            if (1.0 - computeSimilarity(imageDescriptors[i], imageDescriptors[j]) <= threshold)
            {
                if (!acceptedForIdentity[templateIdentityPrefix])
                {
                    acceptedForIdentity[templateIdentityPrefix] = true;
                    acceptancesNo++;
                }
            }
        }
    }
    //cout << acceptancesNo << "\n";
    //cout << attemptsNo << "\n";

    return acceptancesNo / attemptsNo;
}