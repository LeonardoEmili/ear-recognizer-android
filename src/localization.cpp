#include "localization.hpp"

#define CROPPED_PATH "generated/cropped/"

void initializeCascade(CascadeClassifier &cascade, String name)
{
    if (!cascade.load(name))
    {
        cout << "--(!)Error loading " << name << " cascade classifier.\n";
        exit(1);
    };
}

int cropAndFlipImages(char *datasetPath)
{

    vector<string> imageNames = readDataset(datasetPath);

    // Function findFile always reminds us where it found these files, just annoying to see
    freopen("/dev/null", "w", stderr);
    String leftEarCascadeName = findFile("haarcascade_mcs_leftear.xml");
    String rightEarCascadeName = findFile("haarcascade_mcs_rightear.xml");
    freopen("/dev/null", "w", stderr);

    CascadeClassifier leftEarCascade;
    CascadeClassifier rightEarCascade;

    initializeCascade(leftEarCascade, leftEarCascadeName);
    initializeCascade(rightEarCascade, rightEarCascadeName);

    double n_detected = 0.0;
    double n_visited = 0.0;

    bool display_images = false;

    for (String imageName : imageNames)
    {

        ostringstream imgPath;
        imgPath << datasetPath << imageName;

        Mat image = imread(imgPath.str(), IMREAD_COLOR); // Read the file

        if (image.empty())
        { // Check for invalid input
            cout << "Could not open or find the image" << std::endl;
            return -1;
        }
        n_visited++;
        // Checking left ear
        if (detectImage(image, leftEarCascade, false, display_images, imageName) > 0)
        {
            n_detected++;
            cout << "Left ear found !\n"
                 << endl;
        }
        else
        {
            // Checking right ear
            if (detectImage(image, rightEarCascade, true, display_images, imageName) > 0)
            {
                n_detected++;
                cout << "Right ear found !\n"
                     << endl;
            }
            else
            {
                /* Trying to flip the given image horizontally and interpet it  
                    as the opposite ear*/
                Mat flipped;
                flip(image, flipped, 1);
                // Checking left ear
                if (detectImage(flipped, leftEarCascade, false, display_images, imageName) > 0)
                {
                    n_detected++;
                    cout << "Left ear found !\n"
                         << endl;
                }
                else
                {
                    // Checking right ear
                    if (detectImage(flipped, rightEarCascade, true, display_images, imageName) > 0)
                    {
                        n_detected++;
                        cout << "Right ear found !\n"
                             << endl;
                    }
                }
            }
        }
    }

    cout << "Detection rate ";
    printf("%f\n", n_detected / n_visited);

    return 0;
}

bool isValidROI(Rect BBox, Mat originalFrame)
{
    return (BBox.x >= 0 && BBox.y >= 0 && BBox.width >= 0 && BBox.height >= 0 && BBox.x + BBox.width <= originalFrame.cols && BBox.y + BBox.height <= originalFrame.rows);
}

int detectImage(Mat frame, CascadeClassifier &cascade, bool rightClassifier,
                bool display, String imageName)
{
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    //equalizeHist( frame_gray, frame_gray );

    Mat croppedEar(frame);
    std::vector<Rect> ears;
    cascade.detectMultiScale(frame_gray, ears);
    for (Rect ear : ears)
    {
        //rectangle(frame, ears[i], Scalar( 255, 0, 255 ), 4); // display a rectangle
        if (!isValidROI(ear, croppedEar))
        {
            return 0;
        }
        croppedEar = croppedEar(ear);
    }
    cout << ears.size() << "\n"
         << flush;

    // Saving cropped (and in case flipped) image as output
    if (ears.size() > 0)
    {
        Mat outputImg = croppedEar;
        // Flipping if ear is the right one. We only keep left ears for recognition
        if (rightClassifier)
        {
            flip(croppedEar, outputImg, 1);
        }
        String outputPath = CROPPED_PATH + imageName;
        cv::utils::fs::createDirectories(CROPPED_PATH);
        imwrite(outputPath, outputImg);
    }

    //Displaying image
    if (display)
    {
        displayDetected(croppedEar);
    }
    return ears.size();
}

void displayDetected(Mat croppedEar)
{
    imshow("Ear detection", croppedEar);
    waitKey(0);
    destroyAllWindows();
}
