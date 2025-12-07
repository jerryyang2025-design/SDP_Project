#include "header_files/data.h"
#include "header_files/sounds.h"
#include "FEHSound.h"

void playMusic(Container& container) {
    FEHSound menuMusic(container.files.soundFiles.menuMusic);
    menuMusic.play();
}