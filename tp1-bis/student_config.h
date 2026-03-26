#ifndef STUDENT_CONFIG_H
#define STUDENT_CONFIG_H

struct TP1BisConfig {
    const char* defaultDepthPath = "../data/D/pointCloud_data_2026-02-19_12-03-32_depth.tiff";
    int defaultThreshold = 0;
    int cropX = 0;
    int cropY = 0;
    int cropW = 0;
    int cropH = 0;
};

inline constexpr TP1BisConfig kStudentConfig{};

#endif // STUDENT_CONFIG_H
