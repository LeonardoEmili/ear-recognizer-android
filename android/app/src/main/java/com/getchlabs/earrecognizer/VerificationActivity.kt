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

class VerificationActivity : AppCompatActivity() {

    val PICK_IMAGE = 1

    private lateinit var btnPickImage: Button
    private lateinit var imgEar: ImageView
    private lateinit var tvName: EditText


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_verification)

        // calling the action bar
        var actionBar = getSupportActionBar()

        // showing the back button in action bar
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true)
        }

        btnPickImage = findViewById(R.id.btn_pick_image)
        btnPickImage.setOnClickListener { pickImage() }
        tvName = findViewById(R.id.tv_name)
    }

    override fun onContextItemSelected(item: MenuItem): Boolean {
        Toast.makeText(this, item.itemId.toString(), Toast.LENGTH_LONG).show();
        when (item.itemId) {
            android.R.id.home -> {
                finish()
                return true
            }
        }
        return super.onContextItemSelected(item)
    }


    fun pickImage() {
        val getIntent = Intent(Intent.ACTION_GET_CONTENT)
        getIntent.type = "image/*"

        val pickIntent = Intent(Intent.ACTION_PICK, MediaStore.Images.Media.EXTERNAL_CONTENT_URI)
        pickIntent.type = "image/*"

        val chooserIntent = Intent.createChooser(getIntent, "Select Image")
        chooserIntent.putExtra(Intent.EXTRA_INITIAL_INTENTS, arrayOf(pickIntent))

        startActivityForResult(chooserIntent, PICK_IMAGE)
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode == PICK_IMAGE) {
            //TODO: action
            Toast.makeText(this, "Got the image", Toast.LENGTH_SHORT).show()
            val inputStream: InputStream? = getContentResolver().openInputStream(data?.data!!)
            val bufferedInputStream =  BufferedInputStream(inputStream);
            val bmp = BitmapFactory.decodeStream(bufferedInputStream);


            //imgEar.setImageBitmap(recognize(bmp, this))
            var descriptor = getDescriptor(bmp, this)
            if (descriptor != null) {
                if (verifyIdentity(this, tvName.text.toString(), descriptor)) {
                    Toast.makeText(this, "Correct", Toast.LENGTH_LONG).show()

                } else {
                    Toast.makeText(this, "Impostor", Toast.LENGTH_LONG).show()

                }
            } else{
                Toast.makeText(this, "Invalid", Toast.LENGTH_LONG).show()
            }

        }
    }
}