#pragma once

#include <vector>
#include <array>
#include <limits>
#include "FEHSound.h"

#define SCREEN_X 320
#define SCREEN_Y 240
#define FPS 30
#define PLAYER_HEIGHT 200
#define NUMBEROFFILES 4
#define NUMBEROFSTAGES 3

class Container;

struct Object {
    std::vector<std::array<float,3>> vertices, tempVertices;
    std::vector<std::array<int,3>> faces, faceColors, hitbox, tempFaces, tempFaceColors;
    std::array<float,3> center, color;
    float reflectionValue;
    float refractionValue;
};

struct Objects {
    std::array<std::array<float,3>,9> playerHitbox;
    std::vector<std::array<float,3>> snow;
    std::array<float,3> lightSource = {-10000,10000,-10000}, cameraPosition, cameraVector = {0,0,1}, cameraUpVector = {0,1,0}, cameraRightVector = {1,0,0}, universalUp = {0,1,0}; // play around with the light position
    std::array<int,3> backgroundColor;
    struct Object end, water; // default water height of 0
    std::vector<struct Object> platforms, movingPlatforms;
};

struct line {
    int y, x1 = -1, x2;
    unsigned color; // color converted to hexadecimal
};

struct Screen {
    std::vector<std::array<float,4>> vertices = {}; // stored as a float to keep accurate depth value (not z, find projection with cameraVector), and in or out of view
    std::vector<std::array<float,3>> effects;
    std::vector<std::array<int,3>> faces, faceColors;
    std::array<std::array<float,SCREEN_X>,SCREEN_Y> depths; // used for z buffer
    std::array<std::array<std::array<int,3>,SCREEN_X>,SCREEN_Y> currentPixels, previousPixels = {}; // previous to draw only new changes
    std::vector<struct line> lines;

    Screen() {
        std::array<float, SCREEN_X> tempRow;
        tempRow.fill(std::numeric_limits<float>::infinity());

        for (int i = 0; i < SCREEN_Y; i++) {
            depths[i] = tempRow;
        }
    }
};

struct PlayerStates {
    std::array<float,3> tempVelocity = {}, persistentVelocity = {};
    std::array<int,2> onGround; // index one, 0 = false, 1 = true; index 2, 1 2 3 4 for object type
};

struct GameStates {
    bool cutscenePlayed = false;
    int frames = 0;
    bool pause = false;
    bool hoverOnButton = false;
    int timeBetweenFrames = 0; // in milliseconds
    int fps = 0;
};

struct MusicStates {
    int musicPlaying = -1; // matches the index of music
    float startTime;
};

struct States {
    struct PlayerStates playerStates;
    struct GameStates gameStates;
    struct MusicStates musicStates;
    std::array<int,3> stagePoints = {};
    int currentStage = 0;
};

struct Rotation {
    std::array<float,2> previousMouse, currentMouse;
    float xzRotation=0, yzRotation=0, total_yzRotation=0;
    bool newMousePos;
};

struct Art {
    char menu[30] = "art/menu_art.png";
    char cutscene[30] = "art/cutscene_art2.png";
    char transition[30] = "art/transition.png";
    char win[30] = "art/win.png";
};

struct FileData {
    std::array<float,3> center;
    float size, height;
};

struct StageData {
    std::array<int,3> backgroundColor;
    std::array<float,3> camera;
    std::array<std::array<float,3>,9> playerHitbox = {};
};

struct SoundFiles {
    char menuMusic[30] = "sounds/menu.wav"; // music[0]
    char cutsceneMusic[30] = "sounds/cutscene.wav"; // music[1]
    char iceMelting[30] = "sounds/ice_melt.wav"; // sfx[0]
    char gameMusic[30] = "sounds/game.wav"; // music[2], polargeist from geometry dash cause why not
    char win[30] = "sounds/win.wav"; // sfx[1]
    char lose[30] = "sounds/lose.wav"; // sfx[2]
    char transition[35] = "sounds/mixkit-air-woosh-1489.wav"; // https://mixkit.co/free-sound-effects/interface/
    char click[38] = "sounds/mixkit-select-click-1109.wav"; // https://mixkit.co/free-sound-effects/whoosh/
};

class Files {
    private:
        std::array<struct FileData, NUMBEROFFILES> fileMetaData;
        std::array<struct StageData, NUMBEROFSTAGES> stageMetaData;
        std::vector<std::string> stageOne = {"obj_files/1_water_0_2.obj",
            "obj_files/1_floe_0_0.obj",
            "obj_files/1_floe_1_1.obj",
            "obj_files/1_end_0_3.obj"};
    public:
        struct Art art;
        struct SoundFiles soundFiles;
        std::vector<FEHSound> music, sfx;
        void loadFile(Container& container, const std::string& fileName);
        void loadStage(Container& container, int stage);
        Files();
};

class Container {
    public:
        struct Objects objects; // will add more once the rest are in
        struct Screen screen;
        struct States states;
        struct Rotation rotation;
        Files files;
};