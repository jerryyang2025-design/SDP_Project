#include <fstream>
#include <string>
#include "header_files/data.h"
#include "header_files/utils.h"
#include "header_files/environment_effects.h"

#define ICECOLOR {230, 230, 240}
#define WATERCOLOR {230, 230, 240}
#define ICEREFLECTION 1
#define WATERREFLECTION 0.5
#define ICEREFRACTION 15
#define WATERREFRACTION 5
#define STARTING_HEIGHT 500

Files::Files() { // edit the location and size of each object/stage
    fileMetaData[0].center = {0, 0, 0};
    fileMetaData[0].size = 300;
    fileMetaData[0].height = 50;
    fileMetaData[1].center = {0, 0, 0};
    fileMetaData[1].size = 5000;
    fileMetaData[1].height = 0;

    stageMetaData[0].backgroundColor = {140, 170, 210};
    stageMetaData[0].camera = {0, PLAYER_HEIGHT + STARTING_HEIGHT, 0};
    stageMetaData[0].playerHitbox = generateHitbox(stageMetaData[0].camera,50);
}

void Files::loadFile(Container& container, const std::string& fileName) {
    std::ifstream file;

    file.open(fileName);

    std::vector<std::string> objectType = split(fileName,'/');
    objectType = split(objectType[1],'_');
    int type = 0, index, fileID;

    index = std::stoi(objectType[2]);
    fileID = std::stoi(objectType[3]);

    if (objectType[1] == "floe") {
        type = 1;
        struct Object platform;
        container.objects.platforms.push_back(platform); // make sure to load them in the stated order
        container.objects.platforms[index].color = ICECOLOR;
        container.objects.platforms[index].reflectionValue = ICEREFLECTION;
        container.objects.platforms[index].refractionValue = ICEREFRACTION;
        container.objects.platforms[index].center = fileMetaData[fileID].center;
    } else if (objectType[1] == "move") {
        type = 2;
        struct Object platform;
        container.objects.movingPlatforms.push_back(platform);
        container.objects.movingPlatforms[index].color = ICECOLOR;
        container.objects.movingPlatforms[index].reflectionValue = ICEREFLECTION;
        container.objects.movingPlatforms[index].refractionValue = ICEREFRACTION;
        container.objects.movingPlatforms[index].center = fileMetaData[fileID].center;
    } else if (objectType[1] == "end") {
        type = 3;
        container.objects.end.vertices.clear();
        container.objects.end.faces.clear();
        container.objects.end.faceColors.clear();
        container.objects.end.hitbox.clear();
        container.objects.end.color = ICECOLOR;
        container.objects.end.reflectionValue = ICEREFLECTION;
        container.objects.end.refractionValue = ICEREFRACTION;
        container.objects.end.center = fileMetaData[fileID].center;
    } else if (objectType[1] == "water") {
        type = 4;
        container.objects.water.vertices.clear();
        container.objects.water.faces.clear();
        container.objects.water.faceColors.clear();
        container.objects.water.hitbox.clear();
        container.objects.water.color = WATERCOLOR;
        container.objects.water.reflectionValue = WATERREFLECTION;
        container.objects.water.refractionValue = WATERREFRACTION;
        container.objects.water.center = fileMetaData[fileID].center;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> testline = split(line,' ');
        if (testline[0] == "v") {
            std::array<float,3> vertex;
            vertex[0] = std::stof(testline[1]) * fileMetaData[fileID].size + fileMetaData[fileID].center[0];
            vertex[1] = std::stof(testline[2]) * fileMetaData[fileID].height + fileMetaData[fileID].center[1];
            vertex[2] = std::stof(testline[3]) * fileMetaData[fileID].size + fileMetaData[fileID].center[2];
            if (type == 1) {
                container.objects.platforms[index].vertices.push_back(vertex);
            } else if (type == 2) {
                container.objects.movingPlatforms[index].vertices.push_back(vertex);
            } else if (type == 3) {
                container.objects.end.vertices.push_back(vertex);
            } else if (type == 4) {
                container.objects.water.vertices.push_back(vertex);
                printf("%f\n",vertex[0]);
            }
        }
        if (testline[0] == "f") {
            std::array<int,3> face;
            face[0] = std::stoi(split(testline[1],'/')[0]) - 1;
            face[1] = std::stoi(split(testline[2],'/')[0]) - 1;
            face[2] = std::stoi(split(testline[3],'/')[0]) - 1;
            if (type == 1) {
                container.objects.platforms[index].faces.push_back(face);
                container.objects.platforms[index].hitbox.push_back(face);
                container.objects.platforms[index].faceColors.push_back(ICECOLOR);
            } else if (type == 2) {
                container.objects.movingPlatforms[index].faces.push_back(face);
                container.objects.movingPlatforms[index].hitbox.push_back(face);
                container.objects.movingPlatforms[index].faceColors.push_back(ICECOLOR);
            } else if (type == 3) {
                container.objects.end.faces.push_back(face);
                container.objects.end.hitbox.push_back(face);
                container.objects.end.faceColors.push_back(ICECOLOR);
            } else if (type == 4) {
                container.objects.water.faces.push_back(face);
                container.objects.water.faceColors.push_back(WATERCOLOR);
            }
        }
    }

    file.close();
}

void resetData(Container& container) { // does not reset end or water, since those will be in every stage and will be overwritten regardless
    container.states.gameStates.frames = 0;
    container.states.gameStates.pause = false;
    container.states.playerStates.tempVelocity = {};
    container.states.playerStates.persistentVelocity = {};
    container.objects.cameraVector = {0,0,1};
    container.objects.cameraUpVector = {0,1,0};
    container.objects.cameraRightVector = {1,0,0};
    container.objects.snow.clear();
    container.objects.platforms.clear();
    container.objects.movingPlatforms.clear();
    container.rotation.currentMouse = {};
    container.rotation.previousMouse = {};
    container.rotation.total_yzRotation = 0;
    container.rotation.xzRotation = 0;
    container.rotation.yzRotation = 0;
    container.screen.currentPixels = {};
    container.screen.previousPixels = {};
}

void Files::loadStage(Container& container, int stage) {
    resetData(container);
    createSnow(container);

    // test
    // Object* object = new Object();
    // object->center = {0, 100, 200};
    // object->color = {230, 230, 240};
    // object->reflectionValue = 0.5;
    // object->refractionValue = 15;
    
    // object->vertices.push_back({100, 200, 400}); // 0: top-right
    // object->vertices.push_back({100, 0, 400}); // 1: bottom-right
    // object->vertices.push_back({-100, 200, 400}); // 2: top-left
    // object->vertices.push_back({-100, 0, 200}); // 3: bottom-left
    // object->vertices.push_back({0, 1000, 1500}); // 4: up/back
    // object->vertices.push_back({100, 800, 800}); // 5: top-right-back
    // object->vertices.push_back({5000, 0, 5000});
    // object->vertices.push_back({-5000, 0, -5000});
    // object->vertices.push_back({5000, 0, -5000});
    // object->vertices.push_back({-5000, 0, 5000});
    
    // object->faces.push_back({0, 1, 2}); // top-right triangle
    // object->faceColors.push_back({230, 230, 240});
    // object->faces.push_back({2, 1, 3}); // bottom-left triangle
    // object->faceColors.push_back({230, 230, 240});
    // object->faces.push_back({2, 4, 0}); // edge case test
    // object->faceColors.push_back({230, 230, 240});
    // object->faces.push_back({2, 5, 1}); // z buffer test
    // object->faceColors.push_back({230, 230, 240});
    // object->faces.push_back({6, 8, 7}); // floor
    // object->faceColors.push_back({200, 220, 240});
    // object->faces.push_back({6, 7, 9}); // floor
    // object->faceColors.push_back({200, 220, 240});
    
    // object->hitbox.push_back({0, 1, 2}); // top-right triangle
    // object->hitbox.push_back({2, 1, 3}); // bottom-left triangle
    // object->hitbox.push_back({2, 4, 0}); // edge case test
    // object->hitbox.push_back({2, 5, 1}); // z buffer test
    // object->hitbox.push_back({6, 8, 7}); // floor
    // object->hitbox.push_back({6, 7, 9}); // floor

    // container.objects.platforms.push_back(*object);

    if (stage == 1) {
        container.objects.backgroundColor = stageMetaData[0].backgroundColor;
        container.objects.cameraPosition = stageMetaData[0].camera;
        container.objects.playerHitbox = stageMetaData[0].playerHitbox;
        for (int i = 0; i < stageOne.size(); i++) {
            loadFile(container,stageOne[i]);
        }
    } else if (stage == 2) {

    } else if (stage == 3) {
        
    }
}