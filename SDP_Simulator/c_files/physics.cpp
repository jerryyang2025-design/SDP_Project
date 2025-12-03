#include "header_files/data.h"
#include "header_files/utils.h"

#define GRAVITATIONAL_CONSTANT 100
#define COLLISION_TEST_BOUNDS 500
#define CHECKED_DEPTH -10
#define pi 3.141592653589793238462643383

void applyGravity(Container& container, int time) { // time between frames in miliseconds
    container.states.playerStates.persistentVelocity[1] += -GRAVITATIONAL_CONSTANT * time / 1000.0f;
}

void polygonCollision(Container& container, const struct Object& object, int type, int polygon) {
    if (distance(object.center[0] - container.objects.cameraPosition[0],object.center[1] - container.objects.cameraPosition[1],object.center[2] - container.objects.cameraPosition[2]) < COLLISION_TEST_BOUNDS) {
        for (int i = 0; i < container.objects.playerHitbox.size(); i++) {
            std::array<float,3> testVertex = container.objects.playerHitbox[i];

            std::array<std::array<float,3>,3> hitbox = {object.vertices[object.hitbox[polygon][0]],object.vertices[object.hitbox[polygon][1]],object.vertices[object.hitbox[polygon][2]]};
            std::array<float,3> center,vectorOne,vectorTwo,sideOne,sideTwo,sideThree,normalVector,normalVectorOne,normalVectorTwo,normalVectorThree,toPointVector;

            for (int i = 0; i < 3; i++) { // maybe precompute and store polygon vectors if noticably slow
                center[i] = (hitbox[0][i] + hitbox[1][i] + hitbox[2][i]) / 3;
                vectorOne[i] = hitbox[1][i] - hitbox[0][i];
                vectorTwo[i] = hitbox[2][i] - hitbox[0][i];
                sideOne[i] = hitbox[1][i] - hitbox[0][i];
                sideTwo[i] = hitbox[2][i] - hitbox[1][i];
                sideThree[i] = hitbox[0][i] - hitbox[2][i];
                toPointVector[i] = testVertex[i] - center[i];
            }

            normalVector = crossProduct(vectorOne,vectorTwo);

            float penetrationDepth = magnitudeInDirection(normalVector,toPointVector);
            if (penetrationDepth < 0 && penetrationDepth > CHECKED_DEPTH) {
                normalVectorOne = crossProduct(normalVector,sideOne);
                normalVectorTwo = crossProduct(normalVector,sideTwo);
                normalVectorThree = crossProduct(normalVector,sideThree);

                if (magnitudeInDirection(normalVectorOne,toPointVector) > 0 && magnitudeInDirection(normalVectorTwo,toPointVector) > 0 && magnitudeInDirection(normalVectorThree,toPointVector) > 0) {
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

                    } else if (slant == 1) {

                    } else if (slant == 2) {
                        
                    }
                    // move player back based on group
                    // adjust velocity based on group
                }
            }
        }
    }
}

// turn onGround off if collision not detected in a frame