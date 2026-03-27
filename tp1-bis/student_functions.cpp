#include "student_functions.h"
#include <algorithm>

void normalizeDepth(DepthImage& depth) {
    if (depth.data.empty()) {
        return;
    }

    // Trouver le min et le max
    uint16_t minVal = *std::min_element(depth.data.begin(), depth.data.end());
    uint16_t maxVal = *std::max_element(depth.data.begin(), depth.data.end());

    // Éviter la division par zéro si tous les pixels ont la même valeur
    if (minVal == maxVal) {
        return;
    }

    // Normalisation linéaire vers [0, 65535]
    for (auto& value : depth.data) {
        value = static_cast<uint16_t>(
            (static_cast<uint32_t>(value - minVal) * 65535u) / (maxVal - minVal)
         );
    }

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

    for (size_t i = 0; i < depth.data.size(); ++i) {
        result.data[i] = (depth.data[i] <= threshold) ? 255 : 0;
    }

    return result;
}

Image8 erodeMask(const Image8& mask) {
    Image8 result;
    result.width = mask.width;
    result.height = mask.height;
    result.data.resize(mask.data.size(), 0);
    for (int y = 0; y < mask.height; ++y) {
        for (int x = 0; x < mask.width; ++x) {
            bool keep = true;
            // Voisinage 3x3 (kernel carré)
            for (int dy = -1; dy <= 1 && keep; ++dy) {
                for (int dx = -1; dx <= 1 && keep; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx < 0 || nx >= mask.width || ny < 0 || ny >= mask.height) {
                        keep = false; // bord traité comme background
                    }
                    else {
                        if (mask.data[ny * mask.width + nx] == 0)
                            keep = false;
                    }
                }
            }
            result.data[y * mask.width + x] = keep ? 255 : 0;
        }
    }
    return result;
}

Image8 dilateMask(const Image8& mask) {
    Image8 result;
    result.width = mask.width;
    result.height = mask.height;
    result.data.resize(mask.data.size(), 0);
    for (int y = 0; y < mask.height; ++y) {
        for (int x = 0; x < mask.width; ++x) {
            bool activate = false;
            // Voisinage 3x3
            for (int dy = -1; dy <= 1 && !activate; ++dy) {
                for (int dx = -1; dx <= 1 && !activate; ++dx) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < mask.width && ny >= 0 && ny < mask.height) {
                        if (mask.data[ny * mask.width + nx] == 255)
                            activate = true;
                    }
                }
            }
            result.data[y * mask.width + x] = activate ? 255 : 0;
        }
    }

    return result;
}

Image8 openMask(const Image8& mask) {
    // Opening = érosion puis dilatation
    return dilateMask(erodeMask(mask));

}

Image8 closeMask(const Image8& mask) {
    // Closing = dilatation puis érosion
    return erodeMask(dilateMask(mask));

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

    for (int y = 0; y < effectiveHeight; ++y) {
        for (int x = 0; x < effectiveWidth; ++x) {
            result.data[y * result.width + x] =
                mask.data[(cropY + y) * mask.width + (cropX + x)];
        }
    }

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
        result.data[i] = (mask.data[i] == 255) ? depth.data[i] : 0;
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
    for (int y = 0; y < effectiveHeight; ++y) {
        for (int x = 0; x < effectiveWidth; ++x) {
            result.data[y * result.width + x] =
                depth.data[(cropY + y) * depth.width + (cropX + x)];
        }
    }


    return result;
}
