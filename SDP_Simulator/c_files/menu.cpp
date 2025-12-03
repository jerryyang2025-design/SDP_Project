#include "FEHLCD.h"
#include "FEHImages.h"
#include "header_files/menu.h"

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
                    // Run game or something
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