#include "../io.h"
#include "student_functions.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Usage: " << argv[0] << " <pred_mask_path> <crop_info_path> <gt_mask_path>" << std::endl;
        return 1;
    }

    const std::string predMaskPath = argv[1];
    const std::string cropInfoPath = argv[2];
    const std::string gtMaskPath = argv[3];

    Image8 croppedPredMask = loadImage(predMaskPath);
    Image8 gtMask = loadImage(gtMaskPath);
    CropInfo cropInfo;

    if (croppedPredMask.data.empty() || gtMask.data.empty()) {
        std::cerr << "Erreur : impossible de charger les masques." << std::endl;
        return 1;
    }

    if (!loadCropInfo(cropInfoPath, cropInfo)) {
        std::cerr << "Erreur : impossible de charger les metadata de crop." << std::endl;
        return 1;
    }

    Image8 predMask = reconstructFullMask(croppedPredMask, cropInfo);
    if (predMask.data.empty()) {
        std::cerr << "Erreur : impossible de reconstruire le masque pleine taille." << std::endl;
        return 1;
    }

    if (predMask.width != gtMask.width || predMask.height != gtMask.height) {
        std::cerr << "Erreur : les deux masques doivent avoir les memes dimensions." << std::endl;
        return 1;
    }

    std::cout << "Dimensions prediction reconstruite : " << predMask.width << " x " << predMask.height << std::endl;
    std::cout << "Dimensions ground truth : " << gtMask.width << " x " << gtMask.height << std::endl;

    const auto metricsStart = std::chrono::high_resolution_clock::now();

    const int overlap = countOverlap(predMask, gtMask);
    const int unionCount = countUnion(predMask, gtMask);
    const int predComponents = countConnectedComponents(predMask);
    const int gtComponents = countConnectedComponents(gtMask);
    const int componentDifference = std::abs(predComponents - gtComponents);

    const auto cpuDiameterStart = std::chrono::high_resolution_clock::now();
    const double predLargestDiameterCpu = computeLargestComponentDiameter(predMask);
    const double gtLargestDiameterCpu = computeLargestComponentDiameter(gtMask);
    const auto cpuDiameterEnd = std::chrono::high_resolution_clock::now();
    const auto cpuDiameterMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(cpuDiameterEnd - cpuDiameterStart).count();

    const auto cudaDiameterStart = std::chrono::high_resolution_clock::now();
    const double predLargestDiameter = cudaComputeLargestComponentDiameter(predMask);
    const double gtLargestDiameter = cudaComputeLargestComponentDiameter(gtMask);
    const auto cudaDiameterEnd = std::chrono::high_resolution_clock::now();
    const auto cudaDiameterMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(cudaDiameterEnd - cudaDiameterStart).count();

    const double diameterDifference = std::abs(predLargestDiameter - gtLargestDiameter);
    const double iou = computeIoU(predMask, gtMask);
    const double score = computeSimpleScore(predMask, gtMask);

    const auto metricsEnd = std::chrono::high_resolution_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(metricsEnd - metricsStart).count();

    std::cout << "Overlap : " << overlap << std::endl;
    std::cout << "Union : " << unionCount << std::endl;
    std::cout << "Composantes connexes prediction : " << predComponents << std::endl;
    std::cout << "Composantes connexes ground truth : " << gtComponents << std::endl;
    std::cout << "Difference de composantes connexes : " << componentDifference << std::endl;
    std::cout << "Diametre maximal prediction (C++) : " << predLargestDiameterCpu << std::endl;
    std::cout << "Diametre maximal ground truth (C++) : " << gtLargestDiameterCpu << std::endl;
    std::cout << "Diametre maximal prediction : " << predLargestDiameter << std::endl;
    std::cout << "Diametre maximal ground truth : " << gtLargestDiameter << std::endl;
    std::cout << "Difference de diametre maximal : " << diameterDifference << std::endl;
    std::cout << "IoU : " << iou << std::endl;
    std::cout << "Score simple : " << score << std::endl;
    std::cout << "Temps diametre C++ : " << cpuDiameterMs << " ms" << std::endl;
    std::cout << "Temps diametre CUDA : " << cudaDiameterMs << " ms" << std::endl;
    std::cout << "Temps de calcul des metriques : " << elapsedMs << " ms" << std::endl;

    return 0;
}
