#pragma once

#include <vector>
#include <array>
#include <map>

#define SCREEN_X 319
#define SCREEN_Y 239

struct Object {
    std::vector<std::array<float,3>> vertices;
    std::vector<std::array<int,3>> faces, faceColors, hitbox;
    std::array<float,3> center, color;
    int reflectionValue;
    int refractionValue;
};

struct Objects {
    std::vector<std::array<float,3>> playerHitbox;
    std::array<float,3> lightSource = {-1000,1000,-1000}; // check if I can do this in a .h file, also play around with the values
    struct Object end, water;
    std::vector<struct Object> platforms;
};

struct Polygons {
    float depth;
    std::array<float,3> color;
    std::array<std::array<int,2>,3> vertices;
};

struct Screen {
    int width = SCREEN_X, height = SCREEN_Y;
    std::vector<struct Polygons> polygons;
    std::map<std::array<std::array<int,SCREEN_X>,SCREEN_Y>,char[30]> currentPixels, previousPixels;
};

class Container {
    public:
        struct Objects objects; // will add more once the rest are in
        struct Screen screen;
};