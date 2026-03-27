#ifndef STUDENT_CONFIG_H
#define STUDENT_CONFIG_H

struct TP1BisConfig {
    const char* defaultDepthPath = "../data/D/pointCloud_data_2026-02-19_12-03-32_depth.tiff";
    int defaultThreshold = 340;
    int cropX = 340;
    int cropY = 100;
    int cropW = 1240;
    int cropH = 900;
};

inline constexpr TP1BisConfig kStudentConfig{};

#endif // STUDENT_CONFIG_H
