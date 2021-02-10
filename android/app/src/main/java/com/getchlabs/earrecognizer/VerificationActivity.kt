package com.getchlabs.earrecognizer


import android.content.Intent
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.provider.MediaStore
import android.view.MenuItem
import android.widget.*
import com.getchlabs.earrecognizer.recognition.addTemplate
import com.getchlabs.earrecognizer.recognition.getDescriptor
import com.getchlabs.earrecognizer.recognition.verifyIdentity
import java.io.BufferedInputStream
import java.io.InputStream

class VerificationActivity : RecognitionActivity() {


    private lateinit var btnPickImage: Button
    private lateinit var btnOpenCamera: Button
    private lateinit var tvName: EditText
    private lateinit var checkTemplate : CheckBox
    private lateinit var hintBox : TextView


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_verification)
        title = ("Verification")

        btnPickImage = findViewById(R.id.btn_pick_image)
        btnPickImage.setOnClickListener { pickImage(this, PICK_IMAGE_FROM_GALLERY) }
        btnOpenCamera = findViewById(R.id.btn_open_camera)
        btnOpenCamera.setOnClickListener { pickImage(this, PICK_IMAGE_FROM_CAMERA) }
        tvName = findViewById(R.id.tv_name)
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
        if (verifyIdentity(this, tvName.text.toString(), descriptor)) {
            if (checkTemplate.isChecked) {
                addTemplate(this, tvName.text.toString(), descriptor)
                Toast.makeText(this, "Template succesfully updated", Toast.LENGTH_SHORT).show()
            }
            //Toast.makeText(this, "Genuine", Toast.LENGTH_LONG).show()
            hintBox.text = "Genuine template submitted"

        } else {
            hintBox.text = "You shall not pass!"
            //Toast.makeText(this, "Impostor", Toast.LENGTH_LONG).show()

        }
        } else {
            hintBox.text = "Invalid template"
            Toast.makeText(this, "Invalid template", Toast.LENGTH_LONG).show()

        }

    }



}