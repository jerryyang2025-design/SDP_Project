#include <fstream>
#include <string>
#include "header_files/data.h"
#include "header_files/utils.h"

#define ICECOLOR {230, 230, 240}
#define WATERCOLOR {230, 230, 240}
#define ICEREFLECTION 1
#define WATERREFLECTION 0.5
#define ICEREFRACTION 15
#define WATERREFRACTION 5

Files::Files() { // edit the location and size of each object/stage
    fileMetaData[0].center = {0, 0, 0};
    fileMetaData[0].size = 1;
    fileMetaData[0].height = 1;

    stageMetaData[0].backgroundColor = {140, 170, 200};
    stageMetaData[0].camera = {0, 100, 0};
}

void Files::loadFile(Container& container, const std::string& fileName) {
    std::ifstream file;

    file.open(fileName);

    std::vector<std::string> objectType = split(fileName,'_');
    int type = 0, index, fileID;

    index = std::stoi(objectType[2]);
    fileID = std::stoi(objectType[3]);

    if (objectType[1] == "floe") {
        type = 1;
        container.objects.platforms[index].color = ICECOLOR;
        container.objects.platforms[index].reflectionValue = ICEREFLECTION;
        container.objects.platforms[index].refractionValue = ICEREFRACTION;
        container.objects.platforms[index].center = fileMetaData[fileID].center;
    } else if (objectType[1] == "move") {
        type = 2;
        container.objects.movingPlatforms[index].color = ICECOLOR;
        container.objects.movingPlatforms[index].reflectionValue = ICEREFLECTION;
        container.objects.movingPlatforms[index].refractionValue = ICEREFRACTION;
        container.objects.movingPlatforms[index].center = fileMetaData[fileID].center;
    } else if (objectType[1] == "end") {
        type = 3;
        container.objects.end.color = ICECOLOR;
        container.objects.end.reflectionValue = ICEREFLECTION;
        container.objects.end.refractionValue = ICEREFRACTION;
        container.objects.end.center = fileMetaData[fileID].center;
    } else if (objectType[1] == "water") {
        type = 4;
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
                container.objects.water.hitbox.push_back(face);
                container.objects.water.faceColors.push_back(WATERCOLOR);
            }
        }
    }

    file.close();
}

void Files::loadStage(Container& container, int stage) {
    
}