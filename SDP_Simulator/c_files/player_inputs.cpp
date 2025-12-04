#define _USE_MATH_DEFINES
#include <math.h>
#include "FEHKeyboard.h"
#include "FEHLCD.h"
#include "header_files/data.h"
#include "header_files/utils.h"

#define userSpeed 10.0
#define jumpForce 60.0
#define sensitivity 0.375
#define yCameraLimit 85 // in degrees

void playerInputs(Container& container) { // Reads in inputs from keyboard and mouse
    if (Keyboard.isPressed(KEY_W)) {
        container.states.playerStates.tempVelocity[2] = userSpeed;
    }
    if (Keyboard.isPressed(KEY_A)) {
        container.states.playerStates.tempVelocity[0] = -userSpeed;
    }
    if (Keyboard.isPressed(KEY_S)) {
        container.states.playerStates.tempVelocity[2] = -userSpeed;
    }
    if (Keyboard.isPressed(KEY_D)) {
        container.states.playerStates.tempVelocity[0] = userSpeed;
    }
    if (Keyboard.isPressed(KEY_SPACE)) {
        if (true) {
            container.states.playerStates.persistentVelocity[1] += jumpForce;
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

        if (container.rotation.newMousePos = false) { // newMousePos makes sure camera only rotates when dragging
            container.rotation.xzRotation = sensitivity*(x - container.rotation.previousMouse[0]); // uses x displacement to get xz rotation
            container.rotation.yzRotation = -sensitivity*(y - container.rotation.previousMouse[1]); // uses y displacement to get yz rotation
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
    std::array<float,3> cameraSpherical, upSpherical, rightSpherical;
    cameraSpherical = cartesianToSpherical(container.objects.cameraVector); // Converts cameraVector to spherical coords
    rightSpherical = cartesianToSpherical(container.objects.cameraRightVector);

    cameraSpherical[1] += -container.rotation.xzRotation;
    rightSpherical[1] += -container.rotation.xzRotation;

    if ((cameraSpherical[2] + container.rotation.yzRotation) > yCameraLimit*M_PI/180) {
        cameraSpherical[2] = yCameraLimit*M_PI/180; // Checks if angle will go above limit. If so, set to limit.
    }
    else if ((cameraSpherical[2] + container.rotation.yzRotation) < -yCameraLimit*M_PI/180) {
        cameraSpherical[2] = -yCameraLimit*M_PI/180;
    }
    else {
        cameraSpherical[2] += container.rotation.yzRotation;
    }
    
    container.objects.cameraVector = sphericalToCartesian(cameraSpherical[0],cameraSpherical[1],cameraSpherical[2]);
    container.objects.cameraUpVector[1] = container.objects.cameraVector[1];
    container.objects.cameraRightVector = sphericalToCartesian(rightSpherical[0],rightSpherical[1],rightSpherical[2]);

}