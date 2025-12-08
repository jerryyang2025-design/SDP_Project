/*
    Contains the functions that handle the movement and camera rotation based on the player's inputs.
*/

#include "FEHKeyboard.h"
#include "FEHLCD.h"
#include "header_files/data.h"
#include "header_files/utils.h"
#include "header_files/physics.h"
#include "header_files/player_inputs.h"
#include <math.h>

#define USERSPEED 10.0 // remember this is per frame, so adjust accordingly
#define JUMPFORCE 60.0
#define SENSITIVITY 0.01
#define PI 3.141592653589793238462643383

/*Reads player inputs from the keyboard and mouse. Changes velocity vectors based on where the camera is pointed.
Also finds mouse displacement and returns a camera rotation angle. Author: Nigel*/
void playerInputs(Container& container) { // Reads in inputs from keyboard and mouse
    container.states.playerStates.tempVelocity = {};

    std::array<float,3> tempCamera=container.objects.cameraVector,tempRightCamera=container.objects.cameraRightVector;
    tempCamera[1] = 0;
    normalize(tempCamera);
    normalize(tempRightCamera);

    float fpsAdjuster = FPS / clamp(container.states.gameStates.fps,1,30);

    if (Keyboard.isPressed(KEY_W)) {
        //container.states.playerStates.tempVelocity[2] = USERSPEED;
        container.states.playerStates.tempVelocity[2] = fpsAdjuster*USERSPEED*tempCamera[2];
        container.states.playerStates.tempVelocity[0] = fpsAdjuster*USERSPEED*tempCamera[0];
    }
    if (Keyboard.isPressed(KEY_A)) {
        container.states.playerStates.tempVelocity[0] = -fpsAdjuster*USERSPEED*tempRightCamera[0];
        container.states.playerStates.tempVelocity[2] = -fpsAdjuster*USERSPEED*tempRightCamera[2];
    }
    if (Keyboard.isPressed(KEY_S)) {
        container.states.playerStates.tempVelocity[2] = -fpsAdjuster*USERSPEED*tempCamera[2];
        container.states.playerStates.tempVelocity[0] = -fpsAdjuster*USERSPEED*tempCamera[0];
    }
    if (Keyboard.isPressed(KEY_D)) {
        container.states.playerStates.tempVelocity[0] = fpsAdjuster*USERSPEED*tempRightCamera[0];
        container.states.playerStates.tempVelocity[2] = fpsAdjuster*USERSPEED*tempRightCamera[2];
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

        if (!container.rotation.newMousePos) { // newMousePos makes sure camera only rotates when dragging
            container.rotation.xzRotation = SENSITIVITY*(x - container.rotation.previousMouse[0]); // uses x displacement to get xz rotation
            container.rotation.yzRotation = -SENSITIVITY*(y - container.rotation.previousMouse[1]); // uses y displacement to get yz rotation
        }
        container.rotation.newMousePos = false;
    } else {
        container.rotation.newMousePos = true;
        container.rotation.xzRotation = 0;
        container.rotation.yzRotation = 0;
    }

}

/*Takes the camera rotation from playerInputs and adds it to the camera vectors. Author: Nigel*/
void cameraRotation(Container& container) { // Rotates camera based on angle rotated
    if (container.rotation.xzRotation != 0 || container.rotation.yzRotation != 0) {
        std::array<float,3> tempCamera = cartesianToSpherical(container.objects.cameraVector);
        std::array<float,3> tempRightCamera = cartesianToSpherical(container.objects.cameraRightVector);

        tempCamera[1] += container.rotation.xzRotation;
        tempRightCamera[1] += container.rotation.xzRotation;

        tempCamera[2] = clamp(tempCamera[2] + container.rotation.yzRotation,-85 * PI/180,85 * PI/180);

        container.objects.cameraVector = sphericalToCartesian(tempCamera);
        container.objects.cameraRightVector = sphericalToCartesian(tempRightCamera);
        normalize(container.objects.cameraVector);
        normalize(container.objects.cameraRightVector);

        container.objects.cameraUpVector = crossProduct(container.objects.cameraVector,container.objects.cameraRightVector);
        normalize(container.objects.cameraUpVector);
    }
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