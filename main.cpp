#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "Sorter.h"

int main(int argc, char *argv[])
{
    std::string filename {argv[1]};
    std::string outFilename {argv[2]};

    sf::Image image{};
    image.loadFromFile(filename);

    srand(time(NULL));
    auto srt = Sorter(&image);
    image = srt.sort(argv);

    image.saveToFile(outFilename);

    return 0;
}
