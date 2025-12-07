#include "header_files/data.h"
#include "header_files/sounds.h"
#include "FEHSound.h"
#include "FEHUtility.h"

void pauseMusic(Container& container);

void setVolumes(Container& container) {
    container.files.music[0].setVolume(0.25); // menu music
    container.files.music[1].setVolume(1.0); // cutscene wind
    container.files.music[2].setVolume(0.2); // game music
    container.files.sfx[0].setVolume(0.9); // ice breaking
    container.files.sfx[1].setVolume(1.0); // win sfx
    container.files.sfx[2].setVolume(0.8); // lose sfx
}

void playMenuMusic(Container& container) {
    if (container.states.musicStates.musicPlaying != 0) {
        container.files.music[0].restart();
        container.states.musicStates.musicPlaying = 0;
        container.states.musicStates.startTime = TimeNow();
    }
    if ((TimeNow()-container.states.musicStates.startTime)>=156.233) {
        container.files.music[0].restart();
        container.states.musicStates.startTime = TimeNow();
    }
}

void playCutsceneMusic(Container& container) {
    container.files.music[0].pause();
    container.files.music[1].restart();
    container.states.musicStates.musicPlaying = 1;
}

void playGameMusic(Container& container) {
    if (container.states.musicStates.musicPlaying != 2) {
        pauseMusic(container);
        container.files.music[2].restart();
        container.states.musicStates.musicPlaying = 2;
        container.states.musicStates.startTime = TimeNow();
    }
    if ((TimeNow()-container.states.musicStates.startTime)>=111.455) {
        container.files.music[2].restart();
        container.states.musicStates.startTime = TimeNow();
    }
}


void stopGameMusic(Container& container) {
    container.files.music[2].pause();
    container.states.musicStates.musicPlaying = -1;
}

void pauseMusic(Container& container) {
    for (int i=0;i<2;i++) {
        container.files.music[i].pause();
    }
}