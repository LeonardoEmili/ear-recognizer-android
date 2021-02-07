package com.getchlabs.earrecognizer.recognition

import android.content.Context
import android.content.Context.MODE_PRIVATE
import android.content.Context.MODE_WORLD_READABLE
import org.opencv.core.Core.findFile
import org.opencv.core.Core.flip
import org.opencv.core.Mat
import org.opencv.core.MatOfRect
import org.opencv.core.Rect
import org.opencv.imgproc.Imgproc.*
import org.opencv.objdetect.CascadeClassifier
import java.io.File

fun initializeCascade(cascade: CascadeClassifier? , name: String): Boolean {
    if (!cascade!!.load(name)) {
        println("--(!)Error loading " + name + " cascade classifier.")
        return false
    }
    return true
}

fun detectROI(image: ArrayList<Mat?>, ROI: ArrayList<Rect>, debugFlag: Boolean, context: Context): Boolean {
    // Suppress findFile annoying reminder
    //freopen("/dev/null", "w", stderr);
    //val leftEarCascadeName: String = findFile("haarcascade_mcs_leftear.xml")
    //val rightEarCascadeName: String = findFile("haarcascade_mcs_rightear.xml")
    //freopen("/dev/null", "w", stderr);

    /*var  leftCascade: CascadeClassifier? = null
    var rightCascade: CascadeClassifier? = null

    if (!initializeCascade(leftCascade, leftEarCascadeName)) return false
    if (!initializeCascade(rightCascade, rightEarCascadeName)) return false*/
    saveHaarCascadesAsFiles(context)

    val cascadeDir: File = context.getDir("haarcascades", Context.MODE_PRIVATE)
    var leftFile = File(cascadeDir, "haarcascade_mcs_leftear.xml")
    val leftPath = leftFile.absolutePath
    var rightFile = File(cascadeDir, "haarcascade_mcs_rightear.xml")
    val rightPath = rightFile.absolutePath
    var leftCascade = CascadeClassifier(leftPath)
    leftCascade.load(leftPath)
    var rightCascade = CascadeClassifier(rightPath)
    rightCascade.load(rightPath)

    var detected = false


        if (image[0]!!.empty()) {  // Check for invalid input
            println("Could not open or find the image")
        }

        var grayImage = arrayListOf(Mat())
        cvtColor(image[0], grayImage[0], COLOR_BGR2GRAY);
        image[0] = grayImage[0]

    val outputSize = 96

        // Checking left ear
        if (_detectROI(grayImage, leftCascade, ROI, false, outputSize)) {
            detected = true
            if (debugFlag) println("Left ear found !")
            return detected
        }
        // Checking right ear
        if (_detectROI(grayImage, rightCascade, ROI, true, outputSize)) {
            detected = true
            if (debugFlag) println("Right ear found !")
            return detected

        }

        // Horizontally flip the image and interpret it as the opposite ear
        var flipped = Mat();
        flip(image[0], flipped, 1);
        //cvtColor(flipped, grayImage, COLOR_BGR2GRAY);
        image[0] = flipped //greyImage

        // Checking left (flipped) ear
        if (_detectROI(grayImage, leftCascade, ROI, false, outputSize)) {
            detected = true
            if (debugFlag) println("Left (flipped) ear found !")
            return detected


        }

        // Checking right (flipped) ear
        if (_detectROI(grayImage, rightCascade, ROI, true, outputSize)) {
            detected = true
                if (debugFlag) println("Right (flipped) ear found !")
            return detected

        }

    return detected
}



/**
 * Checks if the provided ROI area lies within the original frame.
 * @param BBox the bounding box denoting the region of interest
 * @param originalFrame the original image
 * @return whether the provided BBox is a valid area in the original image.
 */
fun isValidROI(BBox: Rect, originalFrame: Mat): Boolean {
    return (BBox.x >= 0 && BBox.y >= 0 && BBox.width >= 0 && BBox.height >= 0 &&
            BBox.x + BBox.width <= originalFrame.cols() &&
            BBox.y + BBox.height <= originalFrame.rows())
}

fun _detectROI(frameGray: ArrayList<Mat>, cascade: CascadeClassifier?,
ROI: ArrayList<Rect>, rightClassifier: Boolean, outputSize: Int): Boolean {
    var resultImage = frameGray[0]

    var ears = MatOfRect()


    cascade!!.detectMultiScale(frameGray[0], ears);


    var earss = arrayListOf<Rect>()


    ears.toList().forEach {
        if (isValidROI(it, frameGray[0]) &&
            arrayListOf(it.width, it.height).min()!! >= outputSize) {
            earss.add(it)
        }
    }

    // Interpret right ears as left ears for the recognition process
    if (rightClassifier) {
        flip(resultImage, frameGray[0], 1);
    }

    if (earss.size > 0) {
        earss.forEach{ROI.add(it)}
    }

    return earss.size > 0;
}

