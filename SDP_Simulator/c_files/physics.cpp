#include <cmath>
#include "header_files/data.h"
#include "header_files/utils.h"
#include "header_files/player_inputs.h"

#define GRAVITATIONAL_CONSTANT 100
#define COLLISION_TEST_BOUNDS 500
#define CHECKED_DEPTH -30
#define DRAG 0.95
#define WATER_HEIGHT 0
#define pi 3.141592653589793238462643383 // too lazy to include a library

void applyGravity(Container& container, int time) { // time between frames in miliseconds
    container.states.playerStates.persistentVelocity[1] += -GRAVITATIONAL_CONSTANT * clamp(time,0,100) / 1000.0f;
}

void applyDrag(Container& container) {
    container.states.playerStates.persistentVelocity[0] *= DRAG;
    container.states.playerStates.persistentVelocity[2] *= DRAG;
}

/*
applies gravity and drag forces to the player's persistent velocity
purpose is the simulate physical forces
Author: Jerry
*/
void handlePhysics(Container& container) {
    applyDrag(container);
    applyGravity(container,container.states.gameStates.timeBetweenFrames);
}

/*
moves the player and hitbox using a movement vector
purpose is to simplify moving the camera and hitbox to a single function call
Author: Jerry
*/
void movePlayer(Container& container, std::array<float,3> movement) {
    for (int i = 0; i < 3; i++) {
        container.objects.cameraPosition[i] += movement[i];
    }
    for (int i = 0; i < container.objects.playerHitbox.size(); i++) {
        for (int j = 0; j < 3; j++) {
            container.objects.playerHitbox[i][j] += movement[j];
        }
    }
}

/*
resets the vertical component of persistent velocity and move the player up when collision is detected
purpose is to prevent the player from falling through the platforms
Author: Jerry
*/
void collisionCorrection(Container& container, float depth, int slant, std::array<float,3> normal, int step) {
    std::array<float,3> reverseNormal;
    for (int i = 0; i < 3; i++) {
        reverseNormal[i] = -normal[i];
    }
    if (slant == 0) {
        container.states.playerStates.persistentVelocity[1]  = 0;

        normalize(normal);
        for (int i = 0; i < 3; i++) {
            normal[i] *= depth;
        }
        float normalAngle = angle(normal,container.objects.universalUp);
        depth = 0; // depth * sin(normalAngle);
        float vertical = magnitudeInDirection(container.objects.universalUp,normal) + depth;
        std::array<float,3> move = {0,vertical,0};
        movePlayer(container,move);
    } // else if (slant == 1) {
    //     float magnitude;
    //     if ((STEP_AMOUNT - step) > 0) {
    //         magnitude = magnitudeInDirection(reverseNormal,container.states.playerStates.persistentVelocity) / (STEP_AMOUNT - step);
    //     } else {
    //         magnitude = magnitudeInDirection(reverseNormal,container.states.playerStates.persistentVelocity);
    //     }
    //     normalize(normal); // how did it take me until this long to add a normalize function instead of doing it manually or in other functions
    //     for (int i = 0; i < 3; i++) {
    //         normal[i] *= magnitude;
    //         container.states.playerStates.persistentVelocity[i] += normal[i];
    //     }
    //
    //     normalize(normal);
    //     for (int i = 0; i < 3; i++) {
    //         normal[i] *= depth;
    //     }
    //     movePlayer(container,normal);
    // } else if (slant == 2) {
    //     float magnitude;
    //     if ((STEP_AMOUNT - step) > 0) {
    //         magnitude = magnitudeInDirection(reverseNormal,container.states.playerStates.persistentVelocity) / (STEP_AMOUNT - step);
    //     } else {
    //         magnitude = magnitudeInDirection(reverseNormal,container.states.playerStates.persistentVelocity);
    //     }
    //     normalize(normal);
    //     for (int i = 0; i < 3; i++) {
    //         normal[i] *= magnitude;
    //     }
    //     magnitude = pythag(normal[0],normal[2]);
    //     normalize(normal);
    //     for (int i = 0; i < 3; i++) {
    //         normal[i] *= magnitude;
    //     }
    //     container.states.playerStates.persistentVelocity[0] += normal[0];
    //     container.states.playerStates.persistentVelocity[2] += normal[2];

    //     normal[1] = 0;
    //     normalize(normal);
    //     for (int i = 0; i < 3; i++) {
    //         normal[i] *= depth;
    //     }
    //     magnitude = pythag(normal[0],normal[2]);
    //     normalize(normal);
    //     std::array<float,3> move = {normal[0] * magnitude,0,normal[2] * magnitude};
    //     movePlayer(container,move);
    // }
}

/*
uses vector projection and same side testing to detect collision with a polygon
purpose is to let the other functions know when the player has collided and with what object they collided with
Author: Jerry
*/
void polygonCollision(Container& container, const struct Object& object, int type, int polygon, int step) {
    if (distance(object.center[0] - container.objects.cameraPosition[0],object.center[1] - container.objects.cameraPosition[1],object.center[2] - container.objects.cameraPosition[2]) < COLLISION_TEST_BOUNDS) {
        std::array<float,3> center,vectorOne,vectorTwo,sideOne,sideTwo,sideThree,normalVector;
        std::array<std::array<float,3>,3> hitbox = {object.vertices[object.hitbox[polygon][0]],object.vertices[object.hitbox[polygon][1]],object.vertices[object.hitbox[polygon][2]]};
        for (int j = 0; j < 3; j++) {
            center[j] = (hitbox[0][j] + hitbox[1][j] + hitbox[2][j]) / 3;
            vectorOne[j] = hitbox[1][j] - hitbox[0][j];
            vectorTwo[j] = hitbox[2][j] - hitbox[0][j];
            sideOne[j] = hitbox[1][j] - hitbox[0][j];
            sideTwo[j] = hitbox[2][j] - hitbox[1][j];
            sideThree[j] = hitbox[0][j] - hitbox[2][j];
        }
        normalVector = crossProduct(vectorOne,vectorTwo);
        for (int i = 0; i < container.objects.playerHitbox.size(); i++) {
            std::array<float,3> testVertex = container.objects.playerHitbox[i];
            std::array<float,3> normalVectorOne,normalVectorTwo,normalVectorThree,toPointVector,toPointVectorOne,toPointVectorTwo,toPointVectorThree;

            for (int j = 0; j < 3; j++) {
                toPointVector[j] = testVertex[j] - center[j];
                toPointVectorOne[j] = testVertex[j] - hitbox[0][j];
                toPointVectorTwo[j] = testVertex[j] - hitbox[1][j];
                toPointVectorThree[j] = testVertex[j] - hitbox[2][j];
            }

            float penetrationDepth = magnitudeInDirection(normalVector,toPointVector);
            if (penetrationDepth < 0 && penetrationDepth > CHECKED_DEPTH) {
                normalVectorOne = crossProduct(normalVector,sideOne);
                normalVectorTwo = crossProduct(normalVector,sideTwo);
                normalVectorThree = crossProduct(normalVector,sideThree);

                if (magnitudeInDirection(normalVectorOne,toPointVectorOne) > 0 && magnitudeInDirection(normalVectorTwo,toPointVectorTwo) > 0 && magnitudeInDirection(normalVectorThree,toPointVectorThree) > 0) {
                    container.states.playerStates.onGround = {1,type};
                    float angleOfPoly = angle(normalVector,container.objects.universalUp) * 180.0f / pi;
                    int slant = 0;
                    // if (angleOfPoly <= 15 || angleOfPoly >= 165) {
                    //     slant = 0;
                    // } else if (angleOfPoly >= 75 && angleOfPoly <= 105) {
                    //     slant = 2;
                    // } else {
                    //     slant = 1;
                    // }
                    collisionCorrection(container, fabs(penetrationDepth), slant, normalVector, step);
                }
            }
        }
    }
}

/*
iterates through every object and applies collision testing
purpose is the group all collision tests into a single function
Author: Jerry
*/
void handleCollision(Container& container, int step) {
    for (int i = 0; i < container.objects.platforms.size(); i++) {
        for (int j = 0; j < container.objects.platforms[i].hitbox.size(); j++) {
            polygonCollision(container,container.objects.platforms[i],1,j,step);
        }
    }
    // for (int i = 0; i < container.objects.movingPlatforms.size(); i++) {
    //     for (int j = 0; j < container.objects.movingPlatforms[i].hitbox.size(); j++) {
    //         polygonCollision(container,container.objects.movingPlatforms[i],2,j,step);
    //     }
    // }
    for (int j = 0; j < container.objects.end.hitbox.size(); j++) {
        polygonCollision(container,container.objects.end,3,j,step);
    }
    if (container.objects.cameraPosition[1] < WATER_HEIGHT + PLAYER_HEIGHT) {
        container.states.playerStates.onGround = {1,4};
    }
}