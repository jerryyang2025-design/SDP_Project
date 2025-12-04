#include "header_files/data.h"
#include "header_files/utils.h"

#define GRAVITATIONAL_CONSTANT 100
#define COLLISION_TEST_BOUNDS 500
#define CHECKED_DEPTH -10
#define DRAG 0.9
#define pi 3.141592653589793238462643383 // too lazy to include a library

void applyGravity(Container& container, int time) { // time between frames in miliseconds
    container.states.playerStates.persistentVelocity[1] += -GRAVITATIONAL_CONSTANT * time / 1000.0f;
}

void applyDrag(Container& container) {
    container.states.playerStates.persistentVelocity[0] *= DRAG;
    container.states.playerStates.persistentVelocity[2] *= DRAG;
}

void collisionCorrection(Container& container) {
    // uhh do stuff here
}

void polygonCollision(Container& container, const struct Object& object, int type, int polygon) {
    if (distance(object.center[0] - container.objects.cameraPosition[0],object.center[1] - container.objects.cameraPosition[1],object.center[2] - container.objects.cameraPosition[2]) < COLLISION_TEST_BOUNDS) {
        for (int i = 0; i < container.objects.playerHitbox.size(); i++) {
            std::array<float,3> testVertex = container.objects.playerHitbox[i];

            std::array<std::array<float,3>,3> hitbox = {object.vertices[object.hitbox[polygon][0]],object.vertices[object.hitbox[polygon][1]],object.vertices[object.hitbox[polygon][2]]};
            std::array<float,3> center,vectorOne,vectorTwo,sideOne,sideTwo,sideThree,normalVector,normalVectorOne,normalVectorTwo,normalVectorThree,toPointVector,toPointVectorOne,toPointVectorTwo,toPointVectorThree;

            for (int j = 0; j < 3; j++) { // maybe precompute and store polygon vectors if noticably slow
                center[j] = (hitbox[0][j] + hitbox[1][j] + hitbox[2][j]) / 3;
                vectorOne[j] = hitbox[1][j] - hitbox[0][j];
                vectorTwo[j] = hitbox[2][j] - hitbox[0][j];
                sideOne[j] = hitbox[1][j] - hitbox[0][j];
                sideTwo[j] = hitbox[2][j] - hitbox[1][j];
                sideThree[j] = hitbox[0][j] - hitbox[2][j];
                toPointVector[j] = testVertex[j] - center[j];
                toPointVectorOne[j] = testVertex[j] - hitbox[0][j];
                toPointVectorTwo[j] = testVertex[j] - hitbox[1][j];
                toPointVectorThree[j] = testVertex[j] - hitbox[2][j];
            }

            normalVector = crossProduct(vectorOne,vectorTwo);

            float penetrationDepth = magnitudeInDirection(normalVector,toPointVector);
            if (penetrationDepth < 0 && penetrationDepth > CHECKED_DEPTH) {
                normalVectorOne = crossProduct(normalVector,sideOne);
                normalVectorTwo = crossProduct(normalVector,sideTwo);
                normalVectorThree = crossProduct(normalVector,sideThree);

                if (magnitudeInDirection(normalVectorOne,toPointVectorOne) > 0 && magnitudeInDirection(normalVectorTwo,toPointVectorTwo) > 0 && magnitudeInDirection(normalVectorThree,toPointVectorThree) > 0) {
                    container.states.playerStates.onGround = {1,type};
                    float angleOfPoly = angle(normalVector,container.objects.universalUp) * 180.0f / pi;
                    int slant;
                    if (angleOfPoly <= 15 || angleOfPoly >= 165) {
                        slant = 0;
                    } else if (angleOfPoly >= 75 && angleOfPoly <= 105) {
                        slant = 2;
                    } else {
                        slant = 1;
                    }
                    if (slant == 0) {
                        // move vertically
                    } else if (slant == 1) {
                        // move in normal direction
                    } else if (slant == 2) {
                        // move sideways, use x z of normal vector
                    }
                    // move player back based on group
                    // adjust velocity based on group
                }
            }
        }
    }
}

// turn onGround off if collision not detected in a frame
// central handle collision function to loop through all hitbox polygons