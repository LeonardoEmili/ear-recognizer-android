#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <dirent.h>


using namespace cv;
using namespace std;

/**
 * Execute a command a return the result.
 * @param cmd is the command to be executed
 * @return the output of the command
 */
string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

/**
 * Read a dataset and return the list of images.
 * @param path where the dataset is located
 * @param outArray the list of images returned as an array
 */
void readDataset(char *path, char **outArray) {
    if (DIR *dir = opendir(path)) {
        int i = 0;
        while (struct dirent *entry = readdir(dir)) {

            // Skip special directories
            if (strcmp(entry->d_name, ".") == 0) continue;
            if (strcmp(entry->d_name, "..") == 0) continue;
            if (entry->d_name[0] == '.') continue;

            outArray[i++] = entry->d_name;
            cout << outArray[i-1] << '\n';
        }
    }
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

    for (char *e : inputImages) {
        // Apply whatever processing to images here
        // e is a the filename of each picture
        
        cout << e << '\n';
    }
    
    return 0;

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