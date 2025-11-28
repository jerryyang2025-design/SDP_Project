#pragma once

#include <vector>
#include <array>
#include <map>

#define SCREEN_X 320
#define SCREEN_Y 240

struct Object {
    std::vector<std::array<float,3>> vertices;
    std::vector<std::array<int,3>> faces, faceColors, hitbox;
    std::array<float,3> center, color;
    int reflectionValue;
    int refractionValue;
};

struct Objects {
    std::vector<std::array<float,3>> playerHitbox;
    std::array<float,3> lightSource = {-1000,1000,-1000}, cameraPosition, cameraVector, cameraUpVector, cameraRightVector; // play around with the values
    struct Object end, water;
    std::vector<struct Object> platforms, movingPlatforms;
};

struct line {
    int y, x1, x2;
    char color[12]; // color converted to hexadecimal
};

struct Screen {
    std::vector<std::array<float,4>> vertices; // stored as a float to keep accurate depth value (not z, find projection with cameraVector), and in or out of view
    std::vector<std::array<int,3>> faces, faceColors;
    std::map<std::array<std::array<int,SCREEN_X>,SCREEN_Y>,float> depths; // used for z buffer
    std::map<std::array<std::array<int,SCREEN_X>,SCREEN_Y>,std::array<int,3>> currentPixels, previousPixels; // previous to draw only new changes
};

class Container {
    public:
        struct Objects objects; // will add more once the rest are in
        struct Screen screen;
};