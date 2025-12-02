#include "FEHKeyboard.h"
#include "FEHLCD.h"
#include "header_files/data.h"

#define userSpeed 10.0
#define jumpForce 60.0
#define sensitivity 0.375

void playerInputs(Container& container) {
    if (Keyboard.isPressed(KEY_W)) {
        container.states.playerStates.tempVelocity[2] = userSpeed;
    }

    if (Keyboard.isPressed(KEY_A)) {
        container.states.playerStates.tempVelocity[0] = -userSpeed;
    }
    
    if (Keyboard.isPressed(KEY_S)) {
        container.states.playerStates.tempVelocity[0] = userSpeed;
    }

    if (Keyboard.isPressed(KEY_D)) {
        container.states.playerStates.tempVelocity[2] = -userSpeed;
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

        container.rotation.xzRotation = sensitivity*(x - container.rotation.previousMouse[0]); // uses x displacement to get xz rotation
        container.rotation.yzRotation = sensitivity*(y - container.rotation.previousMouse[1]); // uses y displacement to get yz rotation
        container.rotation.total_yzRotation += container.rotation.yzRotation;
    }

}