#ifndef STUDENT_FUNCTIONS_H
#define STUDENT_FUNCTIONS_H

#include "../io.h"
#include <cstdint>

void normalizeDepth(DepthImage& depth);
void invertDepth(DepthImage& depth);
Image8 thresholdDepth(const DepthImage& depth, uint16_t threshold);

Image8 erodeMask(const Image8& mask);
Image8 dilateMask(const Image8& mask);
Image8 openMask(const Image8& mask);
Image8 closeMask(const Image8& mask);
Image8 cropMask(const Image8& mask, int cropX, int cropY, int cropW, int cropH);

DepthImage maskDepth(const DepthImage& depth, const Image8& mask);
DepthImage cropDepth(const DepthImage& depth, int cropX, int cropY, int cropW, int cropH);

#endif // STUDENT_FUNCTIONS_H
