#include "student_functions.h"

#include <cmath>
#include <cuda_runtime.h>
#include <iostream>
#include <vector>

#define CUDA_CHECK_LOCAL(call) \
    do { \
        cudaError_t err = (call); \
        if (err != cudaSuccess) { \
            std::cerr << "Erreur CUDA : " << cudaGetErrorString(err) \
                      << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            exit(1); \
        } \
    } while (0)

// Each thread evaluates one candidate pair of pixels inside the same component.
// The goal is to find the largest squared Euclidean distance among all pairs.
// Input:
// - pixels: coordinates of the pixels belonging to one connected component
// - count: number of pixels in that component
// - maxDistanceSquared: output value on the GPU for the largest squared distance
//
// Each thread evaluates one candidate pair of pixels.
__global__ void kernelComponentDiameter(const PixelCoord* pixels, int count, int* maxDistanceSquared) {
    // TODO: Compute the global linear thread index and the total number of pairs.

    // Threads outside the virtual pair matrix do nothing.
    // TODO: Ignore threads whose index is outside the NxN pair matrix.


    // Map the linear thread index to a pair (i, j) in the NxN pair matrix.
    // TODO: Convert the linear index into the pair coordinates (i, j).


    // Keep only the upper triangle to avoid duplicate work and self-pairs.
    // TODO: Skip self-pairs and duplicate pairs.


    // Compute the squared Euclidean distance between pixels i and j.
    // TODO: Compute dx, dy and the squared distance for this pair.


    // Several threads may find a better distance at the same time,
    // so we use an atomic max on the shared output value.
    // TODO: Update the global maximum with an atomic operation.

}

namespace {

double cudaComputeComponentDiameter(const std::vector<PixelCoord>& pixels) {
    // A component with 0 or 1 pixel has a diameter of 0.
    if (pixels.size() < 2) {
        return 0.0;
    }

    PixelCoord* d_pixels = nullptr;
    int* d_maxDistanceSquared = nullptr;

    // Copy the component pixels to the GPU and allocate one integer
    // to store the best squared distance found by the kernel.
    CUDA_CHECK_LOCAL(cudaMalloc(&d_pixels, pixels.size() * sizeof(PixelCoord)));
    CUDA_CHECK_LOCAL(cudaMalloc(&d_maxDistanceSquared, sizeof(int)));
    CUDA_CHECK_LOCAL(cudaMemcpy(d_pixels, pixels.data(), pixels.size() * sizeof(PixelCoord), cudaMemcpyHostToDevice));
    CUDA_CHECK_LOCAL(cudaMemset(d_maxDistanceSquared, 0, sizeof(int)));

    // We launch one thread per pair in the virtual NxN matrix.
    // TODO: Compute the total number of candidate pairs.
    // TODO: Choose a block size and derive the grid size from totalPairs.



    kernelComponentDiameter<<<gridSize, blockSize>>>(d_pixels, static_cast<int>(pixels.size()), d_maxDistanceSquared);
    
    CUDA_CHECK_LOCAL(cudaGetLastError());
    CUDA_CHECK_LOCAL(cudaDeviceSynchronize());
    int maxDistanceSquared = 0;
    CUDA_CHECK_LOCAL(cudaMemcpy(&maxDistanceSquared, d_maxDistanceSquared, sizeof(int), cudaMemcpyDeviceToHost));
    CUDA_CHECK_LOCAL(cudaFree(d_pixels));
    CUDA_CHECK_LOCAL(cudaFree(d_maxDistanceSquared));

    // Convert back to the true Euclidean distance only once on the CPU.
    return std::sqrt(static_cast<double>(maxDistanceSquared));
}

} // namespace

double cudaComputeLargestComponentDiameter(const Image8& mask) {
    // Empty masks have no component and therefore a diameter of 0.
    if (mask.width <= 0 || mask.height <= 0 || mask.data.empty()) {
        return 0.0;
    }

    // Components are still extracted on the CPU. CUDA is used for the
    // expensive brute-force pairwise distance computation inside each component.
    std::vector<uint8_t> visited(mask.data.size(), 0);
    double largestDiameter = 0.0;

    // For each connected component in the mask
    for (int y = 0; y < mask.height; ++y) {
        for (int x = 0; x < mask.width; ++x) {
            const size_t index = static_cast<size_t>(y) * mask.width + x;
            if (mask.data[index] == 0 || visited[index] != 0) {
                continue;
            }

            // Extract one connected component, then send only its pixels
            // to the GPU to compute its exact diameter.
            const std::vector<PixelCoord> pixels = extractComponentPixels(mask, x, y, visited);
            const double diameter = cudaComputeComponentDiameter(pixels);
            if (diameter > largestDiameter) {
                largestDiameter = diameter;
            }
        }
    }

    return largestDiameter;
}
