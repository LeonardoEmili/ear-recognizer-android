package com.getchlabs.earrecognizer

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.MenuItem
import android.widget.*
import com.getchlabs.earrecognizer.recognition.addTemplate
import com.getchlabs.earrecognizer.recognition.getDescriptor
import com.getchlabs.earrecognizer.recognition.identify
import com.getchlabs.earrecognizer.recognition.verifyIdentity

class IdentificationActivity : RecognitionActivity() {


    private lateinit var btnPickImage: Button
    private lateinit var btnOpenCamera: Button
    private lateinit var checkTemplate : CheckBox
    private lateinit var hintBox : TextView


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_identification)
        title = ("Identification")



        btnPickImage = findViewById(R.id.btn_pick_image)
        btnPickImage.setOnClickListener { pickImage(this, PICK_IMAGE_FROM_GALLERY) }
        btnOpenCamera = findViewById(R.id.btn_open_camera)
        btnOpenCamera.setOnClickListener { pickImage(this, PICK_IMAGE_FROM_CAMERA) }
        checkTemplate = findViewById(R.id.template_update)
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
            var identity = identify(this, descriptor)
            if (identity != null) {
                if (checkTemplate.isChecked) {
                    addTemplate(this, identity, descriptor)
                    Toast.makeText(this, "Template succesfully updated", Toast.LENGTH_SHORT).show()
                }
                hintBox.text = "Welcome back: " + identity
            } else
                hintBox.text = "You shall not pass!"

        } else {
            hintBox.text = "Invalid template"
        }

    }

}