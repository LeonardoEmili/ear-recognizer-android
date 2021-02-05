package com.getchlabs.earrecognizer.recognition

import android.content.Context
import android.graphics.Bitmap
import android.util.Log
import org.opencv.android.Utils
import org.opencv.core.CvException
import org.opencv.core.CvType
import org.opencv.core.Mat
import org.opencv.core.Scalar
import org.opencv.imgproc.Imgproc
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream


fun bitmapToMat(bmp: Bitmap): Mat {
    val mat = Mat()
    val bmp32 = bmp.copy(Bitmap.Config.ARGB_8888, true)
    Utils.bitmapToMat(bmp32, mat)
    return mat
}

fun matToBitmap(mat: Mat): Bitmap {
    var bmp: Bitmap? = null
    val tmp = Mat(mat.rows(), mat.width(), CvType.CV_8U, Scalar(4.0))
    try {
        Imgproc.cvtColor(/*seedsImage*/mat, tmp, Imgproc.COLOR_GRAY2RGBA, 4)
        bmp = Bitmap.createBitmap(tmp.cols(), tmp.rows(), Bitmap.Config.ARGB_8888)
        Utils.matToBitmap(tmp, bmp)
    } catch (e: CvException) {
        e.message?.let { Log.d("Exception", it) }
    }
    return bmp!!
}

fun saveHaarCascadesAsFiles(context: Context) {
    var ist: InputStream
    var os: FileOutputStream
    try {
        ist = context.getResources().getAssets()
            .open("haarcascades/haarcascade_mcs_leftear.xml")
        val cascadeDir: File = context.getDir("haarcascades", Context.MODE_PRIVATE)
        var mCascadeFile = File(cascadeDir, "haarcascade_mcs_leftear.xml")
        val os: FileOutputStream
        os = FileOutputStream(mCascadeFile)
        val buffer = ByteArray(4096)
        var bytesRead: Int
        while (ist.read(buffer).also { bytesRead = it } != -1) {
            os.write(buffer, 0, bytesRead)
        }
        ist.close()
        os.close()
    } catch (e: IOException) {
    }
    try {
        ist = context.getResources().getAssets()
            .open("haarcascades/haarcascade_mcs_rightear.xml")
        val cascadeDir: File = context.getDir("haarcascades", Context.MODE_PRIVATE)
        var mCascadeFile = File(cascadeDir, "haarcascade_mcs_rightear.xml")
        val os: FileOutputStream
        os = FileOutputStream(mCascadeFile)
        val buffer = ByteArray(4096)
        var bytesRead: Int
        while (ist.read(buffer).also { bytesRead = it } != -1) {
            os.write(buffer, 0, bytesRead)
        }
        ist.close()
        os.close()
    } catch (e: IOException) {
    }
}