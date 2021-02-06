package com.getchlabs.earrecognizer.recognition

import org.opencv.core.KeyPoint
import org.opencv.core.Mat
import org.opencv.core.MatOfKeyPoint
import org.opencv.core.Point
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

fun extractFeatures(images: ArrayList<Mat?>,
                    landmarks: ArrayList<ArrayList<Point>>)
{
    var keypoints = arrayListOf<MatOfKeyPoint>()
    var _t = arrayListOf<Mat?>()
    extractFeatures(images, keypoints, _t);

    // Translate KeyPoint -> Point2d
    for ( image in keypoints)
    {
        var imgLdmks = arrayListOf<ArrayList<Point>>()

            var ldmks = arrayListOf<Point>()
            for ( k in kpts)
            {
                ldmks.push_back(Point2d((double) k . pt . x, (double) k . pt . y));
            }

        landmarks.add(ldmks);
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
        auto image = images [i];
        auto imageName = imageNames [i];
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