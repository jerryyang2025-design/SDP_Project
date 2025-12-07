#include "FEHLCD.h"
#include "FEHImages.h"
#include "FEHSound.h"
#include "header_files/menu.h"

int main()
{
    freopen("/dev/null","w",stdout);

    Container* container = new Container();
    FEHSound music((*container).files.soundFiles.menuMusic); // make sure you know the index of each sound file
    (*container).files.music.push_back(music);

    drawMenu(*container);
    while (1) {
        float x, y;

        if (LCD.Touch(&x, &y)) {
            while (LCD.Touch(&x, &y)); // makes sure it doesn't spam between menu and other screens
            if (x >= 195 && x <= 305) {
                if (y >= 15 && y <= 60) {
                    buttonFlash(*container, 1);
                    transition(*container, 2);
                    stageSelect(*container);
                    drawMenu(*container);
                }
                if (y >= 65 && y <= 110) {
                    buttonFlash(*container, 2);
                    transition(*container, 3);
                    statsPage(*container);
                    drawMenu(*container);
                }
                if (y >= 115 && y <= 160) {
                    buttonFlash(*container, 3);
                    transition(*container, 4);
                    instructions(*container);
                    drawMenu(*container);
                }
                if (y >= 165 && y <= 210) {
                    buttonFlash(*container, 4);
                    transition(*container, 5);
                    credits(*container);
                    drawMenu(*container);
                }
            }
        }

        LCD.Update();
    }
    return 0;
}