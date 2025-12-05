#include <chrono>
#include "FEHLCD.h"
#include "FEHUtility.h"
#include "FEHImages.h"
#include "header_files/menu.h"
#include "header_files/data.h"
#include "header_files/player_inputs.h"
#include "header_files/environment_effects.h"
#include "header_files/physics.h"
#include "header_files/renderer.h"
#include "header_files/utils.h"

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

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y));
            if (x >= 50 && x <= 270) {
                if (y >= 10 && y <= 55) {
                    return;
                }
                if (y >= 60 && y <= 105) {
                    container.files.loadStage(container,1);
                    runGame(container);
                    drawStage(container);
                }
                /*
                Add more options here
                */
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
    LCD.SetFontScale(1.5);
    LCD.WriteAt("Stats",115,70);
}

void statsPage(Container& container) {
    drawStats(container);

    while (1) {
        float x, y;

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y));
            if (x >= 50 && x <= 270) {
                if (y >= 10 && y <= 55) {
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
}

void instructions(Container& container) {
    drawInstructions(container);

    while (1) {
        float x, y;

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y));
            if (x >= 50 && x <= 270) {
                if (y >= 10 && y <= 55) {
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
    LCD.SetFontScale(1.5);
    LCD.WriteAt("Credits",100,70);
}

void credits(Container& container) {
    drawCredits(container);

    while (1) {
        float x, y;

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y));
            if (x >= 50 && x <= 270) {
                if (y >= 10 && y <= 55) {
                    return;
                }
            }
        }

        LCD.Update();
    }
}

void playCutscene(Container& container) { // probably add music and sound effects throughout
    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();
    LCD.Update();
    Sleep(1.0);

    FEHImage cutscene1; // also maybe add text or something so it doesn't look like a random static image even though it is
    cutscene1.Open(container.files.art.menu); // idk maybe we can make the text flash on the screen to add "movement"
    cutscene1.Draw(0, 0);
    cutscene1.Close();
    LCD.Update();
    Sleep(4.0);

    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();
    LCD.Update();
    Sleep(1.0);

    FEHImage cutscene2;
    cutscene2.Open(container.files.art.cutscene);
    cutscene2.Draw(0, 0);
    cutscene2.Close();
    LCD.Update();
    Sleep(3.0);

    LCD.SetBackgroundColor(BLACK);
    LCD.Clear();
    LCD.Update();
    Sleep(1.0);
}

void runGame(Container& container) {
    if (!container.states.gameStates.cutscenePlayed) {
        // playCutscene(container);
        container.states.gameStates.cutscenePlayed = true;
    }
    while (!container.states.gameStates.pause) {
        auto start = std::chrono::steady_clock::now();

        handleEnvironmentAnimations(container);
        playerInputs(container);
        handlePhysics(container);
        handleInputEffects(container);
        // check game states
        render(container);

        auto end = std::chrono::steady_clock::now();
        container.states.gameStates.timeBetweenFrames = static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        );
        manageFPS(container.states.gameStates.timeBetweenFrames);
        container.states.gameStates.frames++;
        // probably more I'm forgetting
    }
    // temporary, will probably keep the return statement but add something before too (pause, win, lose screen)
    return;
}