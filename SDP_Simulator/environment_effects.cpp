#include <math.h>
#include "FEHRandom.h"
#include "data.h"
#include "utils.h"

#define WAVEHEIGHT 10
#define SNOWSPEED 10
#define SNOWHEIGHT 300.0f
#define ENVIRONMENT_LENGTH 1000
#define ENVIRONMENT_WIDTH 500

float waveMovement(std::array<float,3> vertex, long frames) {
    float wave1 = sin(frames * 0.25 + vertex[0] + vertex[2]);
    float wave2 = 0.5 * sin(frames * 0.35 + vertex[0] * 0.7 - vertex[2] * 0.5); // maybe adjust wave directions
    return WAVEHEIGHT * (wave1 + wave2) / 1.5; // maybe adjust depending on framerate, or change to use time
}

void handleWaves(Container& container) {
    for (int i = 0; i < container.objects.water.vertices.size(); i++) {
        container.objects.water.vertices[i][1] = waveMovement(container.objects.water.vertices[i], container.states.gameStates.frames);
    }
}

void createSnow(Container& container) {
    int amountOfSnow = clamp(Random.RandInt() / 200 + 150,150,300);
    for (int i = 0; i < amountOfSnow; i++) {
        float height = clamp(Random.RandInt() / 100.0f + 200,200,500);
        float x = clamp(Random.RandInt() / 30.0f - ENVIRONMENT_WIDTH / 2,-ENVIRONMENT_WIDTH / 2,ENVIRONMENT_WIDTH / 2);
        float z = clamp(Random.RandInt() / 60.0f - ENVIRONMENT_LENGTH / 10,-ENVIRONMENT_LENGTH / 10,ENVIRONMENT_LENGTH);
        container.objects.snow.push_back({x,height,z});
    }
}

float snowFalling(std::array<float,3> vertex) {
    if (vertex[1] > -10) {
        return -SNOWSPEED + vertex[1];
    } else {
        return SNOWHEIGHT;
    }
}

void handleSnow(Container& container) {
    for (int i = 0; i < container.objects.snow.size(); i++) {
        container.objects.snow[i][1] = snowFalling(container.objects.snow[i]);
    }
}

void handleEnvironmentAnimations(Container& container) {
    handleWaves(container);
    handleSnow(container);
}

// add platform moving function, don't forget to move hitbox too
// have a temporary velocity for the platforms, add it to player temp velocity if collision detected

// put everything in a handleEnvironment function