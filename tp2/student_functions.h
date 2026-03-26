#ifndef STUDENT_FUNCTIONS_H
#define STUDENT_FUNCTIONS_H

#include "../io.h"
#include <vector>
#include <string>

struct CropInfo {
    int originalWidth = 0;
    int originalHeight = 0;
    int cropX = 0;
    int cropY = 0;
    int cropW = 0;
    int cropH = 0;
};

struct PixelCoord {
    int x = 0;
    int y = 0;
};

int countOverlap(const Image8& mask1, const Image8& mask2);
int countUnion(const Image8& mask1, const Image8& mask2);
int countConnectedComponents(const Image8& mask);
std::vector<PixelCoord> extractComponentPixels(const Image8& mask, int startX, int startY, std::vector<uint8_t>& visited);
double computeComponentDiameter(const std::vector<PixelCoord>& pixels);
double computeLargestComponentDiameter(const Image8& mask);
double cudaComputeLargestComponentDiameter(const Image8& mask);
double computeSimpleScore(const Image8& mask1, const Image8& mask2);
double computeIoU(const Image8& mask1, const Image8& mask2);
bool loadCropInfo(const std::string& path, CropInfo& info);
Image8 reconstructFullMask(const Image8& croppedMask, const CropInfo& info);

#endif // STUDENT_FUNCTIONS_H
