#include "Sorter.h"

sf::Image *Point::image = nullptr;

Sorter::Sorter(sf::Image* img)
{
    image = img;
    sf::Vector2u imageSize = image->getSize();
    width = imageSize.x;
    height = imageSize.y;
    sortedImage = {};
    sortedImage.create(width, height);
    Point::image = img;
}

Sorter::~Sorter()
{
    //dtor
}

sf::Image Sorter::sort(char *argv[])
{
    std::string pathType {argv[3]};
    std::string funcType {argv[4]};
    int maxIntervals {std::stoi(argv[5])};
    bool randomizeIntervals {static_cast<bool>(std::stoi(argv[6]))};
    int angle {std::stoi(argv[7])};
    bool toMerge {static_cast<bool>(std::stoi(argv[8]))};
    bool toReverse {static_cast<bool>(std::stoi(argv[9]))};
    bool toMirror {static_cast<bool>(std::stoi(argv[10]))};

    std::vector<std::vector<Point>> path;
    std::vector<std::vector<Point>> sortedPath;

    if (pathType == "rows")
        path = rows();
    else if (pathType == "columns")
        path = columns();
    else if (pathType == "rectangles")
        path = rectangles();
    else if (pathType == "angled")
        path = angled(angle);

    if (toMerge)
        mergeIntoOne(&path);

    path = applyIntervals(&path, maxIntervals, randomizeIntervals);
    sortedPath = path;

    for (auto& seq : sortedPath)
        std::sort(seq.begin(), seq.end());

    if (toReverse)
        reverseSort(&sortedPath);

    if (toMirror)
        mirror(&sortedPath);

    for (unsigned int i = 0; i < path.size(); ++i)
        for (unsigned int j = 0; j < path[i].size(); ++j)
        {
            Point before = path[i][j];
            Point after = sortedPath[i][j];
            sortedImage.setPixel(before.j, before.i, image->getPixel(after.j, after.i));
        }

    return sortedImage;
}

std::vector<std::vector<Point>> Sorter::rows()
{
    std::vector<std::vector<Point>> out{};

    for (unsigned int i = 0; i < height; ++i)
    {
        out.push_back({});

        for (unsigned int j = 0; j < width; ++j)
            out[i].push_back({i, j});
    }

    return out;
}

std::vector<std::vector<Point>> Sorter::columns()
{
    std::vector<std::vector<Point>> out{};

    for (unsigned int j = 0; j < width; ++j)
    {
        out.push_back({});

        for (unsigned int i = 0; i < height; ++i)
            out[j].push_back({i, j});

    }

    return out;
}

std::vector<std::vector<Point>> Sorter::rectangles()
{
    std::vector<std::vector<Point>> out {};
    int maxBorderDist = width < height ? width / 2 : height / 2;

    for (int borderDist = 0; borderDist < maxBorderDist; ++borderDist)
    {
        int i0 = borderDist;
        int j0 = borderDist;
        int i1 = static_cast<int>(height) - borderDist;
        int j1 = static_cast<int>(width) - borderDist;

        out.push_back({});

        for (int j = j0; j < j1; ++j)
            out[out.size() - 1].push_back({i0, j});

        for (int i = i0 + 1; i < i1; ++i)
            out[out.size() - 1].push_back({i, j1 - 1});

        for (int j = j1 - 2; j >= j0; --j)
            out[out.size() - 1].push_back({i1 - 1, j});

        for (int i = i1 - 2; i >= i0 + 1; --i)
            out[out.size() - 1].push_back({i, j0});

        std::rotate(out[out.size() - 1].begin(),
                    out[out.size() - 1].begin() + (rand() % out[out.size() - 1].size()),
                    out[out.size() - 1].end());
    }

    return out;
}

std::vector<std::vector<Point>> Sorter::angled(int angle)
{
    constexpr double pi = 3.1415926535897;

    bool toFlipY = angle >= 0;

    if (!toFlipY)
        angle *= -1;

    bool toFlipX = angle > 90;

    int times = 0;

    if (toFlipX)
    {
        times = angle / 90;
        angle %= 90;
    }

    bool toTranspose = angle > 45;

    if (toTranspose)
    {
        int temp = width;
        width = height;
        height = temp;
        angle = 90 - angle;
    }

    int deltay = std::round((width - 1) * std::tan(angle * pi / 180));
    int deltax = width - 1;
    int err = 0;
    int deltaerr = deltay;
    int y = 0;
    std::vector<Point> line{};

    for (int x = 0; x < deltax + 1; ++x)
    {
        line.push_back({y, x});
        err += deltaerr;

        if (2 * err >= deltax)
        {
            ++y;
            err -= deltax;
        }
    }

    std::vector<std::vector<Point>> out {};

    for (int di = -deltay; di < static_cast<int>(height); ++di)
    {
        out.push_back({});

        for (const Point& p : line)
        {
            unsigned int i = p.i + di;
            unsigned int j = p.j;

            if (0 <= i && i < height)
                out[out.size() - 1].push_back({i, j});
            else if (out[out.size() - 1].size() > 0)
                break;
        }
    }

    for (std::vector<Point> &seq : out)
    {
        for (Point &p : seq)
        {
            if (toFlipY)
                p.i = height - 1 - p.i;

            if (times % 2 == 1)
                p.j = width - 1 - p.j;

            if (toTranspose)
            {
                unsigned int temp = p.i;
                p.i = p.j;
                p.j = temp;
            }
        }
    }

    if (toTranspose)
    {
        int temp = width;
        width = height;
        height = temp;
    }

    return out;
}

void Sorter::mergeIntoOne(std::vector<std::vector<Point>>* path)
{
    while ((*path).size() > 1)
    {
        (*path)[0].insert((*path)[0].end(),
                          (*path)[1].begin(),
                          (*path)[1].end());
        (*path).erase((*path).begin() + 1);
    }
}

void Sorter::reverseSort(std::vector<std::vector<Point>>* path)
{
    for (std::vector<Point> &seq : (*path))
        std::reverse(seq.begin(), seq.end());
}

void Sorter::mirror(std::vector<std::vector<Point>>* path)
{
    for (std::vector<Point> &seq : (*path))
    {
        std::deque<Point> mirrored{};

        for (int i = seq.size() - 1; i >= 0; --i)
        {
            if (i % 2 == 0)
                mirrored.push_back(seq[i]);
            else
                mirrored.push_front(seq[i]);

            seq.erase(seq.begin() + i);
        }

        seq.assign(mirrored.begin(), mirrored.end());
    }
}

std::vector<std::vector<Point>> Sorter::applyIntervals(std::vector<std::vector<Point>>* path, int maxIntervals, bool randomize)
{
    std::vector<std::vector<Point>> out{};

    if (maxIntervals < 2)
        return *path;

    for (auto &seq : (*path))
    {
        int start = 0;

        while (start < static_cast<int>(seq.size()))
        {
            int size;

            if (randomize)
                size = rand() % maxIntervals;
            else
                size = maxIntervals;

            std::vector<Point> segment;

            if (start + size < static_cast<int>(seq.size()))
                segment = {seq.begin() + start, seq.begin() + start + size};
            else
                segment = {seq.begin() + start, seq.end()};

            start += size;

            if (segment.size() > 0)
                out.push_back(segment);
        }
    }

    return out;
}

