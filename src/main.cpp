#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <dirent.h>


using namespace cv;
using namespace std;


int main(int argc, char **argv) {

    if (DIR* dir = opendir(".")) {
        while (struct dirent *entry = readdir(dir)) {
            cout << entry->d_name << '\n';
        }
    }

    //for (const auto &entry: std::__fs::filesystem::directory_iterator(".")) {
      //  cout << entry.path();
    //}

    if (argc != 2) {
        cout << " Usage: " << argv[0] << " ImageToLoadAndDisplay" << endl;
        return -1;
    }

    Mat image;
    image = imread(argv[1], IMREAD_COLOR);  // Read the file
    if (image.empty())                      // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
        return -1;
    }
    namedWindow("Display window",
                WINDOW_AUTOSIZE);     // Create a window for display.
    imshow("Display window", image);  // Show our image inside it.
    waitKey(0);                       // Wait for a keystroke in the window
    return 0;
}