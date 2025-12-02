#pragma once

#include <vector>
#include <array>
#include <limits>

#define SCREEN_X 320
#define SCREEN_Y 240
#define FPS 30

struct Object {
    std::vector<std::array<float,3>> vertices;
    std::vector<std::array<int,3>> faces, faceColors, hitbox;
    std::array<float,3> center, color;
    float reflectionValue;
    float refractionValue;
};

struct Objects {
    std::vector<std::array<float,3>> playerHitbox, snow;
    std::array<float,3> lightSource = {-10000,10000,-10000}, cameraPosition, cameraVector = {0,0,1}, cameraUpVector = {0,1,0}, cameraRightVector = {1,0,0}; // play around with the light position
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
    int placeholder;
};

struct GameStates {
    int frames = 0;
};

struct States {
    struct PlayerStates playerStates;
    struct GameStates gameStates;
};

class Files {
    private:

    public:
        void loadFile();
        void loadStage();
};

class Container {
    public:
        struct Objects objects; // will add more once the rest are in
        struct Screen screen;
        struct States states;
};