#include "student_functions.h"
#include <cmath>
#include <fstream>
#include <queue>
#include <string>

int countOverlap(const Image8& mask1, const Image8& mask2) {
    if (mask1.width != mask2.width || mask1.height != mask2.height || mask1.data.size() != mask2.data.size()) {
        return 0;
    }

    // TODO: Count the number of pixels where both masks are non-zero.

    return 0;
}

int countUnion(const Image8& mask1, const Image8& mask2) {
    if (mask1.width != mask2.width || mask1.height != mask2.height || mask1.data.size() != mask2.data.size()) {
        return 0;
    }

    // TODO: Count the number of pixels where at least one of the two masks is non-zero.


    return 0;
}

std::vector<PixelCoord> extractComponentPixels(const Image8& mask, int startX, int startY, std::vector<uint8_t>& visited) {
    // This function extracts one full connected component from the mask.
    // Starting from one white pixel (startX, startY), it explores all
    // 8-connected white neighbors, marks them as visited, and returns
    // the list of all pixel coordinates belonging to that component.
    std::vector<PixelCoord> pixels;

    if (mask.width <= 0 || mask.height <= 0 || mask.data.empty() ||
        startX < 0 || startX >= mask.width || startY < 0 || startY >= mask.height) {
        return pixels;
    }

    const size_t startIndex = static_cast<size_t>(startY) * mask.width + startX;
    if (mask.data[startIndex] == 0 || visited[startIndex] != 0) {
        return pixels;
    }

    std::queue<PixelCoord> frontier;
    frontier.push({startX, startY});
    visited[startIndex] = 1;

    // TODO: Use a BFS/queue to visit the whole connected component.
    // Explore the 8 neighbors of the current pixel.
    // Mark the neighbor as visited and push it into the queue.

    return pixels;
}

double computeComponentDiameter(const std::vector<PixelCoord>& pixels) {
    // This function computes the exact diameter of one connected component.
    // The diameter is the largest Euclidean distance between any two pixels
    // belonging to that component.
    if (pixels.size() < 2) {
        return 0.0;
    }

    // TODO: Compare all pairs of pixels and keep the largest squared distance.


    // TODO: Convert the best squared distance into the true Euclidean distance.
    return 0.0;
}
double computeLargestComponentDiameter(const Image8& mask) {
    // This function scans the whole mask, extracts each connected component,
    // computes its exact diameter, and keeps the largest diameter found.
    if (mask.width <= 0 || mask.height <= 0 || mask.data.empty()) {
        return 0.0;
    }

    std::vector<uint8_t> visited(mask.data.size(), 0);
    double largestDiameter = 0.0;

    // TODO: Traverse the whole image to detect each new connected component.
    for (int y = 0; y < mask.height; ++y) {
        for (int x = 0; x < mask.width; ++x) {
            const size_t index = static_cast<size_t>(y) * mask.width + x;
            if (mask.data[index] == 0 || visited[index] != 0) {
                continue;
            }

            // TODO: Extract the current component and compute its diameter.


            // TODO: Keep the largest diameter among all components.

        }
    }

    return 0.0;
}

int countConnectedComponents(const Image8& mask) {

    if (mask.width <= 0 || mask.height <= 0 || mask.data.empty()) {
        return 0;
    }
    // TODO: Count connected components in a binary mask using 8-connectivity.

}

double computeSimpleScore(const Image8& mask1, const Image8& mask2) {

    if (mask1.width != mask2.width || mask1.height != mask2.height || mask1.data.size() != mask2.data.size() ||
        mask1.data.empty()) {
        return 0.0;
    }
    // TODO: Compute a simple score defined as overlap / total_pixels.

}

double computeIoU(const Image8& mask1, const Image8& mask2) {

    if (mask1.width != mask2.width || mask1.height != mask2.height || mask1.data.size() != mask2.data.size()) {
        return 0.0;
    }
 
    // TODO: Compute the Union.
    int unionCount = 0;
    if (unionCount == 0) {
        return 1.0;
    }
    // TODO: Compute the Intersection over Union (IoU) between the two binary masks.

}

bool loadCropInfo(const std::string& path, CropInfo& info) {
    std::ifstream file(path);
    if (!file) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        const size_t sep = line.find('=');
        if (sep == std::string::npos) {
            continue;
        }

        const std::string key = line.substr(0, sep);
        const int value = std::stoi(line.substr(sep + 1));

        if (key == "originalWidth") info.originalWidth = value;
        else if (key == "originalHeight") info.originalHeight = value;
        else if (key == "cropX") info.cropX = value;
        else if (key == "cropY") info.cropY = value;
        else if (key == "cropW") info.cropW = value;
        else if (key == "cropH") info.cropH = value;
    }

    return info.originalWidth > 0 && info.originalHeight > 0 &&
           info.cropW > 0 && info.cropH > 0 &&
           info.cropX >= 0 && info.cropY >= 0;
}

Image8 reconstructFullMask(const Image8& croppedMask, const CropInfo& info) {
    Image8 fullMask;
    fullMask.width = info.originalWidth;
    fullMask.height = info.originalHeight;

    if (fullMask.width <= 0 || fullMask.height <= 0) {
        fullMask.width = 0;
        fullMask.height = 0;
        return fullMask;
    }

    if (croppedMask.width != info.cropW || croppedMask.height != info.cropH) {
        fullMask.width = 0;
        fullMask.height = 0;
        return fullMask;
    }

    if (info.cropX + info.cropW > info.originalWidth || info.cropY + info.cropH > info.originalHeight) {
        fullMask.width = 0;
        fullMask.height = 0;
        return fullMask;
    }

    fullMask.data.assign(static_cast<size_t>(fullMask.width) * fullMask.height, 0);

    for (int y = 0; y < croppedMask.height; ++y) {
        for (int x = 0; x < croppedMask.width; ++x) {
            // TODO: Reconstruct the full mask by placing the cropped mask at the correct position in a new image of size (originalWidth, originalHeight).
        }
    }

    return fullMask;
}
