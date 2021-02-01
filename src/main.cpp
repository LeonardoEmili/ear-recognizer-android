#include <iostream>
#include "localization.hpp"
#include "utility.hpp"

using namespace std;

/**
 * Execute the actual main function from an helper function to be able
 * to see exceptions on MacOS too.
 */
int f(int argc, char **argv) {
    if (argc != 2) {
        cout << " Usage: " << argv[0] << " path to dataset" << endl;
        return -1;
    }

    char *datasetPath = argv[1];
    vector<string> imageNames = readDataset(datasetPath);

    cropAndFlipImages(datasetPath, imageNames, true);
    return 0;
}

int main(int argc, char **argv) {
    try {
        return f(argc, argv);
    } catch (exception &e) {
        cout << "Unhandled exception thrown: " << e.what() << endl;
    }
    return -1;
}