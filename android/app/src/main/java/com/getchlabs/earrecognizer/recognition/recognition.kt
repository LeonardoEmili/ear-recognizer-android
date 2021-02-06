package com.getchlabs.earrecognizer.recognition

import android.content.Context
import android.graphics.Bitmap
import org.opencv.core.Mat
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


    return matToBitmap(processedROI[0]!!)

}

