#include <dirent.h>

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/objdetect.hpp>

using namespace cv;
using namespace std;

CascadeClassifier ear_cascade;

/**
 * Execute a command a return the result.
 * @param cmd is the command to be executed
 * @return the output of the command
 */
string exec(const char *cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
    {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

/**
 * Read a dataset and return the list of images.
 * @param path where the dataset is located
 * @param outArray the list of images returned as an array
 */
void readDataset(char *path, char **outArray)
{
    if (DIR *dir = opendir(path))
    {
        int i = 0;
        while (struct dirent *entry = readdir(dir)) {
            // Skip special directories
            if (strcmp(entry->d_name, ".") == 0)
                continue;
            if (strcmp(entry->d_name, "..") == 0)
                continue;
            if (entry->d_name[0] == '.')
                continue;

            outArray[i++] = entry->d_name;
            //cout << outArray[i-1] << '\n';
        }
    }
}


void detectAndDisplay( Mat frame ) {
    Mat frame_gray;
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    //-- Detect faces

    Mat croppedEar(frame);
    std::vector<Rect> faces;
    ear_cascade.detectMultiScale( frame_gray, faces );
    for ( size_t i = 0; i < faces.size(); i++ ) {
        Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
        //ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2 ), 0, 0, 360, Scalar( 255, 0, 255 ), 4 );
        //rectangle(frame, faces[i], Scalar( 255, 0, 255 ), 4);
        croppedEar = croppedEar(faces[i]);
    }
    //-- Show what you got
    imshow( "Capture - Face detection", croppedEar );

    waitKey(0);                       // Wait for a keystroke in the window
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << " Usage: " << argv[0] << " path to dataset" << endl;
        return -1;
    }

    // Run 'wc -l' to see the size of the dataset
    ostringstream cmd;
    cmd << "ls -l1 " << argv[1] << " | wc -l";

    int datasetSize = stoi(exec(cmd.str().c_str()));
    char *inputImages[datasetSize];

    readDataset(argv[1], inputImages);

    // String eyes_cascade_name = samples::findFile(
    // parser.get<String>("eyes_cascade") );

    if (!ear_cascade.load("/Users/leonardoemili/opencv/data/haarcascades/"
                          "haarcascade_mcs_leftear.xml")) {
        cout << "--(!)Error loading face cascade\n";
        return -1;
    };

    for (char *e : inputImages) {
        // Apply whatever processing to images here
        // e is a the filename of each picture

        ostringstream imgPath;
        imgPath << argv[1] << "4.jpeg";

        Mat image;
        image = imread(imgPath.str(), IMREAD_COLOR);  // Read the file

        if (image.empty()) {  // Check for invalid input
            cout << "Could not open or find the image" << std::endl;
            return -1;
        }

        detectAndDisplay(image);

        return 0;

    }

    return 0;
}