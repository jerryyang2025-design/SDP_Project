#include "FEHKeyboard.h"
#include "FEHLCD.h"
#include "header_files/data.h"
#include "header_files/utils.h"
#include "header_files/physics.h"
#include "header_files/player_inputs.h"
#include <math.h>

#define USERSPEED 10.0 // remember this is per frame, so adjust accordingly
#define JUMPFORCE 60.0
#define SENSITIVITY 0.2
#define CAMERALIMIT 85 // in degrees
#define PI 3.141592653589793238462643383

void playerInputs(Container& container) { // Reads in inputs from keyboard and mouse
    container.states.playerStates.tempVelocity = {};
    // needs to be changed to use camera vector instead of assuming z is forward

    std::array<float,3> tempCamera=container.objects.cameraVector,tempRightCamera=container.objects.cameraRightVector;
    normalize(tempCamera);
    normalize(tempRightCamera);

    if (Keyboard.isPressed(KEY_W)) {
        //container.states.playerStates.tempVelocity[2] = USERSPEED;
        container.states.playerStates.tempVelocity[2] = USERSPEED*tempCamera[2];
        container.states.playerStates.tempVelocity[0] = USERSPEED*tempCamera[0];
    }
    if (Keyboard.isPressed(KEY_A)) {
        container.states.playerStates.tempVelocity[0] = -USERSPEED*tempRightCamera[0];
        container.states.playerStates.tempVelocity[2] = -USERSPEED*tempRightCamera[2];
    }
    if (Keyboard.isPressed(KEY_S)) {
        container.states.playerStates.tempVelocity[2] = -USERSPEED*tempCamera[2];
        container.states.playerStates.tempVelocity[0] = -USERSPEED*tempCamera[0];
    }
    if (Keyboard.isPressed(KEY_D)) {
        container.states.playerStates.tempVelocity[0] = USERSPEED*tempRightCamera[0];
        container.states.playerStates.tempVelocity[2] = USERSPEED*tempRightCamera[2];
    }
    if (Keyboard.isPressed(KEY_SPACE)) {
        if (container.states.playerStates.onGround[0]) {
            container.states.playerStates.persistentVelocity[1] += JUMPFORCE;
        }
    }
    if (Keyboard.isPressed(KEY_ESCAPE)) {
        container.states.gameStates.pause = true;
    }
    float x,y;
    if (LCD.Touch(&x,&y)) {
        container.rotation.previousMouse[0] = container.rotation.currentMouse[0];
        container.rotation.previousMouse[1] = container.rotation.currentMouse[1];
        container.rotation.currentMouse[0] = x; // Sets current mouse position to what LCD.touch detected
        container.rotation.currentMouse[1] = y;

        if (container.rotation.newMousePos == false) { // newMousePos makes sure camera only rotates when dragging
            container.rotation.xzRotation = SENSITIVITY*(x - container.rotation.previousMouse[0]); // uses x displacement to get xz rotation
            container.rotation.yzRotation = -SENSITIVITY*(y - container.rotation.previousMouse[1]); // uses y displacement to get yz rotation
        }
        container.rotation.newMousePos = false;
    }
    else {
        container.rotation.newMousePos = true;
        container.rotation.xzRotation = 0;
        container.rotation.yzRotation = 0;
    }

}

void cameraRotation(Container& container) { // Rotates camera based on angle rotated
    std::array<float,3> cameraSpherical,tempUpVector, upSpherical, rightSpherical;
    float temp;
    
    cameraSpherical = cartesianToSpherical(container.objects.cameraVector); // Converts cameraVector to spherical coords
    rightSpherical = cartesianToSpherical(container.objects.cameraRightVector);
    upSpherical = cartesianToSpherical(container.objects.cameraUpVector);
    /*tempUpVector = container.objects.cameraUpVector;
    
    toCameraSpace(container.objects,tempUpVector,x,y,z);
    tempUpVector[0] = x;
    tempUpVector[1] = y;
    tempUpVector[2] = z;
    
    upSpherical = cartesianToSpherical(tempUpVector);
    */

    cameraSpherical[1] += -container.rotation.xzRotation*PI/180;
    rightSpherical[1] += -container.rotation.xzRotation*PI/180;

    if ((cameraSpherical[2] + container.rotation.yzRotation*PI/180) > CAMERALIMIT*PI/180) {
        cameraSpherical[2] = CAMERALIMIT*PI/180; // Checks if angle will go above limit. If so, set to limit.
        upSpherical[2] = (CAMERALIMIT+90)*PI/180;
    }
    else if ((cameraSpherical[2] + container.rotation.yzRotation*PI/180) < -CAMERALIMIT*PI/180) {
        cameraSpherical[2] = -CAMERALIMIT*PI/180;
        upSpherical[2] = -(CAMERALIMIT+90)*PI/180;
    }
    else {
        cameraSpherical[2] += container.rotation.yzRotation*PI/180;
        upSpherical[2] += container.rotation.yzRotation*PI/180;
    }
    
    container.objects.cameraVector = sphericalToCartesian(cameraSpherical[0],cameraSpherical[1],cameraSpherical[2]);
    container.objects.cameraUpVector = sphericalToCartesian(upSpherical[0],upSpherical[1],upSpherical[2]);
    temp = container.objects.cameraUpVector[0];
    container.objects.cameraUpVector[0] = container.objects.cameraUpVector[2];
    container.objects.cameraUpVector[2] = temp;
    container.objects.cameraRightVector = sphericalToCartesian(rightSpherical[0],rightSpherical[1],rightSpherical[2]);
}

void handleMovement(Container& container) {
    container.states.playerStates.onGround = {};
    std::array<float,3> velocity;
    for (int i = 0; i < 3; i++) {
        velocity[i] = container.states.playerStates.persistentVelocity[i] + container.states.playerStates.tempVelocity[i];
    }
    for (int i = 0; i < 3; i++) {
        velocity[i] /= STEP_AMOUNT;
    }
    for (int i = 0; i < STEP_AMOUNT; i++) {
        movePlayer(container,velocity);
        handleCollision(container,i);
    }
}

void handleInputEffects(Container& container) {
    cameraRotation(container);
    handleMovement(container);
}