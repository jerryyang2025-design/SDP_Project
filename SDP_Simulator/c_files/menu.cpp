#include "FEHLCD.h"
#include "FEHImages.h"
#include "header_files/menu.h"

void drawMenu(Container& container) {
    FEHImage menu;
    menu.Open(container.files.art.menu);
    menu.Draw(0, 0);

    LCD.SetFontColor(WHITE);
    LCD.SetFontScale(2);
    LCD.WriteAt("Glacier",10,SCREEN_Y * 0.6);
    LCD.WriteAt("Run",50,SCREEN_Y * 0.8);

    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(195,20,110,45);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(200,25,100,35);
    LCD.SetFontColor(ROYALBLUE);
    LCD.SetFontScale(1);
    LCD.WriteAt("Stages",212,33);

    /*
    Add more buttons here
    */
}

void drawStage(Container& container) {
    FEHImage menu;
    menu.Open(container.files.art.menu);
    menu.Draw(0, 0);

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