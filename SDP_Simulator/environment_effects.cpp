#include <math.h>
#include "data.h"
#include "utils.h"

#define WAVEHEIGHT 10
#define SNOWSPEED 10

float waveMovement(std::array<float,3> vertex, long frames) {
    float wave1 = sin(frames * 0.25 + vertex[0] + vertex[2]);
    float wave2 = 0.5 * sin(frames * 0.35 + vertex[0] * 0.7 - vertex[2] * 0.5); // maybe adjust wave directions
    return WAVEHEIGHT * (wave1 + wave2) / 1.5; // maybe adjust depending on framerate, or change to use time
}

void handleWaves(Container container) {
    for (int i = 0; i < container.objects.water.vertices.size(); i++) {
        container.objects.water.vertices[i][1] = waveMovement(container.objects.water.vertices[i], container.states.gameStates.frames);
    }
}

float snowFalling(std::array<float,3> vertex) {
    return -SNOWSPEED + vertex[1];
}

void handleSnow(Container container) {
    for (int i = 0; i < container.objects.snow.size(); i++) {
        container.objects.snow[i][1] = snowFalling(container.objects.snow[i]);
    }
}

// add platform moving function, don't forget to move hitbox too
// have a temporary velocity for the platforms, add it to player temp velocity if collision detected

// put everything in a handleEnvironment function