package com.getchlabs.earrecognizer

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.MenuItem
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import com.getchlabs.earrecognizer.recognition.getDescriptor
import com.getchlabs.earrecognizer.recognition.identify
import com.getchlabs.earrecognizer.recognition.verifyIdentity

class IdentificationActivity : RecognitionActivity() {


    private lateinit var btnPickImage: Button
    private lateinit var btnOpenCamera: Button


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_identification)
        title = ("Identification")



        btnPickImage = findViewById(R.id.btn_pick_image)
        btnPickImage.setOnClickListener { pickImage(this, PICK_IMAGE_FROM_GALLERY) }
        btnOpenCamera = findViewById(R.id.btn_open_camera)
        btnOpenCamera.setOnClickListener { pickImage(this, PICK_IMAGE_FROM_CAMERA) }
    }





    override fun onActivityResult(requestCode: Int, resultCode: Int, intent: Intent?) {
        super.onActivityResult(requestCode, resultCode, intent)

        intent ?: return


        var bmp = getBitmapFromIntent(this, requestCode, intent) ?: return

        Toast.makeText(this, "Got the image", Toast.LENGTH_SHORT).show()


        //imgEar.setImageBitmap(recognize(bmp, this))
        var descriptor = getDescriptor(bmp, this)
        if (descriptor != null) {
            var identity = identify(this, descriptor)
            if (identity != null)
                Toast.makeText(this, "Identity: " + identity, Toast.LENGTH_LONG).show()
             else
                Toast.makeText(this, "Impostor", Toast.LENGTH_LONG).show()

        } else {
            Toast.makeText(this, "Invalid template", Toast.LENGTH_LONG).show()

        }

    }

}