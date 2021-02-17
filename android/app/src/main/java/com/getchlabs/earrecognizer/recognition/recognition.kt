package com.getchlabs.earrecognizer.recognition

import android.content.Context
import android.graphics.Bitmap
import android.util.Log
import android.widget.Toast
import com.google.gson.GsonBuilder
import com.google.gson.reflect.TypeToken
import org.opencv.core.Mat
import org.opencv.core.Point
import org.opencv.core.Rect
import java.io.*


fun addTemplate(context: Context, identity: String, template: Mat) {
    var map: HashMap<String, ArrayList<Mat>>? = readGallery(context)
    if (map == null) {
        map = hashMapOf()
    }
    if (!map.containsKey(identity)) {
        map[identity] = arrayListOf()
    }
    map[identity]!!.add(template)
    writeGallery(context, map)
}

fun verifyIdentity(context: Context, identity: String, probe: Mat,
                   threshold: Double = OPTIMAL_RECOGNITION_THRESHOLD): Boolean {
    var map: HashMap<String, ArrayList<Mat>> = readGallery(context) ?: return false
    if (!map.containsKey(identity)) {
        return false
    }
    for (template in map[identity]!!) {
        if (1.0 - computeSimilarity(probe, template) <= threshold) {
            return true
        }
    }

    return false
}

fun identify(context: Context, probe: Mat,
             threshold: Double = OPTIMAL_RECOGNITION_THRESHOLD): String? {

    var map: HashMap<String, ArrayList<Mat>> = readGallery(context) ?: return null

    var lowDistance = 1.1
    var bestIdentity : String? = null
    for (identity in map.keys) {
        for (template in map[identity]!!) {
            var distance = 1.0 - computeSimilarity(probe, template)
            if (distance <= threshold && distance < lowDistance) {
                lowDistance = distance
                bestIdentity = identity
            }
        }
    }
    return bestIdentity
}

fun readGallery(context: Context): HashMap<String, ArrayList<Mat>>? {
    var path = context.getFilesDir().getAbsolutePath() + "/" + "gallery.json"
    var file = File(path)
    if (!file.exists()) {
        return null
    }


    var gson = GsonBuilder().create()
    val type = object : TypeToken<HashMap<String, ArrayList<String>>>() {}.type
    val br =  BufferedReader(FileReader(file))
    val stringMap = gson.fromJson<HashMap<String, ArrayList<String>>>(br, type)
    var ret = hashMapOf<String, ArrayList<Mat>>()
    for (key in stringMap.keys) {
        var arrayList = arrayListOf<Mat>()
        stringMap[key]!!.forEach { arrayList.add(matFromJson(it)!!) }
        ret[key] = arrayList
    }
    return ret
}

fun writeGallery(context: Context, map: Map<String, ArrayList<Mat>>) {
    var path = context.getFilesDir().getAbsolutePath() + "/" + "gallery.json"
    var file = File(path)
    if (!file.exists()) {
        file.createNewFile()
    }

    val stringMap = hashMapOf<String, ArrayList<String>>()
    for (key in map.keys) {
        var arrayList = arrayListOf<String>()
        map[key]!!.forEach { arrayList.add(matToJson(it)!!) }
        stringMap[key] = arrayList
    }

    var gson = GsonBuilder().create()
    val text = gson.toJson(stringMap)

    try {
        val outputStreamWriter = FileOutputStream(file)
        outputStreamWriter.write(text.toByteArray())
        outputStreamWriter.close()
    } catch (e: IOException) {
        Log.e("Exception", "File write failed: $e")
    }
}


/**
 * Returns the descriptor of the image.
 */
fun getDescriptor(bmp: Bitmap, context: Context): Mat? {

    val aspectRatio: Float = bmp.getWidth() /
            bmp.getHeight().toFloat()
    val width = 400
    val height = Math.round(width / aspectRatio)

    var resized = Bitmap.createScaledBitmap(
            bmp, width, height, false)

    var mat = bitmapToMat(resized)
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


    var descriptors = arrayListOf<Mat?>()
    extractFeatures(processedROI2, descriptors)

    //println(descriptors[0])

    return descriptors[0]

}

