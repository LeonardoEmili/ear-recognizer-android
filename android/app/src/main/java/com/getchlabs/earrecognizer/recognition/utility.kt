package com.getchlabs.earrecognizer.recognition

import android.content.Context
import android.graphics.Bitmap
import android.util.Base64
import android.util.Log
import com.google.gson.Gson
import com.google.gson.JsonObject
import com.google.gson.JsonParser
import org.opencv.android.Utils
import org.opencv.core.*
import org.opencv.imgproc.Imgproc
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream

val OPTIMAL_RECOGNITION_THRESHOLD = 0.88

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

fun computeCentroid(points: ArrayList<Point>): Point {
    var x = 0.0
    var y = 0.0
    for (point in points) {
        x+=point.x
        y+=point.y
    }
    return Point(x / points.size, y / points.size)
}


fun matToJson(mat: Mat): String? {
    val obj = JsonObject()
    if (mat.isContinuous) {
        val cols = mat.cols()
        val rows = mat.rows()
        val elemSize = mat.elemSize().toInt()
        val data = ByteArray(cols * rows * elemSize)
        mat[0, 0, data]
        obj.addProperty("rows", mat.rows())
        obj.addProperty("cols", mat.cols())
        obj.addProperty("type", mat.type())

        // We cannot set binary data to a json object, so:
        // Encoding data byte array to Base64.
        val dataString: String = String(Base64.encode(data, Base64.DEFAULT))
        obj.addProperty("data", dataString)
        val gson = Gson()
        return gson.toJson(obj)
    } else {
        Log.e("", "Mat not continuous.")
    }
    return "{}"
}

fun matFromJson(json: String?): Mat? {
    val parser = JsonParser()
    val JsonObject = parser.parse(json).asJsonObject
    val rows = JsonObject["rows"].asInt
    val cols = JsonObject["cols"].asInt
    val type = JsonObject["type"].asInt
    val dataString = JsonObject["data"].asString
    val data: ByteArray = Base64.decode(dataString.toByteArray(), Base64.DEFAULT)
    val mat = Mat(rows, cols, type)
    mat.put(0, 0, data)
    return mat
}