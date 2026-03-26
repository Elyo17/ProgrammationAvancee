#include "student_functions.h"
#include <algorithm>
#include <limits>

void normalizeDepth(DepthImage& depth) {
    // TODO: Implement linear normalization to [0, 65535].
    // Steps:
    // 1. Handle the empty image case.
    // 2. Find d_min and d_max in depth.data.
    // 3. If d_min == d_max, return to avoid division by zero.
    // 4. Apply: (value - d_min) / (d_max - d_min) * 65535
    if (depth.data.empty()) {
        return;
    }

    const auto [min_it, max_it] = std::minmax_element(depth.data.begin(), depth.data.end());
    const uint16_t d_min = *min_it;
    const uint16_t d_max = *max_it;

    if (d_min == d_max) {
        return;
    }

    const uint32_t range = static_cast<uint32_t>(d_max) - d_min;

    for (auto& value : depth.data) {
        value = static_cast<uint16_t>(
            ((static_cast<uint32_t>(value) - d_min) * 65535u) / range
        );
    }
}

void invertDepth(DepthImage& depth) {
    const uint16_t M = 65535;

    for (auto& value : depth.data) {
        value = M - value;
    }
}

Image8 thresholdDepth(const DepthImage& depth, int threshold) {
    Image8 result;
    result.width = depth.width;
    result.height = depth.height;
    result.data.resize(depth.data.size());

    for (size_t i = 0; i < depth.data.size(); ++i) {
        result.data[i] = (depth.data[i] > threshold) ? 255 : 0;
    }

    return result;
}
