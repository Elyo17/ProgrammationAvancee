#ifndef STUDENT_FUNCTIONS_H
#define STUDENT_FUNCTIONS_H

#include "../io.h"

// CPU versions
void normalizeDepth(DepthImage& depth);
void invertDepth(DepthImage& depth);
Image8 thresholdDepth(const DepthImage& depth, int threshold);

// CUDA versions
void cudaNormalizeDepth(DepthImage& depth);
void cudaInvertDepth(DepthImage& depth);
Image8 cudaThresholdDepth(const DepthImage& depth, int threshold);

#endif // STUDENT_FUNCTIONS_H