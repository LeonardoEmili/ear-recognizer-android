package com.getchlabs.earrecognizer.recognition

import org.opencv.core.*
import org.opencv.core.Core.BORDER_REPLICATE
import org.opencv.core.CvType.CV_16F
import org.opencv.core.CvType.CV_8UC1
import org.opencv.imgproc.Imgproc.*
import java.lang.Math.atan2

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
            //croppedEar = croppedEar.adjustROI(ear.y, croppedEar.rows()-(ear.y+ear.height),
              //      ear.x, croppedEar.cols()-(ear.x+ear.width))
        croppedEar =  Mat(croppedEar, ear)

        var outputImg: Mat = croppedEar

            resize(outputImg, resized, Size(outputSize.toDouble(), outputSize.toDouble()));


        outputImages.add(resized)
    }
}


fun alignImages(processedROI: ArrayList<ArrayList<Mat?>>,
                paddingPercentages: ArrayList<ArrayList<Double>>,
                landmarks: ArrayList<ArrayList<ArrayList<Point>>>)
{


        var images = processedROI[0]
        var ldmks = landmarks[0]
        var paddingPercs = paddingPercentages[0];
        for (j in 0.until(images.size))
        {
            var image = images[j]
            var ldmk = ldmks[j]
            var paddingPercentage = paddingPercs[j]

            alignImage(image, paddingPercentage, ldmk)
        }

}

fun alignImage(image: Mat?, paddingPercentage: Double,  landmarks: ArrayList<Point>,
               outputSize: Int = 96)
{
    image!!
    // Fitting line among landmark points
    var line = Mat()
    var landmarkss = Mat.zeros(landmarks.size, 2, CV_8UC1 )
    for (i in 0.until(landmarks.size)) {
        landmarkss.put(i,0, landmarks[i].x)
        landmarkss.put(i,1, landmarks[i].y)
    }
    fitLine(landmarkss, line, DIST_L2, 0.0, 0.01, 0.01);
    var radians= kotlin.math.atan2(line.get(1,0)[0], line.get(0,0)[0])

    // vector -> radians = atan2(vy,vx)
    // Ears tilted counter clockwise wrt vertical line -> vector of type (y
    // = 0.381557, x = 0.924345), radians angle of type 1.1793 (starting
    // from left and going clockwise) Ears tilted clockwise wrt vertical
    // line -> vector of type (y = 0.148791, x = -0.988869), radians angle
    // of type -1.4214 (starting from left and going clockwise)
    var angle = (if (radians >= 0)  -1.0 else 1.0) * 90.0 +
    radians * (180.0 / 3.141592653589793238463)


    // get rotation matrix for rotating the image around its center in pixel
    // coordinates
    var center = Point((image.cols() - 1.0) / 2.0, (image.rows() - 1.0) / 2.0)
    var rot = getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle, center not relevant
    var bbox = RotatedRect(Point(), image.size(), angle).boundingRect();
    // adjust transformation matrix
    rot.put(0, 2, rot.get(0,2)[0] + (bbox.width / 2.0 - image.cols() / 2.0));
    rot.put(1, 2, rot.get(1,2)[0] + (bbox.height / 2.0 - image.rows() / 2.0));



    // Rotating ear image with interpolation
    var rotated = Mat()
    warpAffine(image, rotated, rot, bbox.size(), INTER_LANCZOS4,
            BORDER_REPLICATE);

    // Zooming using padding
    //
    var croppedEar = Mat()
    rotated.copyTo(croppedEar)
    var cropped = Rect()
    var allowanceFactor = 1.2
    var padding: Int = (paddingPercentage * rotated.cols() / allowanceFactor).toInt()
    cropped.x = padding;
    cropped.y = padding;
    cropped.width = rotated.cols() - 2 * padding;
    cropped.height = rotated.rows() - 2 * padding;
    //croppedEar = croppedEar.adjustROI(cropped.y, croppedEar.rows()-(cropped.y+cropped.height),
      //      cropped.x, croppedEar.cols()-(cropped.x+cropped.width))
    croppedEar =  Mat(croppedEar, cropped)


    // Resizing rotated image
    resize(croppedEar, image, Size(outputSize.toDouble(), outputSize.toDouble()));

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