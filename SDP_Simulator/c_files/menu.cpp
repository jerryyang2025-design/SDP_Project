/*
    Draws all menu screens and handles the buttons on screen.
    Includes the runGame function, which handles the game loop and all the functions the game uses.
*/

#include <chrono>
#include <cmath>
#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHImages.h"
#include "FEHSound.h"
#include "header_files/menu.h"
#include "header_files/data.h"
#include "header_files/player_inputs.h"
#include "header_files/environment_effects.h"
#include "header_files/physics.h"
#include "header_files/renderer.h"
#include "header_files/utils.h"
#include "header_files/sounds.h"

void drawMenu(Container& container);
void drawStage(Container& container);
void drawStats(Container& container);
void drawInstructions(Container& container);
void drawCredits(Container& container);
void drawWin(Container& container);

/*Transition that plays when menu screens are changed*/
void transition(Container& container, int screen) {
    // for (int i = 0; i <= SCREEN_Y; i = i + 6) {
    //     LCD.SetFontColor(BLACK);
    //     LCD.FillRectangle(0,0,SCREEN_X,i);
    //     LCD.Update();
    //     Sleep(0.005);
    // }
    container.files.sfx[3].playFrom(0.4);
    for (int i = 0; i <= SCREEN_Y; i = i + 6) { // this is so much better than the black screen
        FEHImage transition;
        transition.Open(container.files.art.transition);
        transition.Draw(0, i - SCREEN_Y);
        transition.Close();
        LCD.Update();
        Sleep(0.005);
    }
    Sleep(0.4);
    if (screen == 1) { // menu
        for (int i = 0; i < SCREEN_Y; i = i + 6) {
            drawMenu(container);
            FEHImage transition;
            transition.Open(container.files.art.transition);
            transition.Draw(0, i);
            transition.Close();
            LCD.Update();
            Sleep(0.005);
        }
    } else if (screen == 2) { // stages
        for (int i = 0; i < SCREEN_Y; i = i + 6) {
            drawStage(container);
            FEHImage transition;
            transition.Open(container.files.art.transition);
            transition.Draw(0, i);
            transition.Close();
            LCD.Update();
            Sleep(0.005);
        }
    } else if (screen == 3) { // stats
        for (int i = 0; i < SCREEN_Y; i = i + 6) {
            drawStats(container);
            FEHImage transition;
            transition.Open(container.files.art.transition);
            transition.Draw(0, i);
            transition.Close();
            LCD.Update();
            Sleep(0.005);
        }
    } else if (screen == 4) { // instructions
        for (int i = 0; i < SCREEN_Y; i = i + 6) {
            drawInstructions(container);
            FEHImage transition;
            transition.Open(container.files.art.transition);
            transition.Draw(0, i);
            transition.Close();
            LCD.Update();
            Sleep(0.005);
        }
    } else if (screen == 5) { // credits
        for (int i = 0; i < SCREEN_Y; i = i + 6) {
            drawCredits(container);
            FEHImage transition;
            transition.Open(container.files.art.transition);
            transition.Draw(0, i);
            transition.Close();
            LCD.Update();
            Sleep(0.005);
        }
    } else if (screen == 6) { // cutscene
        for (int i = 0; i < SCREEN_Y; i = i + 6) {
            FEHImage cutscene1;
            cutscene1.Open(container.files.art.menu);
            cutscene1.Draw(0, 0);
            cutscene1.Close();

            LCD.SetFontColor(BLACK);
            LCD.SetFontScale(0.5);
            LCD.WriteAt("What a lovely day...",40,160);
            FEHImage transition;
            transition.Open(container.files.art.transition);
            transition.Draw(0, i);
            transition.Close();
            LCD.Update();
            Sleep(0.005);
        }
    } else if (screen == 7) { // win
        for (int i = 0; i < SCREEN_Y; i = i + 6) {
            drawWin(container);
            FEHImage transition;
            transition.Open(container.files.art.transition);
            transition.Draw(0, i);
            transition.Close();
            LCD.Update();
            Sleep(0.005);
        }
    }
}

/*Transition that plays when the game starts*/
void circleTransition(Container& container) {
    container.files.sfx[3].playFrom(0.4);
    for (int i = 0; i <= 170; i = i + 2) {
        LCD.SetFontColor(BLACK);
        if (pow(i,1.5) / 10.0f <= pythag(SCREEN_X,SCREEN_Y)) {
            LCD.FillCircle(SCREEN_X / 2,SCREEN_Y / 2,pow(i,1.5) / 10.0f);
        }
        LCD.Update();
    }
    Sleep(1.0);
}

/*Menu button flashes*/
void buttonFlash(Container& container, int button) {
    container.files.sfx[4].play();
    if (button == 1) { // menu 1
        for (int i = 0; i < 6; i++) {
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(195,15,110,45);
            LCD.SetFontColor(DARKGRAY);
            LCD.FillRectangle(200,20,100,35);
            LCD.SetFontColor(ROYALBLUE);
            LCD.SetFontScale(1);
            LCD.WriteAt("Stages",212,28);
            if (i % 2 == 0) {
                LCD.SetFontColor(WHITE);
                LCD.FillRectangle(195,15,110,45);
            }
            LCD.Update();
            Sleep(0.06);
        }
    } else if (button == 2) { // menu 2
        for (int i = 0; i < 6; i++) {
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(195,65,110,45);
            LCD.SetFontColor(DARKGRAY);
            LCD.FillRectangle(200,70,100,35);
            LCD.SetFontColor(ROYALBLUE);
            LCD.SetFontScale(1);
            LCD.WriteAt("Stats",219,78);
            if (i % 2 == 0) {
                LCD.SetFontColor(WHITE);
                LCD.FillRectangle(195,65,110,45);
            }
            LCD.Update();
            Sleep(0.06);
        }
    } else if (button == 3) { // menu 3
        for (int i = 0; i < 6; i++) {
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(195,115,110,45);
            LCD.SetFontColor(DARKGRAY);
            LCD.FillRectangle(200,120,100,35);
            LCD.SetFontColor(ROYALBLUE);
            LCD.SetFontScale(0.5);
            LCD.WriteAt("Instructions",214,131);
            if (i % 2 == 0) {
                LCD.SetFontColor(WHITE);
                LCD.FillRectangle(195,115,110,45);
            }
            LCD.Update();
            Sleep(0.06);
        }
    } else if (button == 4) { // menu 4
        for (int i = 0; i < 6; i++) {
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(195,165,110,45);
            LCD.SetFontColor(DARKGRAY);
            LCD.FillRectangle(200,170,100,35);
            LCD.SetFontColor(ROYALBLUE);
            LCD.SetFontScale(1);
            LCD.WriteAt("Credits",208,178);
            if (i % 2 == 0) {
                LCD.SetFontColor(WHITE);
                LCD.FillRectangle(195,165,110,45);
            }
            LCD.Update();
            Sleep(0.06);
        }
    } else if (button == 5) { // menu return
        for (int i = 0; i < 6; i++) {
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(50,10,220,45);
            LCD.SetFontColor(DARKGRAY);
            LCD.FillRectangle(55,15,210,35);
            LCD.SetFontColor(ROYALBLUE);
            LCD.SetFontScale(1.5);
            LCD.WriteAt("To Menu",100,20);
            if (i % 2 == 0) {
                LCD.SetFontColor(WHITE);
                LCD.FillRectangle(50,10,220,45);
            }
            LCD.Update();
            Sleep(0.06);
        }
    } else if (button == 6) { // stage 1
        for (int i = 0; i < 6; i++) {
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(50,60,220,45);
            LCD.SetFontColor(DARKGRAY);
            LCD.FillRectangle(55,65,210,35);
            LCD.SetFontColor(ROYALBLUE);
            LCD.SetFontScale(1.5);
            LCD.WriteAt("Stage 1",100,70);
            if (i % 2 == 0) {
                LCD.SetFontColor(WHITE);
                LCD.FillRectangle(50,60,220,45);
            }
            LCD.Update();
            Sleep(0.06);
        }
    } else if (button == 7) { // stage 2
        for (int i = 0; i < 6; i++) {
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(50,110,220,45);
            LCD.SetFontColor(DARKGRAY);
            LCD.FillRectangle(55,115,210,35);
            LCD.SetFontColor(ROYALBLUE);
            LCD.SetFontScale(1.5);
            LCD.WriteAt("Stage 2",100,120);
            if (i % 2 == 0) {
                LCD.SetFontColor(WHITE);
                LCD.FillRectangle(50,110,220,45);
            }
            LCD.Update();
            Sleep(0.06);
        }
    } else if (button == 8) { // stage 3
        for (int i = 0; i < 6; i++) {
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(50,160,220,45);
            LCD.SetFontColor(DARKGRAY);
            LCD.FillRectangle(55,165,210,35);
            LCD.SetFontColor(ROYALBLUE);
            LCD.SetFontScale(1.5);
            LCD.WriteAt("Stage 3",100,170);
            if (i % 2 == 0) {
                LCD.SetFontColor(WHITE);
                LCD.FillRectangle(50,160,220,45);
            }
            LCD.Update();
            Sleep(0.06);
        }
    } else if (button == 9) { // lose return
        for (int i = 0; i < 6; i++) { // temp, will be replaced
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(25,100,110,45);
            LCD.SetFontColor(DARKGRAY);
            LCD.FillRectangle(30,105,100,35);
            LCD.SetFontColor(ROYALBLUE);
            LCD.SetFontScale(0.5);
            LCD.WriteAt("Return to Menu",37,116);
            if (i % 2 == 0) {
                LCD.SetFontColor(WHITE);
                LCD.FillRectangle(25,100,110,45);
            }
            LCD.Update();
            Sleep(0.06);
        }
    } else if (button == 10) { // try again
        for (int i = 0; i < 6; i++) {
            LCD.SetFontColor(GRAY);
            LCD.FillRectangle(185,100,110,45);
            LCD.SetFontColor(DARKGRAY);
            LCD.FillRectangle(190,105,100,35);
            LCD.SetFontColor(ROYALBLUE);
            LCD.SetFontScale(0.5);
            LCD.WriteAt("Try Again",212,116);
            if (i % 2 == 0) {
                LCD.SetFontColor(WHITE);
                LCD.FillRectangle(185,100,110,45);
            }
            LCD.Update();
            Sleep(0.06);
        }
    }
    Sleep(0.1);
}

void drawMenu(Container& container) {
    FEHImage menu;
    menu.Open(container.files.art.menu);
    menu.Draw(0, 0);
    menu.Close();

    LCD.SetFontColor(WHITE);
    LCD.SetFontScale(2);
    LCD.WriteAt("Glacier",10,SCREEN_Y * 0.6);
    LCD.WriteAt("Run",50,SCREEN_Y * 0.8);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(195,15,110,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(200,20,100,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1);
    LCD.WriteAt("Stages",212,28);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(195,65,110,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(200,70,100,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1);
    LCD.WriteAt("Stats",219,78);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(195,115,110,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(200,120,100,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(0.5);
    LCD.WriteAt("Instructions",214,131);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(195,165,110,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(200,170,100,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1);
    LCD.WriteAt("Credits",208,178);
}

void drawStage(Container& container) {
    FEHImage menu;
    menu.Open(container.files.art.menu);
    menu.Draw(0, 0);
    menu.Close();

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(50,10,220,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(55,15,210,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1.5);
    LCD.WriteAt("To Menu",100,20);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(50,60,220,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(55,65,210,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1.5);
    LCD.WriteAt("Stage 1",100,70);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(50,110,220,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(55,115,210,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1.5);
    LCD.WriteAt("Stage 2",100,120);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(50,160,220,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(55,165,210,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1.5);
    LCD.WriteAt("Stage 3",100,170);
}

void stageSelect(Container& container) {
    drawStage(container);

    while (1) {
        float x, y;

        playMenuMusic(container);

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y));
            if (x >= 50 && x <= 270) {
                if (y >= 10 && y <= 55) {
                    buttonFlash(container, 5);
                    transition(container, 1);
                    return;
                }
                if (y >= 60 && y <= 105) {
                    buttonFlash(container, 6);
                    container.files.loadStage(container,1);
                    container.states.currentStage = 1;
                    runGame(container);
                    drawStage(container);
                } else if (y >= 110 && y <= 155) {
                    buttonFlash(container, 7);
                    container.files.loadStage(container,2);
                    container.states.currentStage = 2;
                    runGame(container);
                    drawStage(container);
                }
                 else if (y >= 160 && y <= 205) {
                    buttonFlash(container, 8);
                    container.files.loadStage(container,3);
                    container.states.currentStage = 3;
                    runGame(container);
                    drawStage(container);
                }
            }
        }

        LCD.Update();
    }
}

void drawStats(Container& container) {
    FEHImage menu;
    menu.Open(container.files.art.menu);
    menu.Draw(0, 0);
    menu.Close();

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(50,10,220,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(55,15,210,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1.5);
    LCD.WriteAt("To Menu",100,20);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(50,60,220,145);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(55,65,210,135);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1);
    LCD.WriteAt("Points",125,70);
    LCD.WriteAt("Stage 1:",60,100);
    LCD.WriteAt(container.states.stagePoints[0],170,100);
    LCD.WriteAt("Stage 2:",60,130);
    LCD.WriteAt(container.states.stagePoints[1],170,130);
    LCD.WriteAt("Stage 3:",60,160);
    LCD.WriteAt(container.states.stagePoints[2],170,160);

}

void statsPage(Container& container) {
    drawStats(container);

    while (1) {
        float x, y;

        playMenuMusic(container);

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y));
            if (x >= 50 && x <= 270) {
                if (y >= 10 && y <= 55) {
                    buttonFlash(container, 5);
                    transition(container, 1);
                    return;
                }
            }
        }

        LCD.Update();
    }
}

void drawInstructions(Container& container) {
    FEHImage menu;
    menu.Open(container.files.art.menu);
    menu.Draw(0, 0);
    menu.Close();

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(50,10,220,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(55,15,210,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1.5);
    LCD.WriteAt("To Menu",100,20);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(50,60,220,145);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(55,65,210,135);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1);
    LCD.WriteAt("Instructions",90,70);

    LCD.SetFontScale(0.5);
    LCD.WriteAt("Use WASD to move",62,90);
    LCD.WriteAt("Press spacebar to jump",62,110);
    //LCD.WriteAt("Click and drag to rotate camera",62,130);
    LCD.WriteAt("Move mouse to rotate camera",62,130);
    LCD.WriteAt("Press esc to pause",62,150);
    LCD.WriteAt("Jump to avoid water and reach the",62,170);
    LCD.WriteAt("flag at the end to win!",62,180);
}

void instructions(Container& container) {
    drawInstructions(container);

    while (1) {
        float x, y;

        playMenuMusic(container);

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y));
            if (x >= 50 && x <= 270) {
                if (y >= 10 && y <= 55) {
                    buttonFlash(container, 5);
                    transition(container, 1);
                    return;
                }
            }
        }

        LCD.Update();
    }
}

void drawCredits(Container& container) {
    FEHImage menu;
    menu.Open(container.files.art.menu);
    menu.Draw(0, 0);
    menu.Close();

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(50,10,220,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(55,15,210,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1.5);
    LCD.WriteAt("To Menu",100,20);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(50,60,220,145);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(55,65,210,135);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1);
    LCD.WriteAt("Credits",118,70);

    LCD.SetFontScale(0.5);
    LCD.WriteAt("Game developed by Jerry Yang",62,95);
    LCD.WriteAt("and Nigel Woodall",62,105);
    LCD.WriteAt("Menu art by Jerry Yang",62,125);
    LCD.WriteAt("Menu music: Winter Solace by",62,145);
    LCD.WriteAt("D-Chain, www.newgrounds.com",62,155);
    LCD.WriteAt("Game Music: Polargeist by Step,",62,175);
    LCD.WriteAt("www.newgrounds.com",62,185);
}

void credits(Container& container) {
    drawCredits(container);

    while (1) {
        float x, y;

        playMenuMusic(container);

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y));
            if (x >= 50 && x <= 270) {
                if (y >= 10 && y <= 55) {
                    buttonFlash(container, 5);
                    transition(container, 1);
                    return;
                }
            }
        }

        LCD.Update();
    }
}

void playCutscene(Container& container) { // probably add music and sound effects throughout
    FEHImage cutscene1;
    cutscene1.Open(container.files.art.menu);
    cutscene1.Draw(0, 0);
    cutscene1.Close();

    LCD.SetFontColor(BLACK);
    LCD.SetFontScale(0.5);
    LCD.WriteAt("What a lovely day...",40,160);
    LCD.Update();
    playCutsceneMusic(container);
    Sleep(4.0);

    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();
    LCD.Update();
    Sleep(0.2);

    FEHImage cutscene2;
    cutscene2.Open(container.files.art.cutscene);
    cutscene2.Draw(0, 0);
    cutscene2.Close();

    LCD.SetFontColor(BLACK);
    LCD.SetFontScale(0.5);
    LCD.WriteAt("Oh no! The ice is melting!",40,160);
    LCD.WriteAt("I need to get out of here.",40,190);
    LCD.Update();
    container.files.sfx[0].play();
    Sleep(5.0);
}

void drawWin(Container& container) {
    FEHImage win;
    win.Open(container.files.art.win);
    win.Draw(0, 0);
    win.Close();

    LCD.SetFontColor(MIDNIGHTBLUE);
    LCD.SetFontScale(1.5);
    LCD.WriteAt("Stage Beat!",60,10);
    LCD.SetFontScale(1);
    LCD.WriteAt("Score:",127,55);
    LCD.SetFontScale(1);
    LCD.WriteAt(container.states.stagePoints[container.states.currentStage - 1],135,85);
    LCD.SetFontScale(1);
    LCD.WriteAt("Returning to Menu",60,200);
}

void drawLose(Container& container) {
    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();

    container.files.sfx[2].play();

    for (float i = 0; i <= 1; i = i + 0.05) {
        LCD.SetBackgroundColor(LCD.ScaleColor(GRAY,i / 4.0f));
        LCD.Clear();

        LCD.SetFontColor(LCD.ScaleColor(MAROON,i));
        LCD.SetFontScale(1.5);
        LCD.WriteAt("Stage Lost!",60,30);

        Sleep(0.08);
        LCD.Update();
    }
    Sleep(0.5);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(25,100,110,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(30,105,100,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(0.5);
    LCD.WriteAt("Return to Menu",37,116);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(185,100,110,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(190,105,100,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(0.5);
    LCD.WriteAt("Try Again",212,116);
}

void loseScreen(Container& container) {
    drawLose(container);

    float x, y;
    while (true) {
        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y));
            if (y >= 100 && y <= 145) {
                if (x >= 25 && x <= 135) {
                    buttonFlash(container, 9);
                    transition(container, 2);
                    container.states.gameStates.pause = true;
                    return;
                }
                if (x >= 185 && x <= 295) {
                    buttonFlash(container, 10);
                    circleTransition(container);
                    container.files.loadStage(container,container.states.currentStage);
                    return;
                }
            }
        }
    }
}

/*Checks where the player is standing. If end is reached, stop the stage and do win screen, if water is touched, stop and do lose screen.
Author: Nigel*/
void checkGameState(Container& container) {
    if (container.states.playerStates.onGround[0]) {
        if (container.states.playerStates.onGround[1] == 3) {
            float x = container.states.gameStates.totalTime / 15.0f;
            container.states.stagePoints[container.states.currentStage - 1] = clamp(-(x + 1000) / 2 + 6000 + 10000000000 / (2222 * (x + 1000)),1000,10000);
            stopGameMusic(container);
            transition(container, 7);
            container.files.sfx[1].play();
            drawWin(container);
            LCD.Update();
            container.states.gameStates.pause = true;
            Sleep(5.0);
            transition(container, 2);
        } else if (container.states.playerStates.onGround[1] == 4) {
            stopGameMusic(container);
            circleTransition(container);
            loseScreen(container);
        }
    }
}

/*
    Runs the game while not paused. Handles all functions that occur while the game loop is running.
    Plays the cutscene if not played already.
*/
void runGame(Container& container) {
    if (!container.states.gameStates.cutscenePlayed) {
        transition(container, 6);
        playCutscene(container);
        container.states.gameStates.cutscenePlayed = true;
    }
    circleTransition(container);
    while (!container.states.gameStates.pause) {
        auto start = std::chrono::steady_clock::now();

        playGameMusic(container);
        handleEnvironmentAnimations(container);
        playerInputs(container);
        handlePhysics(container);
        handleInputEffects(container);
        render(container);
        checkGameState(container);

        auto end = std::chrono::steady_clock::now();
        container.states.gameStates.timeBetweenFrames = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
        manageFPS(container, container.states.gameStates.timeBetweenFrames);
        container.states.gameStates.frames++;
    }
    stopGameMusic(container);
    return;
}