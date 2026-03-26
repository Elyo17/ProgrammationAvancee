#include "../io.h"
#include "student_functions.h"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>

using namespace std;
using namespace chrono;

enum Mode { CPU, CUDA, BOTH };

template <typename T>
struct ComparisonResult {
    bool same = true;
    bool dimensionMismatch = false;
    size_t mismatchCount = 0;
    int firstX = -1;
    int firstY = -1;
    T cpuValue{};
    T gpuValue{};
};

template <typename T>
ComparisonResult<T> compareBuffers(const T* cpuData, const T* gpuData, int width, size_t size) {
    ComparisonResult<T> result;

    for (size_t i = 0; i < size; ++i) {
        if (cpuData[i] != gpuData[i]) {
            if (result.mismatchCount == 0) {
                result.firstX = static_cast<int>(i % static_cast<size_t>(width));
                result.firstY = static_cast<int>(i / static_cast<size_t>(width));
                result.cpuValue = cpuData[i];
                result.gpuValue = gpuData[i];
            }
            ++result.mismatchCount;
        }
    }

    result.same = (result.mismatchCount == 0);
    return result;
}

ComparisonResult<uint16_t> compareDepthImages(const DepthImage& cpu, const DepthImage& gpu) {
    ComparisonResult<uint16_t> result;

    if (cpu.width != gpu.width || cpu.height != gpu.height || cpu.data.size() != gpu.data.size()) {
        result.same = false;
        result.dimensionMismatch = true;
        return result;
    }

    return compareBuffers(cpu.data.data(), gpu.data.data(), cpu.width, cpu.data.size());
}

ComparisonResult<uint8_t> compareMaskImages(const Image8& cpu, const Image8& gpu) {
    ComparisonResult<uint8_t> result;

    if (cpu.width != gpu.width || cpu.height != gpu.height || cpu.data.size() != gpu.data.size()) {
        result.same = false;
        result.dimensionMismatch = true;
        return result;
    }

    return compareBuffers(cpu.data.data(), gpu.data.data(), cpu.width, cpu.data.size());
}

template <typename T>
bool printComparisonSummary(const string& stageName, const ComparisonResult<T>& result) {
    cout << "=== Verification " << stageName << " ===" << endl;

    if (result.dimensionMismatch) {
        cout << "Mismatch de dimensions entre les sorties CPU et CUDA." << endl;
        return false;
    }

    cout << "Pixels differents : " << result.mismatchCount << endl;

    if (!result.same) {
        cout << "Premiere difference : (" << result.firstX << ", " << result.firstY << ")" << endl;
        cout << "Valeur CPU : " << static_cast<unsigned int>(result.cpuValue)
             << ", Valeur CUDA : " << static_cast<unsigned int>(result.gpuValue) << endl;
        return false;
    }

    cout << "Correspondance pixel a pixel OK." << endl;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        cout << "Usage: " << argv[0] << " <depth_image_path> [mode]" << endl;
        cout << "Modes: cpu, cuda, both (default: cpu)" << endl;
        return 1;
    }

    string depthPath = argv[1];
    Mode mode = CPU;
    if (argc == 3) {
        string modeStr = argv[2];
        if (modeStr == "cpu") mode = CPU;
        else if (modeStr == "cuda") mode = CUDA;
        else if (modeStr == "both") mode = BOTH;
        else {
            cout << "Invalid mode. Use cpu, cuda, or both." << endl;
            return 1;
        }
    }

    DepthImage depth = loadDepth(depthPath);
    if (depth.data.empty()) {
        return 1;
    }

    DepthImage depthCpu = depth;
    DepthImage depthCuda = depth;
    DepthImage normalizedCpu;
    DepthImage invertedCpu;

    int threshold = 128;
    Image8 threshCpu;
    Image8 threshCuda;
    bool allMatch = true;

    if (mode == CPU || mode == BOTH) {
        cout << "Version CPU..." << endl;

        auto start = high_resolution_clock::now();
        normalizeDepth(depthCpu);
        auto end = high_resolution_clock::now();
        duration<double, milli> duration = end - start;
        cout << ">>> Normalisation CPU: " << duration.count() << " ms" << endl;
        saveDepth(depthCpu, "normalized_depth_cpu.bin");
        normalizedCpu = depthCpu;

        start = high_resolution_clock::now();
        invertDepth(depthCpu);
        end = high_resolution_clock::now();
        duration = end - start;
        cout << ">>> Inversion CPU: " << duration.count() << " ms" << endl;
        saveDepth(depthCpu, "inverted_depth_cpu.bin");
        invertedCpu = depthCpu;

        start = high_resolution_clock::now();
        threshCpu = thresholdDepth(depthCpu, threshold);
        end = high_resolution_clock::now();
        duration = end - start;
        cout << ">>> Seuillage CPU: " << duration.count() << " ms" << endl;
        saveImage(threshCpu, "thresholded_depth_cpu.png");
    }

    if (mode == CUDA || mode == BOTH) {
        cout << "Version CUDA..." << endl;

        auto start = high_resolution_clock::now();
        cudaNormalizeDepth(depthCuda);
        auto end = high_resolution_clock::now();
        duration<double, milli> duration = end - start;
        cout << ">>> Normalisation CUDA: " << duration.count() << " ms" << endl;
        saveDepth(depthCuda, "normalized_depth_cuda.bin");

        if (mode == BOTH) {
            allMatch = printComparisonSummary("normalisation", compareDepthImages(normalizedCpu, depthCuda)) && allMatch;
        }

        start = high_resolution_clock::now();
        cudaInvertDepth(depthCuda);
        end = high_resolution_clock::now();
        duration = end - start;
        cout << ">>> Inversion CUDA: " << duration.count() << " ms" << endl;
        saveDepth(depthCuda, "inverted_depth_cuda.bin");

        if (mode == BOTH) {
            allMatch = printComparisonSummary("inversion", compareDepthImages(invertedCpu, depthCuda)) && allMatch;
        }

        start = high_resolution_clock::now();
        threshCuda = cudaThresholdDepth(depthCuda, threshold);
        end = high_resolution_clock::now();
        duration = end - start;
        cout << ">>> Seuillage CUDA: " << duration.count() << " ms" << endl;
        saveImage(threshCuda, "thresholded_depth_cuda.png");

        if (mode == BOTH) {
            allMatch = printComparisonSummary("seuillage", compareMaskImages(threshCpu, threshCuda)) && allMatch;
        }
    }

    if (mode == BOTH) {
        if (allMatch) {
            cout << "Verification terminee : CPU et CUDA sont identiques pixel a pixel." << endl;
        } else {
            cout << "Verification terminee : des differences CPU/CUDA ont ete detectees." << endl;
        }
    }

    cout << "TP1 termine. Images sauvegardees." << endl;

    return (mode == BOTH && !allMatch) ? 1 : 0;
}
