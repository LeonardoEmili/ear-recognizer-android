package com.getchlabs.earrecognizer

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.provider.MediaStore
import androidx.core.app.ActivityCompat.requestPermissions
import androidx.core.app.ActivityCompat.startActivityForResult
import androidx.core.content.ContextCompat
import androidx.core.content.ContextCompat.checkSelfPermission
import java.io.BufferedInputStream
import java.io.InputStream
import java.util.jar.Manifest


val PICK_IMAGE_FROM_CAMERA = 0
val PICK_IMAGE_FROM_GALLERY = 1
val CAMERA_REQUEST = 1888
val CAMERA_PERMISSION_CODE = 100


fun pickImage(activity: Activity, actionCode: Int) {
    /*
    val getIntent = Intent(Intent.ACTION_GET_CONTENT)
    getIntent.type = "image/*" */

    val pickIntent = Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI)
    pickIntent.type = "image/*"  */

    val chooserIntent = Intent.createChooser(getIntent, "Select Image")
    chooserIntent.putExtra(Intent.EXTRA_INITIAL_INTENTS, arrayOf(pickIntent))

    activity.startActivityForResult(chooserIntent, PICK_IMAGE)  */



if (actionCode == PICK_IMAGE_FROM_CAMERA) {
    if (checkSelfPermission(activity, "android.permission.CAMERA")
            != PackageManager.PERMISSION_GRANTED) {
        activity.requestPermissions(arrayOf("android.permission.CAMERA"), CAMERA_PERMISSION_CODE)
    } else {
        val takePicture = Intent(MediaStore.ACTION_IMAGE_CAPTURE)
        activity.startActivityForResult(takePicture, PICK_IMAGE_FROM_CAMERA)
    }


} else if (actionCode == PICK_IMAGE_FROM_GALLERY) {
    val pickPhoto = Intent(
            Intent.ACTION_PICK,
            MediaStore.Images.Media.EXTERNAL_CONTENT_URI
    )
    activity.startActivityForResult(pickPhoto, PICK_IMAGE_FROM_GALLERY) //one can be replaced with any action code

}
}


fun getBitmapFromIntent(context: Context, requestCode: Int, intent: Intent): Bitmap? {
    var bmp: Bitmap? = null
    when (requestCode) {
        PICK_IMAGE_FROM_CAMERA -> {

            bmp = intent.extras?.get("data") as Bitmap?


        }
        PICK_IMAGE_FROM_GALLERY -> {


            val inputStream: InputStream? = context.getContentResolver()
                    .openInputStream(intent.data ?: return bmp)
            val bufferedInputStream = BufferedInputStream(inputStream);
            bmp = BitmapFactory.decodeStream(bufferedInputStream);


        }
    }
    return bmp
}

