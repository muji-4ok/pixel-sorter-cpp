#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "Sorter.h"

bool fileExists(std::string filename)
{
    std::ifstream file{filename};
    return static_cast<bool>(file);
}

std::string getNewFilename(std::string filename)
{
    auto dot = filename.rfind(".");
    filename.replace(dot + 1, 3, "png");
    auto slash = filename.rfind("\\");
    filename.replace(0, slash + 1, "sorted ");

    return filename;
}

int main(int argc, char *argv[])
{
    std::string filename {argv[1]};
    std::string pathType {argv[2]};
    std::string funcType {argv[3]};
    int maxIntervals {std::stoi(argv[4])};
    bool randomizeIntervals {static_cast<bool>(std::stoi(argv[5]))};
    int angle {std::stoi(argv[6])};
    bool toMerge {static_cast<bool>(std::stoi(argv[7]))};

    sf::Image image{};
    image.loadFromFile(filename);

    srand(time(NULL));
    auto srt = Sorter(&image);
    image = srt.sort(pathType, funcType, maxIntervals, randomizeIntervals, angle, toMerge);

    std::string newFilename = getNewFilename(filename);
    std::cout << newFilename << "\n";
    image.saveToFile(newFilename);

    return 0;
}
