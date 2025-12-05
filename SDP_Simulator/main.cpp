#include "FEHLCD.h"
#include "FEHImages.h"
#include "header_files/menu.h"

int main()
{
    Container* container = new Container();

    drawMenu(*container);
    while (1) {
        float x, y;

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y)); // makes sure it doesn't spam between menu and other screens
            if (x >= 195 && x <= 305) {
                if (y >= 15 && y <= 60) {
                    stageSelect(*container);
                    drawMenu(*container);
                }
                if (y >= 65 && y <= 110) {
                    statsPage(*container);
                    drawMenu(*container);
                }
                if (y >= 115 && y <= 160) {
                    instructions(*container);
                    drawMenu(*container);
                }
                if (y >= 165 && y <= 210) {
                    credits(*container);
                    drawMenu(*container);
                }
            }
        }

        LCD.Update();
    }
    return 0;
}