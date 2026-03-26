#include "student_functions.h"
#include "../utils_cuda.h"
#include <algorithm>
#include <cuda_runtime.h>

// CUDA kernels for depth-image operations.
// Each kernel processes one pixel in parallel.

__global__ void kernelNormalize(uint16_t* depth, int width, int height, uint16_t d_min, uint16_t d_max) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height) return;

    int idx = y * width + x;

    if (d_max == d_min) return;

    const uint16_t M = 65535;
    depth[idx] = static_cast<uint16_t>((static_cast<double>(depth[idx] - d_min) / (d_max - d_min)) * M);
}

__global__ void kernelInvert(uint16_t* depth, int width, int height) {
    // TODO: Implement the inversion kernel.
    // Steps:
    // 1. Compute the pixel coordinates (x, y).
    // 2. Check that the thread is inside the image bounds.
    // 3. Convert (x, y) to a linear index.
    // 4. Invert the depth value in place with: 65535 - depth[idx].
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    if (x >= width || y >= height) return;
    int idx = y * width + x;
    const uint16_t M = 65535;
    depth[idx] = M - depth[idx];
}

__global__ void kernelThreshold(const uint16_t* depth, uint8_t* output, int width, int height, int threshold) {
    // TODO: Implement the threshold kernel.
    // Steps:
    // 1. Compute the pixel coordinates (x, y).
    // 2. Check that the thread is inside the image bounds.
    // 3. Convert (x, y) to a linear index.
    // 4. Write 255 if depth[idx] > threshold, otherwise 0.
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height) return;

    int idx = y * width + x;
    output[idx] = static_cast<uint8_t>(depth[idx] > threshold ? 255 : 0);
}

// Host wrappers that allocate GPU memory, transfer data, launch kernels,
// and copy the results back.
void cudaNormalizeDepth(DepthImage& depth) {
    uint16_t* d_depth;
    size_t size = depth.data.size() * sizeof(uint16_t);
    CUDA_CHECK(cudaMalloc(&d_depth, size));

    CUDA_CHECK(cudaMemcpy(d_depth, depth.data.data(), size, cudaMemcpyHostToDevice));

    auto min_it = std::min_element(depth.data.begin(), depth.data.end());
    auto max_it = std::max_element(depth.data.begin(), depth.data.end());
    uint16_t d_min = *min_it;
    uint16_t d_max = *max_it;

    dim3 block(16, 16);
    dim3 grid((depth.width + block.x - 1) / block.x, (depth.height + block.y - 1) / block.y);

    kernelNormalize<<<grid, block>>>(d_depth, depth.width, depth.height, d_min, d_max);

    CUDA_CHECK(cudaMemcpy(depth.data.data(), d_depth, size, cudaMemcpyDeviceToHost));
    CUDA_CHECK(cudaFree(d_depth));
}

void cudaInvertDepth(DepthImage& depth) {
    // TODO: Implement the CUDA wrapper for inversion.
    // Steps:
    // 1. Allocate GPU memory for the depth image.
    // 2. Copy the input data from host to device.
    // 3. Configure a 16x16 block and a grid covering the full image.
    // 4. Launch kernelInvert.
    // 5. Copy the result back from device to host.
    // 6. Free the GPU memory.
    uint16_t* d_depth;
    size_t size = depth.data.size() * sizeof(uint16_t);
    CUDA_CHECK(cudaMalloc(&d_depth, size));

    CUDA_CHECK(cudaMemcpy(d_depth, depth.data.data(), size, cudaMemcpyHostToDevice));

    dim3 block(16, 16);
    dim3 grid((depth.width + block.x - 1) / block.x, (depth.height + block.y - 1) / block.y);

    kernelInvert<<<grid, block>>>(d_depth, depth.width, depth.height);

    CUDA_CHECK(cudaMemcpy(depth.data.data(), d_depth, size, cudaMemcpyDeviceToHost));
    CUDA_CHECK(cudaFree(d_depth));
}

Image8 cudaThresholdDepth(const DepthImage& depth, int threshold) {
    uint16_t* d_depth;
    uint8_t* d_output;
    size_t size_depth = depth.data.size() * sizeof(uint16_t);
    size_t size_output = depth.data.size() * sizeof(uint8_t);

    CUDA_CHECK(cudaMalloc(&d_depth, size_depth));
    CUDA_CHECK(cudaMalloc(&d_output, size_output));

    CUDA_CHECK(cudaMemcpy(d_depth, depth.data.data(), size_depth, cudaMemcpyHostToDevice));

    dim3 block(16, 16);
    dim3 grid((depth.width + block.x - 1) / block.x, (depth.height + block.y - 1) / block.y);

    kernelThreshold<<<grid, block>>>(d_depth, d_output, depth.width, depth.height, threshold);

    Image8 result;
    result.width = depth.width;
    result.height = depth.height;
    result.data.resize(depth.data.size());

    CUDA_CHECK(cudaMemcpy(result.data.data(), d_output, size_output, cudaMemcpyDeviceToHost));

    CUDA_CHECK(cudaFree(d_depth));
    CUDA_CHECK(cudaFree(d_output));

    return result;
}
