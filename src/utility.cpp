#include "utility.hpp"

/**
 * Read a dataset and return the list of images.
 * @param path where the dataset is located
 * @param outArray the list of images returned as an array
 */
vector<string> readDataset(char *path) {
    vector<string> fnames;
    if (DIR *dir = opendir(path)) {
        int i = 0;
        while (struct dirent *entry = readdir(dir)) {
            // Skip special directories
            if (strcmp(entry->d_name, ".") == 0) continue;
            if (strcmp(entry->d_name, "..") == 0) continue;
            if (entry->d_name[0] == '.') continue;

            fnames.push_back(entry->d_name);
        }
    }
    return fnames;
}

/**
 * Execute a command a return the result.
 * @param cmd is the command to be executed
 * @return the output of the command
 */
string exec(const char *cmd) {
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
 * Run 'wc -l' to see the size of the dataset.
 * @param path the path to the dataset
 * @return the size of the dataset
 */
int getDatasetSize(char *path) {
    ostringstream cmd;
    cmd << "ls -l1 " << path << " | wc -l";
    return stoi(exec(cmd.str().c_str()));
}

/**
 * Utility function to store images using format imageName_ID.[ext].
 * @param imageName the relative path of the image
 * @param dirName path to the output directory
 * @param image image to be saved
 * @param id the index of the provided ROI within the image (serves as an identifier)
 */
void writeToFile(String imageName, String dirName, Mat image, int id) {
    int extIndex = imageName.find_last_of(".");
    string prefix = imageName.substr(0, extIndex);
    string extension = imageName.substr(extIndex, imageName.length());

    String outputPath = dirName + prefix + "_" + to_string(id) + extension;
    cv::utils::fs::createDirectories(dirName);
    imwrite(outputPath, image);
}