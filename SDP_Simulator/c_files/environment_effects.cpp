#include <math.h>
#include "FEHRandom.h"
#include "header_files/data.h"
#include "header_files/utils.h"

#define WAVEHEIGHT 40
#define SNOWSPEED 7
#define SNOWHEIGHT 5000.0f
#define ENVIRONMENT_WIDTH 10000

float waveMovement(std::array<float,3> vertex, long frames) {
    float wave1 = sin(frames * 0.04 + (vertex[0] - vertex[2]) / 3.0f);
    return WAVEHEIGHT * wave1 ; // maybe adjust depending on framerate, or change to use time
}

/*
iterates through every wave vertex and adjust the heights using a sin function
purpose is to animate the water with a subtle wave effect
Author: Jerry
*/
void handleWaves(Container& container) {
    for (int i = 0; i < container.objects.water.size(); i++) {
        for (int j = 0; j < container.objects.water[i].vertices.size(); j++) {
            container.objects.water[i].vertices[j][1] = waveMovement(container.objects.water[i].vertices[j], container.states.gameStates.frames);
        }
    }
}

/*
Randomly create snow around the map
purpose is the add atmosphere
Author: Jerry
*/
void createSnow(Container& container) { // adjust accordingly
    int amountOfSnow = clamp((Random.RandInt() / 32767.0f) * 100 + 100,100,200);
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

/*
Iterates through every snow particle vertex and moves them down, sending them back to the top when they reach the bottom
purpose is to animate the snow
Author: Jerry
*/
void handleSnow(Container& container) {
    for (int i = 0; i < container.objects.snow.size(); i++) {
        container.objects.snow[i][1] = snowFalling(container.objects.snow[i]);
    }
}

/*
central function to handle all animations
purpose is to group related functions so that game loop can call them all at once
Author: Jerry
*/
void handleEnvironmentAnimations(Container& container) {
    handleWaves(container);
    handleSnow(container);
}

// scraped for now, might add in the future as a personal project:
// add platform moving function, don't forget to move hitbox too
// have a temporary velocity for the platforms, add it to player temp velocity if collision detected

// a lot of code in the other files are commented out, that just means it was scraped for now, but I still want to keep it