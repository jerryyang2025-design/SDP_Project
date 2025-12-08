#include <fstream>
#include <string>
#include "header_files/data.h"
#include "header_files/utils.h"
#include "header_files/environment_effects.h"

#define ICECOLOR {230, 230, 240}
#define WATERCOLOR {190, 215, 250}
#define ICEREFLECTION 1.3
#define WATERREFLECTION 0.8
#define ICEREFRACTION 15
#define WATERREFRACTION 5
#define STARTING_HEIGHT 500

Files::Files() { // edit the location and size of each object/stage
    fileMetaData[0].center = {-800, 0, -1300}; // floe
    fileMetaData[0].size = 150;
    fileMetaData[0].height = 100;

    fileMetaData[1].center = {-700, 0, -850}; // floe
    fileMetaData[1].size = 150;
    fileMetaData[1].height = 100;

    fileMetaData[7].center = {-800, 0, -200}; // floe
    fileMetaData[7].size = 150;
    fileMetaData[7].height = 100;

    fileMetaData[8].center = {-300, 0, 0}; // floe
    fileMetaData[8].size = 150;
    fileMetaData[8].height = 100;

    fileMetaData[9].center = {150, 0, 100}; // floe
    fileMetaData[9].size = 150;
    fileMetaData[9].height = 100;

    fileMetaData[10].center = {700, 0, 400}; // floe
    fileMetaData[10].size = 150;
    fileMetaData[10].height = 100;

    fileMetaData[2].center = {1000, 0, 1000}; // water
    fileMetaData[2].size = 1000;
    fileMetaData[2].height = 0;

    fileMetaData[3].center = {800, 0, -150}; // end
    fileMetaData[3].size = 150;
    fileMetaData[3].height = 100;

    fileMetaData[4].center = {-1000, 0, 1000}; // water
    fileMetaData[4].size = 1000;
    fileMetaData[4].height = 0;

    fileMetaData[5].center = {1000, 0, -1000}; // water
    fileMetaData[5].size = 1000;
    fileMetaData[5].height = 0;

    fileMetaData[6].center = {-1000, 0, -1000}; // water
    fileMetaData[6].size = 1000;
    fileMetaData[6].height = 0;

    // Stage data
    stageMetaData[0].backgroundColor = {150, 190, 230};
    stageMetaData[0].camera = {fileMetaData[0].center[0], PLAYER_HEIGHT + STARTING_HEIGHT, fileMetaData[0].center[2]};

    stageMetaData[1].backgroundColor = {150, 190, 230};
    stageMetaData[1].camera = {fileMetaData[11].center[0], PLAYER_HEIGHT + STARTING_HEIGHT, fileMetaData[11].center[2]};

    stageMetaData[2].backgroundColor = {150, 190, 230};
    stageMetaData[2].camera = {fileMetaData[20].center[0], PLAYER_HEIGHT + STARTING_HEIGHT, fileMetaData[20].center[2]};
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
        printf("test");
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
        struct Object platform;
        container.objects.water.push_back(platform);
        container.objects.water[index].vertices.clear();
        container.objects.water[index].faces.clear();
        container.objects.water[index].faceColors.clear();
        container.objects.water[index].hitbox.clear();
        container.objects.water[index].color = WATERCOLOR;
        container.objects.water[index].reflectionValue = WATERREFLECTION;
        container.objects.water[index].refractionValue = WATERREFRACTION;
        container.objects.water[index].center = fileMetaData[fileID].center;
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
                container.objects.water[index].vertices.push_back(vertex);
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
                container.objects.water[index].faces.push_back(face);
                container.objects.water[index].faceColors.push_back(WATERCOLOR);
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
    container.objects.water.clear();
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

    if (stage == 1) {
        container.objects.backgroundColor = stageMetaData[0].backgroundColor;
        container.objects.cameraPosition = stageMetaData[0].camera;
        container.objects.playerHitbox = generateHitbox(stageMetaData[0].camera,50);
        for (int i = 0; i < stageOne.size(); i++) {
            loadFile(container,stageOne[i]);
        }
    // } else if (stage == 2) {
    //     container.objects.backgroundColor = stageMetaData[1].backgroundColor;
    //     container.objects.cameraPosition = stageMetaData[1].camera;
    //     container.objects.playerHitbox = generateHitbox(stageMetaData[1].camera,50);
    //     for (int i = 0; i < stageTwo.size(); i++) {
    //         loadFile(container,stageTwo[i]);
    //     }
    // } else if (stage == 3) {
    //     container.objects.backgroundColor = stageMetaData[2].backgroundColor;
    //     container.objects.cameraPosition = stageMetaData[2].camera;
    //     container.objects.playerHitbox = generateHitbox(stageMetaData[2].camera,50);
    //     for (int i = 0; i < stageThree.size(); i++) {
    //         loadFile(container,stageThree[i]);
    //     }
    }
}