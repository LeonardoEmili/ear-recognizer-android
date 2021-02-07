package com.getchlabs.earrecognizer.recognition

import org.opencv.core.Mat
import org.opencv.core.Rect
import org.opencv.core.Size
import org.opencv.imgproc.Imgproc.resize

fun cropAndResize(ROI: ArrayList<Rect>,  outputImages: ArrayList<Mat?>,
paddingPercentages: ArrayList<Double>, grayImages: ArrayList<Mat?> )
{
    val outputSize = 96
    for (i in 0.until(ROI.size))
    {
        var ear = ROI[i]
        var croppedEar = grayImages[0]
         var resized = Mat()

            // Setting padding around the ROI to perform a smoother rotation (it will be later removed by zooming)
            var padding = 80.0
            // Checking that the padding does not get out of the image
            if (!(ear.x - padding >= 0 && ear.y - padding >= 0 && ear.x + ear.width + padding <
                            croppedEar!!.cols() && ear.y + ear.height + padding < croppedEar.rows()))
            {
                padding = listOf(ear.x - 1.0, ear.y - 1.0,
                        croppedEar!!.cols() - (ear.x + ear.width) - 1.0,
                        croppedEar.rows() - (ear.y + ear.height) - 1.0).min()!!
            }
            ear.x -= padding.toInt()
            ear.y -= padding.toInt()
            ear.height += 2 * padding.toInt()
            ear.width += 2 * padding.toInt()

            var paddingPercentage: Double = padding / croppedEar.cols();
        paddingPercentages.add(paddingPercentage)

            // TODO: check this function call
            croppedEar = croppedEar.adjustROI(ear.y, croppedEar.rows()-ear.y+ear.height,
                    ear.x, croppedEar.cols()-ear.x+ear.width)
            var outputImg: Mat = croppedEar

            resize(outputImg, resized, Size(outputSize.toDouble(), outputSize.toDouble()));


        outputImages.add(resized)
    }
}

/*
void alignImages(vector<vector<Mat>> &processedROI, vector<vector<double>> &paddingPercentages,
vector<vector<vector<Point2d>>> landmarks,
vector<string> imageNames)
{
    for (int i = 0; i < processedROI.size(); i++)
    {
        printProgress(i, processedROI.size());

        auto images = processedROI[i];
        auto imageName = imageNames[i];
        auto ldmks = landmarks[i];
        auto paddingPercs = paddingPercentages[i];
        for (int j = 0; j < images.size(); j++)
        {
            auto image = images[j];
            auto ldmk = ldmks[j];
            auto paddingPercentage = paddingPercs[j];
            String id = to_string(i) + "_" + to_string(j);

            alignImage(image, paddingPercentage, ldmk, imageName, id);
        }
    }
}

void alignImage(Mat &image, double paddingPercentage, vector<Point2d> landmarks, String imageName,
String id, int outputSize)
{
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
    Point2f center((image.cols - 1) / 2.0, (image.rows - 1) / 2.0);
    Mat rot = getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle, center not relevant
    Rect2f bbox = RotatedRect(Point2f(), image.size(), angle).boundingRect2f();
    // adjust transformation matrix
    rot.at<double>(0, 2) += bbox.width / 2.0 - image.cols / 2.0;
    rot.at<double>(1, 2) += bbox.height / 2.0 - image.rows / 2.0;

    // Rotating ear image with interpolation
    Mat rotated;
    warpAffine(image, rotated, rot, bbox.size(), INTER_LANCZOS4,
            BORDER_REPLICATE);

    // Zooming using padding
    Mat croppedEar(rotated);
    Rect cropped;
    double allowanceFactor = 1.2;
    int padding = paddingPercentage * rotated.cols / allowanceFactor;
    cropped.x = padding;
    cropped.y = padding;
    cropped.width = rotated.cols - 2 * padding;
    cropped.height = rotated.rows - 2 * padding;
    croppedEar = croppedEar(cropped);

    // Resizing rotated image
    resize(croppedEar, image, Size(outputSize, outputSize));

    // Exporting rotated image
    writeToFile(imageName, ROTATED_PATH, image, id);
}
*/