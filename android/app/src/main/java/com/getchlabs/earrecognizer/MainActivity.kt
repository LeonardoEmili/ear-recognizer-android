package com.getchlabs.earrecognizer

import android.content.Intent
import android.os.Bundle
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.cardview.widget.CardView
import org.opencv.android.OpenCVLoader

class MainActivity : AppCompatActivity() {

    private lateinit var btnEnrolUser: CardView
    private lateinit var btnVerifyUser: CardView
    private lateinit var btnIdentifyUser: CardView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        OpenCVLoader.initDebug()

        btnEnrolUser = findViewById(R.id.enrol_user);
        btnEnrolUser.setOnClickListener { goToEnrollementActivity() };
        btnVerifyUser = findViewById(R.id.verify_user);
        btnVerifyUser.setOnClickListener { goToVerificationActivity() };
        btnIdentifyUser = findViewById(R.id.identify_user);
        btnIdentifyUser.setOnClickListener { goToIdentificationActivity() };
    }

    fun goToEnrollementActivity() {
        //Toast.makeText(this, "Open enrollement", Toast.LENGTH_LONG).show();
        startActivity(Intent(this, EnrollmentActivity::class.java))
    }

    fun goToVerificationActivity() {
        //Toast.makeText(this, "Open verification", Toast.LENGTH_LONG).show();
        startActivity(Intent(this, VerificationActivity::class.java))
    }

    fun goToIdentificationActivity() {
        //Toast.makeText(this, "Open identification", Toast.LENGTH_LONG).show();
        startActivity(Intent(this, IdentificationActivity::class.java))
    }
}