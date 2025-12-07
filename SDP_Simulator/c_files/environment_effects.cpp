#include <math.h>
#include "FEHRandom.h"
#include "header_files/data.h"
#include "header_files/utils.h"

#define WAVEHEIGHT 40
#define SNOWSPEED 7
#define SNOWHEIGHT 5000.0f
#define ENVIRONMENT_WIDTH 10000

float waveMovement(std::array<float,3> vertex, long frames) {
    float wave1 = sin(frames * 0.02 + (vertex[0] - vertex[2]) * 5.0f);
    return WAVEHEIGHT * wave1 ; // maybe adjust depending on framerate, or change to use time
}

void handleWaves(Container& container) {
    for (int i = 0; i < container.objects.water.vertices.size(); i++) {
        container.objects.water.vertices[i][1] = waveMovement(container.objects.water.vertices[i], container.states.gameStates.frames);
    }
}

void createSnow(Container& container) { // adjust accordingly
    int amountOfSnow = clamp((Random.RandInt() / 32767.0f) * 200 + 200,200,400);
    for (int i = 0; i < amountOfSnow; i++) {
        float height = clamp((Random.RandInt() / 32767.0f) * SNOWHEIGHT + 200,200,5200);
        float x = clamp((Random.RandInt() / 32767.0f) * ENVIRONMENT_WIDTH - ENVIRONMENT_WIDTH / 2,-ENVIRONMENT_WIDTH / 2,ENVIRONMENT_WIDTH / 2);
        float z = clamp((Random.RandInt() / 32767.0f) * ENVIRONMENT_WIDTH - ENVIRONMENT_WIDTH / 2,-ENVIRONMENT_WIDTH / 2,ENVIRONMENT_WIDTH / 2);
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

// scraped for now, might add in the future as a personal project:
// add platform moving function, don't forget to move hitbox too
// have a temporary velocity for the platforms, add it to player temp velocity if collision detected