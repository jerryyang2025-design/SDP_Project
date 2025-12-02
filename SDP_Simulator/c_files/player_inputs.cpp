#include "FEHKeyboard.h"
#include "header_files/data.h"

#define userSpeed 10.0
#define jumpForce 60.0

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



}