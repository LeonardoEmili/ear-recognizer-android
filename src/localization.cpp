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