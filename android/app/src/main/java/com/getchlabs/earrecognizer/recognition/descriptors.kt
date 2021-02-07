package com.getchlabs.earrecognizer.recognition

import org.opencv.core.*
import org.opencv.core.Core.NORM_HAMMING
import org.opencv.features2d.BFMatcher
import org.opencv.features2d.DescriptorMatcher
import org.opencv.features2d.ORB.create
import kotlin.math.pow
import kotlin.math.sqrt


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
/*
fun _detectLandmarks(img: Mat, ldmk: ArrayList<Point>)
{
    var first = true;
    var net = Net()

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
}*/

fun reduceDataSparsity(points: ArrayList<Point>, outPoints: ArrayList<Point>,
                       k: Int)
{
    var centroid = computeCentroid(points)
    // vector<Point2d> _points(points);
    // points.clear();
    outPoints.clear()

    var distances = arrayListOf<Double>()
    for (p in points) {
        // TODO: check these calculations
        var dx = (p.x - centroid.x).pow(2)
        var dy = (p.y - centroid.y).pow(2)
        distances.add(sqrt(dx + dy))
    }
    var mean = distances.average()
    var std = 0.0
    for (v in distances) std += (v - mean) * (v - mean)
    std = sqrt(std * (1.0f / distances.size))


    for (i in 0.until(points.size))
    {
        if (distances[i] < k * std)
            outPoints.add(points[i]);
    }
    if (outPoints.size < 2) {
        outPoints.clear()
        points.forEach { outPoints.add(it) }
    }
}

fun extractFeatures(images: ArrayList<ArrayList<Mat?>>,
                    landmarks: ArrayList<ArrayList<ArrayList<Point>>>)
{
    var keypoints = arrayListOf<MatOfKeyPoint>()
    var _t = arrayListOf<Mat?>()
    extractFeatures(images[0], keypoints, _t);

    // Translate KeyPoint -> Point2d
    for ( image in keypoints)
    {
        var imgLdmks = arrayListOf<ArrayList<Point>>()



        for (kpts in 0.until(image.cols())) {
            var ldmks = arrayListOf<Point>()
            for (k in 0.until(image.rows())) {
                ldmks.add(Point(image.get(k,kpts)[0], image.get(k,kpts)[1]))
            }

            imgLdmks.add(ldmks)
        }
        landmarks.add(imgLdmks);
    }

    var outLandmarks = arrayListOf<ArrayList<ArrayList<Point>>>()
    for ( image in landmarks)
    {
        var outLdmks = arrayListOf<ArrayList<Point>>()
        for (ldmk in image)
        {
            var outLdmk = arrayListOf<Point>()
            reduceDataSparsity(ldmk, outLdmk, 3);
            outLdmks.add(outLdmk)
        }
        outLandmarks.add(outLdmks)
    }


        var image = images[0]
        for (j in 0.until(image.size))
        {
            var outImage = Mat()

            landmarks[0][j] = outLandmarks[0][j];
        }

}




fun extractFeatures(images: ArrayList<ArrayList<Mat?>>, descriptors: ArrayList<Mat?>,
                     edgeThreshold: Int = 10,
                    mask: Mat = Mat())
{
    var keypoints = arrayListOf<MatOfKeyPoint>()
    var tmpDescriptors = ArrayList<Mat?>()
    extractFeatures(images[0], keypoints, tmpDescriptors);

    descriptors.clear();

    tmpDescriptors.forEach { descriptors.add(it) }

}




fun extractFeatures(images: ArrayList<Mat?>,
                    keypoints: ArrayList<MatOfKeyPoint>,
                    descriptors: ArrayList<Mat?>, edgeThreshold: Int = 10,
                    mask: Mat = Mat())
{
    var detector = create(500, 1.2f, 8, edgeThreshold);

    for (i in 0.until(images.size))
    {
        var image = images[i];
        var imageDescriptors = arrayListOf<Mat>()
        var imageKeypoints = arrayListOf<MatOfKeyPoint>()


            var ROIkeypoints = MatOfKeyPoint()
            var roiDescriptors = Mat()

            detector.detectAndCompute(image, mask, ROIkeypoints, roiDescriptors);
            //imageDescriptors.add(roiDescriptors);
            //imageKeypoints.add(ROIkeypoints);

        descriptors.add(roiDescriptors);
        keypoints.add(ROIkeypoints);
    }
}

fun computeSimilarity(queryDescriptors: Mat, objectDescriptors: Mat,
normType: Int = NORM_HAMMING, ratio: Double = 0.75, crossCheck: Boolean = false): Double
{
    var matches = arrayListOf<MatOfDMatch>()
    var matcher = BFMatcher.create(normType, crossCheck);
    matcher.knnMatch(queryDescriptors, objectDescriptors, matches, 2);

    var goodMatches = ArrayList<DMatch>()
    for (match in matches)
    {
        if (match.toList()[0].distance < ratio * match.toList()[1].distance)
        {
            goodMatches.add(match.toList()[0])
        }
    }
    return goodMatches.size.toDouble() /  matches.size.toDouble()
}
