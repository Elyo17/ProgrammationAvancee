#include "student_functions.h"
#include <algorithm>

void normalizeDepth(DepthImage& depth) {
    if (depth.data.empty()) {
        return;
    }

    // TODO: Find the minimum and maximum depth values in the image.

    // Avoid division by zero if all values are the same.

    // TODO: Apply the linear normalization formula to map values to [0, 65535].

}

void invertDepth(DepthImage& depth) {
    for (auto& value : depth.data) {
        // TODO: Invert each depth value in place with: 65535 - value.
        value = static_cast<uint16_t>(65535u - value);
    }
}

Image8 thresholdDepth(const DepthImage& depth, uint16_t threshold) {
    Image8 result;
    result.width = depth.width;
    result.height = depth.height;
    result.data.resize(depth.data.size(), 0);
    //TODO loop on each pixel
    return result;
}

Image8 erodeMask(const Image8& mask) {
    Image8 result;
    result.width = mask.width;
    result.height = mask.height;
    result.data.resize(mask.data.size(), 0);
    //TODO loop to erode one time
    return result;
}

Image8 dilateMask(const Image8& mask) {
    Image8 result;
    result.width = mask.width;
    result.height = mask.height;
    result.data.resize(mask.data.size(), 0);
    //TODO loop to dilate one time

    return result;
}

Image8 openMask(const Image8& mask) {
    // TODO: Opening = erosion followed by dilation.

}

Image8 closeMask(const Image8& mask) {
    // TODO: Closing = dilation followed by erosion.

}

Image8 cropMask(const Image8& mask, int cropX, int cropY, int cropW, int cropH) {
    Image8 result;
    result.width = 0;
    result.height = 0;

    if (mask.width <= 0 || mask.height <= 0 || mask.data.empty()) {
        return result;
    }

    if (cropX >= mask.width || cropY >= mask.height) {
        return result;
    }

    const int effectiveWidth = std::min(cropW, mask.width - cropX);
    const int effectiveHeight = std::min(cropH, mask.height - cropY);
    if (effectiveWidth <= 0 || effectiveHeight <= 0) {
        return result;
    }

    result.width = effectiveWidth;
    result.height = effectiveHeight;
    result.data.resize(static_cast<size_t>(result.width) * result.height);

    return result;
}

DepthImage maskDepth(const DepthImage& depth, const Image8& mask) {
    DepthImage result;
    result.width = depth.width;
    result.height = depth.height;
    result.data.resize(depth.data.size(), 0);

    const size_t count = std::min(depth.data.size(), mask.data.size());
    for (size_t i = 0; i < count; ++i) {
        // TODO: Keep the depth value for foreground pixels and set the background to 0.
    }

    return result;
}

DepthImage cropDepth(const DepthImage& depth, int cropX, int cropY, int cropW, int cropH) {
    DepthImage result;
    result.width = 0;
    result.height = 0;

    if (depth.width <= 0 || depth.height <= 0 || depth.data.empty()) {
        return result;
    }

    if (cropX >= depth.width || cropY >= depth.height) {
        return result;
    }

    const int effectiveWidth = std::min(cropW, depth.width - cropX);
    const int effectiveHeight = std::min(cropH, depth.height - cropY);
    if (effectiveWidth <= 0 || effectiveHeight <= 0) {
        return result;
    }

    result.width = effectiveWidth;
    result.height = effectiveHeight;
    result.data.resize(static_cast<size_t>(result.width) * result.height);

    // TODO: Copy the selected rectangular region into the output depth image.


    return result;
}
