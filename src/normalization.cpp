#include "normalization.hpp"

void cropAndResize(vector<vector<Rect>> &ROI, vector<vector<Mat>> &outputImages,
                   vector<string> imageNames, vector<Mat> grayImages,
                   int outputSize) {
    for (int j = 0; j < ROI.size(); ++j) {
        printProgress(j, ROI.size());

        auto ears = ROI[j];
        auto imageName = imageNames[j];
        Mat croppedEar(grayImages[j]), resized;
        vector<Mat> resizedImages;

        for (int i = 0; i < ears.size(); ++i) {
            Rect ear = ears[i];

            croppedEar = croppedEar(ear);
            Mat outputImg = croppedEar;

            resize(outputImg, resized, Size(outputSize, outputSize));
            resizedImages.push_back(resized);

            String id = to_string(j) + "_" + to_string(i);
            writeToFile(imageName, CROPPED_PATH, resized, id);
        }
        outputImages.push_back(resizedImages);
    }
}

void alignImages(vector<vector<Mat>> &processedROI,
                 vector<vector<vector<Point2d>>> landmarks,
                 vector<string> imageNames) {
    for (int i = 0; i < processedROI.size(); i++) {
        printProgress(i, processedROI.size());

        auto images = processedROI[i];
        auto imageName = imageNames[i];
        auto ldmks = landmarks[i];
        for (int j = 0; j < images.size(); j++) {
            auto image = images[j];
            auto ldmk = ldmks[j];
            String id = to_string(i) + "_" + to_string(j);

            alignImage(image, ldmk, imageName, id);
        }
    }
}

void alignImage(Mat &image, vector<Point2d> landmarks, String imageName,
                String id, int outputSize) {
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

    // Resizing rotated image
    resize(rotated, image, Size(outputSize, outputSize));

    // Exporting rotated image
    writeToFile(imageName, ROTATED_PATH, image, id);
}
