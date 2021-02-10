package com.getchlabs.earrecognizer

import android.R.attr
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.getchlabs.earrecognizer.recognition.addTemplate
import com.getchlabs.earrecognizer.recognition.getDescriptor
import java.io.BufferedInputStream
import java.io.InputStream


class EnrollmentActivity : RecognitionActivity() {

    private lateinit var btnPickImage: Button
    private lateinit var btnOpenCamera: Button
    private lateinit var hintBox : TextView

    //private lateinit var imgEar: ImageView
    private lateinit var tvName: EditText

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_enrollement)
        title = ("Enrollment")


        btnPickImage = findViewById(R.id.btn_pick_image)
        btnPickImage.setOnClickListener { pickImage(this, PICK_IMAGE_FROM_GALLERY) }
        btnOpenCamera = findViewById(R.id.btn_open_camera)
        btnOpenCamera.setOnClickListener { pickImage(this, PICK_IMAGE_FROM_CAMERA) }
        tvName = findViewById(R.id.tv_name)
        hintBox = findViewById(R.id.msg_hint)
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, intent: Intent?) {
        super.onActivityResult(requestCode, resultCode, intent)
        intent ?: return

        var bmp = getBitmapFromIntent(this, requestCode, intent) ?: return

        //Toast.makeText(this, "Got the image", Toast.LENGTH_SHORT).show()

        //imgEar.setImageBitmap(recognize(bmp, this))
        var descriptor = getDescriptor(bmp, this)
        if (descriptor != null) {
            addTemplate(this, tvName.text.toString(), descriptor)
            hintBox.text = "User succesfully enrolled"
        } else {
            hintBox.text = "Invalid template"

        }

    }



/*

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (data == null || data.data == null) {
            return
        }
        if (requestCode == PICK_IMAGE) {
            Toast.makeText(this, "Got the image", Toast.LENGTH_SHORT).show()
            val inputStream: InputStream? = getContentResolver().openInputStream(data?.data!!)
            val bufferedInputStream =  BufferedInputStream(inputStream);
            val bmp = BitmapFactory.decodeStream(bufferedInputStream);


            //imgEar.setImageBitmap(recognize(bmp, this))
            var descriptor = getDescriptor(bmp, this)
            if (descriptor != null) {
                addTemplate(this, tvName.text.toString(), descriptor)
                Toast.makeText(this, "Successfully enrolled", Toast.LENGTH_SHORT).show()
            } else {
                Toast.makeText(this, "Invalid", Toast.LENGTH_LONG).show()

            }

        }

        }*/


}