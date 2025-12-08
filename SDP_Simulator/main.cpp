#include "FEHLCD.h"
#include "FEHImages.h"
#include "FEHSound.h"
#include "header_files/menu.h"
#include "header_files/sounds.h"

/*
Main loop.
Creates all the sound objects and pushes them to the data container, then runs the menu screen loop.
*/
int main()
{
    freopen("/dev/null","w",stdout);

    Container* container = new Container();
    FEHSound menuMusic((*container).files.soundFiles.menuMusic);
    (*container).files.music.push_back(menuMusic);
    FEHSound cutsceneMusic((*container).files.soundFiles.cutsceneMusic);
    (*container).files.music.push_back(cutsceneMusic);
    FEHSound gameMusic((*container).files.soundFiles.gameMusic);
    (*container).files.music.push_back(gameMusic);
    FEHSound iceMelting((*container).files.soundFiles.iceMelting);
    (*container).files.sfx.push_back(iceMelting);
    FEHSound win((*container).files.soundFiles.win);
    (*container).files.sfx.push_back(win);
    FEHSound lose((*container).files.soundFiles.lose);
    (*container).files.sfx.push_back(lose);
    FEHSound whoosh((*container).files.soundFiles.transition);
    (*container).files.sfx.push_back(whoosh);
    FEHSound click((*container).files.soundFiles.click);
    (*container).files.sfx.push_back(click);

    setVolumes(*container);
    drawMenu(*container);
    while (1) {
        float x, y;

        playMenuMusic(*container);

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