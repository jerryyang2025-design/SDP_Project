#include <math.h>
#include "data.h"
#include "utils.h"

#define WAVEHEIGHT 1

float waveMovement(std::array<float,3> vertex, long frames) {
    float wave1 = sin(frames * 0.25 + vertex[0] + vertex[2]);
    float wave2 = 0.5 * sin(frames * 0.35 + vertex[0] * 0.7 - vertex[2] * 0.5); // maybe adjust wave directions
    return WAVEHEIGHT * (wave1 + wave2) / 1.5; // maybe adjust depending on framerate
}

// add platform moving function, don't forget to move hitbox too