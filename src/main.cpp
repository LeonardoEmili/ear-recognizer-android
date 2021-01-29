#include <dirent.h>

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/objdetect.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;
using namespace samples;

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
vector<string> readDataset(char *path)
{
    vector<string> fnames;
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

            fnames.push_back(entry->d_name);
        }
    }
    return fnames;
}


void detectAndDisplay( Mat frame, CascadeClassifier& cascade) {
    Mat frame_gray;
    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );
    //-- Detect faces

    Mat croppedEar(frame);
    std::vector<Rect> faces;
    cascade.detectMultiScale( frame_gray, faces );
    for ( size_t i = 0; i < faces.size(); i++ ) {
        Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
        //rectangle(frame, faces[i], Scalar( 255, 0, 255 ), 4); // display a rectangle
        croppedEar = croppedEar(faces[i]);
    }
    //-- Show what you got
    imshow( "Capture - Face detection", croppedEar );

    waitKey(0);                       // Wait for a keystroke in the window
}

/**
 * Run 'wc -l' to see the size of the dataset.
 * @param path the path to the dataset
 * @return the size of the dataset
 */
int getDatasetSize(char* path) {
    ostringstream cmd;
    cmd << "ls -l1 " << path << " | wc -l";
    return stoi(exec(cmd.str().c_str()));
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << " Usage: " << argv[0] << " path to dataset" << endl;
        return -1;
    }    

    int datasetSize = getDatasetSize(argv[1]);

    vector<string> imageNames = readDataset(argv[1]);

    String leftEarCascadeName = findFile("haarcascade_mcs_leftear.xml");
    String rightEarCascadeName = findFile("haarcascade_mcs_rightear.xml");

    CascadeClassifier leftEarCascade;
    CascadeClassifier rightEarCascade;

    if (!leftEarCascade.load(leftEarCascadeName)) {
        cout << "--(!)Error loading left ear cascade\n";
        return -1;
    };
    if (!rightEarCascade.load(rightEarCascadeName)) {
        cout << "--(!)Error loading right ear cascade\n";
        return -1;
    };

    for (String imageName: imageNames) {

        ostringstream imgPath;
        imgPath << argv[1] << imageName;
        cout << imgPath.str();  // debug

        Mat image;
        image = imread(imgPath.str(), IMREAD_COLOR);  // Read the file

        if (image.empty()) {  // Check for invalid input
            cout << "Could not open or find the image" << std::endl;
            return -1;
        }

        detectAndDisplay(image, leftEarCascade);

        

    }

    return 0;
}