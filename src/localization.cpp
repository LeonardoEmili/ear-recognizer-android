#include "localization.hpp"

void initializeCascade(CascadeClassifier& cascade, String name) {
    if (!cascade.load(name)) {
        cout << "--(!)Error loading " << name << " cascade classifier.\n";
        exit(1);
    };
}

void crop_ears()
{
    cout << "Hello";
}

void localize_ear()
{
    cout << "Ajeje\n";
}

bool isValidROI(Rect BBox, Mat originalFrame)
{
    return (BBox.x >= 0 && BBox.y >= 0 && BBox.width >= 0 && BBox.height >= 0
            && BBox.x + BBox.width <= originalFrame.cols && BBox.y + BBox.height <= originalFrame.rows);
}