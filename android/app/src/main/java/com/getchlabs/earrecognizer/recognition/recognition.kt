package com.getchlabs.earrecognizer.recognition

import android.content.Context
import android.graphics.Bitmap
import org.opencv.core.Mat
import org.opencv.core.Point
import org.opencv.core.Rect


/**
 * Entry point of recognition process.
 */
fun recognize(bmp: Bitmap, context: Context): Bitmap? {


    var mat = bitmapToMat(bmp)
    var image = arrayListOf<Mat?>(mat)

    var ROI = arrayListOf<Rect>()
    if (!detectROI(image, ROI, false, context)) return null

    var processedROI = arrayListOf<Mat?>()
    var paddingPercentages = arrayListOf<Double>()
    cropAndResize(ROI, processedROI, paddingPercentages, image)

    var landmarks = arrayListOf<ArrayList<ArrayList<Point>>>()
    var processedROI2 = arrayListOf(processedROI)
    extractFeatures(processedROI2, landmarks)

    var paddingPercentages2 = arrayListOf(paddingPercentages)
    alignImages(processedROI2, paddingPercentages2, landmarks);



    return matToBitmap(processedROI2[0][0]!!)

}

