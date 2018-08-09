#include "stdafx.h"
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
    sortedImage.copy(*img, 0, 0);
    Point::image = img;
}

sf::Image Sorter::sort(const char *pathType, int maxIntervals, bool randomizeIntervals,
                       int angle, bool toMerge, bool toReverse, bool toMirror,
                       bool toInterval, int lowThreshold)
{
    //std::string pathType{ pathType };
    //int maxIntervals{ std::stoi(argv[4]) };
    //bool randomizeIntervals{ static_cast<bool>(std::stoi(argv[5])) };
    //int angle{ std::stoi(argv[6]) };
    //bool toMerge{ static_cast<bool>(std::stoi(argv[7])) };
    //bool toReverse{ static_cast<bool>(std::stoi(argv[8])) };
    //bool toMirror{ static_cast<bool>(std::stoi(argv[9])) };
    //bool toInterval{ static_cast<bool>(std::stoi(argv[10])) };
    //int lowThreshold{ std::stoi(argv[11]) };

    std::vector<std::vector<Point>> path;
    std::vector<std::vector<Point>> sortedPath;

    if (std::string(pathType) == "rows")
        path = rows();
    else if (std::string(pathType) == "columns")
        path = columns();
    else if (std::string(pathType) == "rectangles")
        path = rectangles();
    else if (std::string(pathType) == "angled")
        path = angled(angle);
    else if (std::string(pathType) == "edges rows")
        path = edgesRows(lowThreshold, lowThreshold * 3, 3);

    if (toMerge)
        mergeIntoOne(&path);

    if (toInterval)
        path = applyIntervals(&path, maxIntervals, randomizeIntervals);

    sortedPath = path;

    #pragma omp parallel for
    for (int i = 0; i < sortedPath.size(); ++i)
        std::sort(sortedPath[i].begin(), sortedPath[i].end());

    if (toReverse)
        reverseSort(&sortedPath);

    if (toMirror)
        mirror(&sortedPath);

    #pragma omp parallel for
    for (int i = 0; i < path.size(); ++i)
        for (int j = 0; j < path[i].size(); ++j)
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

    for (int i = 0; i < height; ++i)
    {
        out.push_back({});
        out[i].reserve(width);
    }

    #pragma omp parallel for
    for (int i = 0; i < height; ++i)
    {
        //out.push_back({});

        for (int j = 0; j < width; ++j)
            out[i].push_back(Point(i, j));
    }

    return out;
}

std::vector<std::vector<Point>> Sorter::columns()
{
    std::vector<std::vector<Point>> out{};

    for (int j = 0; j < width; ++j)
    {
        out.push_back({});
        out[j].reserve(height);
    }

    #pragma omp parallel for
    for (int j = 0; j < width; ++j)
    {
        //out.push_back({});

        for (int i = 0; i < height; ++i)
            out[j].push_back(Point(i, j));

    }

    return out;
}

std::vector<std::vector<Point>> Sorter::rectangles()
{
    std::vector<std::vector<Point>> out{};
    int maxBorderDist = width < height ? width / 2 : height / 2;

    for (int borderDist = 0; borderDist < maxBorderDist; ++borderDist)
    {
        int i0 = borderDist;
        int j0 = borderDist;
        int i1 = static_cast<int>(height) - borderDist;
        int j1 = static_cast<int>(width) - borderDist;

        out.push_back({});
        out[borderDist].reserve(j1 - j0 +
                                i1 - i0 - 1 +
                                j1 - 2 - j0 + 1 +
                                i1 - 2 - i0 - 1 + 1);
    }

    #pragma omp parallel for
    for (int borderDist = 0; borderDist < maxBorderDist; ++borderDist)
    {
        int i0 = borderDist;
        int j0 = borderDist;
        int i1 = static_cast<int>(height) - borderDist;
        int j1 = static_cast<int>(width) - borderDist;

        for (int j = j0; j < j1; ++j)
            out[borderDist].push_back(Point(i0, j));

        for (int i = i0 + 1; i < i1; ++i)
            out[borderDist].push_back(Point(i, j1 - 1));

        for (int j = j1 - 2; j >= j0; --j)
            out[borderDist].push_back(Point(i1 - 1, j));

        for (int i = i1 - 2; i >= i0 + 1; --i)
            out[borderDist].push_back(Point(i, j0));

        std::rotate(out[borderDist].begin(),
                    out[borderDist].begin() + (rand() % out[borderDist].size()),
                    out[borderDist].end());
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
    line.reserve(deltax + 1);

    #pragma omp parallel for ordered
    for (int x = 0; x < deltax + 1; ++x)
    {
        #pragma omp ordered
        {
            line.push_back(Point(y, x));
            err += deltaerr;

            if (2 * err >= deltax)
            {
                ++y;
                err -= deltax;
            }
        }
    }

    std::vector<std::vector<Point>> out{};

    for (int di = -deltay; di < static_cast<int>(height); ++di)
    {
        out.push_back({});
        unsigned int lineSize = 0;

        #pragma omp parallel for
        for (int i = 0; i < line.size(); ++i)
        {
            unsigned int pointI = line[i].i + di;

            if (0 <= pointI && pointI < height)
                ++lineSize;
            else if (lineSize > 0)
                break;
        }

        out[di + deltay].reserve(lineSize);
    }

    #pragma omp parallel for
    for (int di = -deltay; di < static_cast<int>(height); ++di)
    {
        for (const Point& p : line)
        {
            unsigned int i = p.i + di;
            unsigned int j = p.j;

            if (0 <= i && i < height)
                out[di + deltay].push_back(Point(i, j));
            else if (out[di + deltay].size() > 0)
                break;
        }
    }

    #pragma omp parallel for
    for (int i = 0; i < out.size(); ++i)
    {
        for (int j = 0; j < out[i].size(); ++j)
        {
            if (toFlipY)
                out[i][j].i = height - 1 - out[i][j].i;

            if (times % 2 == 1)
                out[i][j].j = width - 1 - out[i][j].j;

            if (toTranspose)
            {
                unsigned int temp = out[i][j].i;
                out[i][j].i = out[i][j].j;
                out[i][j].j = temp;
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
    int endSize = 0;

    #pragma omp parallel for
    for (int i = 0; i < path->size(); ++i)
        endSize += (*path)[i].size();

    (*path)[0].reserve(endSize);

    #pragma omp parallel for ordered
    for (int i = 1; i < path->size(); ++i)
    {
        #pragma omp ordered
        (*path)[0].insert((*path)[0].end(), (*path)[i].begin(), (*path)[i].end());
    }

    path->erase(path->begin() + 1, path->end());

    //while ((*path).size() > 1)
    //{
    //    (*path)[0].insert((*path)[0].end(), (*path)[1].begin(), (*path)[1].end());
    //    path->erase(path->begin() + 1);
    //}

    //std::cout << "Merge done\n";
}

void Sorter::reverseSort(std::vector<std::vector<Point>>* path)
{
    #pragma omp parallel for
    for (int i = 0; i < path->size(); ++i)
        std::reverse((*path)[i].begin(), (*path)[i].end());
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

    //std::cout << "Mirror done\n";
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
                segment = { seq.begin() + start, seq.begin() + start + size };
            else
                segment = { seq.begin() + start, seq.end() };

            start += size;

            if (segment.size() > 0)
                out.push_back(segment);
        }
    }

    //std::cout << "Intervals done\n";

    return out;
}

cv::Mat Sorter::getEdges(int lowThreshold, int highThreshold, int kernelSize)
{
    const unsigned char *pixPtr = image->getPixelsPtr();
    cv::Mat src(static_cast<int>(height), static_cast<int>(width), CV_8UC4, (void*)pixPtr);

    cv::Mat edges;
    src.copyTo(edges);

    cv::cvtColor(edges, edges, cv::COLOR_RGBA2GRAY);
    cv::blur(edges, edges, cv::Size(3, 3));
    cv::Canny(edges, edges, lowThreshold, highThreshold, kernelSize);

    //cv::namedWindow("DEBUGGING", cv::WINDOW_AUTOSIZE);
    //cv::imshow("DEBUGGING", edges);
    //cv::waitKey(0);

    //std::cout << "GetEdges done\n";

    return edges;
}

std::vector<std::vector<Point>> Sorter::edgesRows(int lowThreshold, int highThreshold, int kernelSize)
{
    std::vector<std::vector<Point>> out{};
    cv::Mat edges = getEdges(lowThreshold, highThreshold, kernelSize);

    //cv::namedWindow("DEBUGGING", cv::WINDOW_AUTOSIZE);
    //cv::imshow("DEBUGGING", edges);
    //cv::waitKey(0);

    for (unsigned int i = 0; i < height; ++i)
    {
        out.push_back({});
        unsigned char *row = edges.ptr<unsigned char>(i);

        for (unsigned int j = 0; j < width; ++j)
            if (row[j] > 0 && out[out.size() - 1].size() > 0)
                out.push_back({});
            else
                out[out.size() - 1].push_back(Point(i, j));

        if (out[out.size() - 1].size() == 0)
            out.erase(out.end() - 1);
    }

    //std::cout << "EdgesRows done\n";

    return out;
}