#include "student_functions.h"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <cstddef>
#include <cstring>
#include <stdexcept>

// Ce fichier ne contient pas l'algorithme CUDA.
// Son role est seulement de faire le pont entre :
// - des objets Python / NumPy ;
// - et des fonctions C++ / CUDA qui travaillent sur des buffers bruts.

pybind11::array_t<uint8_t> threshold_depth_binding(
    pybind11::array_t<uint16_t, pybind11::array::c_style | pybind11::array::forcecast> depth,
    int threshold) {
    // Etape 1 : récupérer les infos bas niveau
    pybind11::buffer_info buf = depth.request();

    // 1. Vérifier que c’est bien 2D
    if (buf.ndim != 2) {
        throw std::runtime_error("Input depth image must be 2D");
    }

    // 2. Récupérer height, width et pointeur
    int height = static_cast<int>(buf.shape[0]);
    int width = static_cast<int>(buf.shape[1]);

    uint16_t* depth_ptr = static_cast<uint16_t*>(buf.ptr);

    // 3. Appeler ta fonction CUDA
    Image8 result = thresholdDepthBuffer(depth_ptr, width, height, threshold);

    // 4. Créer un array numpy de sortie
    pybind11::array_t<uint8_t> output({ height, width });

    pybind11::buffer_info out_buf = output.request();
    uint8_t* out_ptr = static_cast<uint8_t*>(out_buf.ptr);

    // Copier les données
    std::memcpy(out_ptr, result.data.data(), result.data.size() * sizeof(uint8_t));

    return output;

}

double largest_component_diameter_binding(
    pybind11::array_t<uint8_t, pybind11::array::c_style | pybind11::array::forcecast> mask) {

    // Etape 1 : récupérer les infos bas niveau
    pybind11::buffer_info buf = mask.request();

    // 1. Vérifier que c’est bien 2D
    if (buf.ndim != 2) {
        throw std::runtime_error("Input mask must be 2D");
    }

    // 2. Récupérer height, width et pointeur
    int height = static_cast<int>(buf.shape[0]);
    int width = static_cast<int>(buf.shape[1]);

    uint8_t* mask_ptr = static_cast<uint8_t*>(buf.ptr);

    // 3. Appeler la fonction CUDA
    double result = largestComponentDiameterBuffer(mask_ptr, width, height);

    // 4. Retourner le résultat
    return result;
}

PYBIND11_MODULE(_tp4_pybind_cuda, m) {
    m.doc() = "TP4 CUDA bindings with pybind11";

    // 1. exposer threshold_depth(...)
    m.def("threshold_depth",
        &threshold_depth_binding,
        "Apply a threshold to a depth image");

    // 2. exposer largest_component_diameter(...)
    m.def("largest_component_diameter",
        &largest_component_diameter_binding,
        "Compute largest component diameter from a binary mask");
    throw std::runtime_error("Not implemented");
}
