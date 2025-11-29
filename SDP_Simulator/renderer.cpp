#include <math.h>
#include "data.h"
#include "utils.h"

#define BRIGHTNESS 3000000 // may adjust depending on light source distance
#define SNOWCOLOR {30, 30, 30}

void polygonLightning(struct Object& object, int polygon, std::array<float,3> lightSource, std::array<float,3> cameraPosition) {
    std::array<float,3> center,vectorOne,vectorTwo,normalVector,toLightVector,toCameraVector,halfVector;
    std::vector<std::array<float,3>> vertices = {object.vertices[object.faces[polygon][0]],
        object.vertices[object.faces[polygon][1]], 
        object.vertices[object.faces[polygon][2]]};

    for (int i = 0; i < 3; i++) { // maybe precompute and store polygon vectors if noticably slow
        center[i] = (vertices[0][i] + vertices[1][i] + vertices[2][i]) / 3;
        vectorOne[i] = vertices[1][i] - vertices[0][i];
        vectorTwo[i] = vertices[2][i] - vertices[0][i];
        toLightVector[i] = lightSource[i] - center[i];
        toCameraVector[i] = cameraPosition[i] -center[i];
    }

    normalVector = crossProduct(vectorOne,vectorTwo);

    float dir = direction(normalVector,toLightVector) + 1.5; // 1.5 to prevent it from going negative, so that direction still matters and to avoid using max(). MAYBE ADJUST
    float dist = distance(center[0] - lightSource[0],center[1] - lightSource[1],center[2] - lightSource[2]);

    float magnitudeOne = distance(toLightVector[0],toLightVector[1],toLightVector[2]);
    float magnitudeTwo = distance(toCameraVector[0],toCameraVector[1],toCameraVector[2]);
    for (int i = 0; i < 3; i++) {
        toLightVector[i] /= magnitudeOne;
        toCameraVector[i] /= magnitudeTwo;
        halfVector[i] = toLightVector[i] + toCameraVector[i];
    }

    float theta = angle(halfVector, normalVector);

    float shine;
    if (dotProduct(toLightVector,normalVector) > 0) {
        shine = object.reflectionValue / (theta * theta + 1) + 1; // maybe adjust the squared part, that's to make sure it goes down quickly
    } else {
        shine = 1;
    }

    float lightValueMultiplier = BRIGHTNESS * dir * shine / (dist * dist);

    if (lightValueMultiplier >= 1) {
        for (int i = 0; i < 3; i++) {
            object.faceColors[polygon][i] = clamp(object.color[i] * lightValueMultiplier,0,255);
        }
    } else {
        object.faceColors[polygon][0] = clamp(object.color[0] * lightValueMultiplier,0,255);
        object.faceColors[polygon][1] = clamp(object.color[1] * pow(lightValueMultiplier,0.8),0,255);
        object.faceColors[polygon][2] = clamp(object.color[2] * pow(lightValueMultiplier,0.5),0,255); // adjust values
    }
}

void polygonRefraction(struct Object& object, int polygon, std::array<float,3> lightSource, std::array<float,3> cameraPosition) {
    std::array<float,3> center,vectorOne,vectorTwo,normalVector,toLightVector,toCameraVector,halfVector;
    std::vector<std::array<float,3>> vertices = {object.vertices[object.faces[polygon][0]],
        object.vertices[object.faces[polygon][1]], 
        object.vertices[object.faces[polygon][2]]};
    
    for (int i = 0; i < 3; i++) {
        center[i] = (vertices[0][i] + vertices[1][i] + vertices[2][i]) / 3;
        vectorOne[i] = vertices[1][i] - vertices[0][i];
        vectorTwo[i] = vertices[2][i] - vertices[0][i];
        toLightVector[i] = lightSource[i] - center[i];
        toCameraVector[i] = cameraPosition[i] -center[i];
    }

    float magnitudeOne = distance(toLightVector[0],toLightVector[1],toLightVector[2]);
    float magnitudeTwo = distance(toCameraVector[0],toCameraVector[1],toCameraVector[2]);
    for (int i = 0; i < 3; i++) {
        toLightVector[i] /= magnitudeOne;
        toCameraVector[i] /= magnitudeTwo;
        halfVector[i] = toLightVector[i] + toCameraVector[i];
    }

    normalVector = crossProduct(vectorOne,vectorTwo);

    float shift = direction(normalVector,halfVector);

    shift = pow(1 - clamp(shift,0,1), 0.5);

    float colorShift[3] = {0.5 + 0.5 * sin(shift * 6.2831 + 0.0), 
        0.5 + 0.5 * sin(shift * 6.2831 + 2.0), 
        0.5 + 0.5 * sin(shift * 6.2831 + 4.0)};

    for (int i = 0; i < 3; i++) {
        object.faceColors[polygon][i] = clamp(object.faceColors[polygon][i] + colorShift[i] * object.refractionValue,0,255);
    }
}

void handleLighting(struct Objects& objects) {
    for (int i = 0; i < objects.platforms.size(); i++) {
        for (int j = 0; j < objects.platforms[i].faces.size(); j++) {
            polygonLightning(objects.platforms[i],j,objects.lightSource,objects.cameraPosition);
            polygonRefraction(objects.platforms[i],j,objects.lightSource,objects.cameraPosition);
        }
    }
    for (int i = 0; i < objects.movingPlatforms.size(); i++) {
        for (int j = 0; j < objects.movingPlatforms[i].faces.size(); j++) {
            polygonLightning(objects.movingPlatforms[i],j,objects.lightSource,objects.cameraPosition);
            polygonRefraction(objects.movingPlatforms[i],j,objects.lightSource,objects.cameraPosition);
        }
    }
    for (int j = 0; j < objects.end.faces.size(); j++) {
        polygonLightning(objects.end,j,objects.lightSource,objects.cameraPosition);
        polygonRefraction(objects.end,j,objects.lightSource,objects.cameraPosition);
    }
    for (int j = 0; j < objects.water.faces.size(); j++) {
        polygonLightning(objects.water,j,objects.lightSource,objects.cameraPosition);
        polygonRefraction(objects.water,j,objects.lightSource,objects.cameraPosition);
    }
}

void project(const struct Objects& objects, const struct Object& object, struct Screen& screen, int vertex) {
    std::array<float,2> depthResults = depth(objects,object.vertices[vertex]);
    std::array<float,2> xResults = fieldOfViewBoundSide(objects,object.vertices[vertex]);
    std::array<float,2> yResults = fieldOfViewBoundUp(objects,object.vertices[vertex]);

    float hidden = 0;
    if (depthResults[1] > 0.5 || xResults[1] > 0.5 || yResults[1] > 0.5) {
        hidden = 1;
    }
    int x = xResults[0] + SCREEN_X / 2;
    int y = -yResults[0] + SCREEN_Y / 2;
    float depth = depthResults[0];

    std::array<float,4> screenVertex = {x,y,depth,hidden};
    screen.vertices.push_back(screenVertex);
}

void projectAll(Container& container) {
    for (int i = 0; i < container.objects.platforms.size(); i++) {
        int size = container.screen.vertices.size();
        for (int j = 0; j < container.objects.platforms[i].vertices.size(); j++) {
            project(container.objects,container.objects.platforms[i],container.screen,j);
        }
        for (int j = 0; j < container.objects.platforms[i].faces.size(); j++) {
            if (container.screen.vertices[container.objects.platforms[i].faces[j][0] + size][3] < 0.5 || container.screen.vertices[container.objects.platforms[i].faces[j][1] + size][3] < 0.5 || container.screen.vertices[container.objects.platforms[i].faces[j][2] + size][3] < 0.5) {
                std::array<int,3> face = container.objects.platforms[i].faces[j], faceColor = container.objects.platforms[i].faceColors[j];
                for (int k = 0; k < 3; k++) {
                    face[k] += size;
                }
                container.screen.faces.push_back(face);
                container.screen.faceColors.push_back(faceColor);
            }
        }
    }
    for (int i = 0; i < container.objects.movingPlatforms.size(); i++) {
        int size = container.screen.vertices.size();
        for (int j = 0; j < container.objects.movingPlatforms[i].vertices.size(); j++) {
            project(container.objects,container.objects.movingPlatforms[i],container.screen,j);
        }
        for (int j = 0; j < container.objects.movingPlatforms[i].faces.size(); j++) {
            if (container.screen.vertices[container.objects.movingPlatforms[i].faces[j][0] + size][3] < 0.5 || container.screen.vertices[container.objects.movingPlatforms[i].faces[j][1] + size][3] < 0.5 || container.screen.vertices[container.objects.movingPlatforms[i].faces[j][2] + size][3] < 0.5) {
                std::array<int,3> face = container.objects.movingPlatforms[i].faces[j], faceColor = container.objects.movingPlatforms[i].faceColors[j];
                for (int k = 0; k < 3; k++) {
                    face[k] += size;
                }
                container.screen.faces.push_back(face);
                container.screen.faceColors.push_back(faceColor);
            }
        }
    }
    int size = container.screen.vertices.size();
    for (int j = 0; j < container.objects.end.vertices.size(); j++) {
        project(container.objects,container.objects.end,container.screen,j);
    }
    for (int j = 0; j < container.objects.end.faces.size(); j++) {
        if (container.screen.vertices[container.objects.end.faces[j][0] + size][3] < 0.5 || container.screen.vertices[container.objects.end.faces[j][1] + size][3] < 0.5 || container.screen.vertices[container.objects.end.faces[j][2] + size][3] < 0.5) {
            std::array<int,3> face = container.objects.end.faces[j], faceColor = container.objects.end.faceColors[j];
            for (int k = 0; k < 3; k++) {
                face[k] += size;
            }
            container.screen.faces.push_back(face);
            container.screen.faceColors.push_back(faceColor);
        }
    }
    size = container.screen.vertices.size();
    for (int j = 0; j < container.objects.water.vertices.size(); j++) {
        project(container.objects,container.objects.water,container.screen,j);
    }
    for (int j = 0; j < container.objects.water.faces.size(); j++) {
        if (container.screen.vertices[container.objects.water.faces[j][0] + size][3] < 0.5 || container.screen.vertices[container.objects.water.faces[j][1] + size][3] < 0.5 || container.screen.vertices[container.objects.water.faces[j][2] + size][3] < 0.5) {
            std::array<int,3> face = container.objects.water.faces[j], faceColor = container.objects.water.faceColors[j];
            for (int k = 0; k < 3; k++) {
                face[k] += size;
            }
            container.screen.faces.push_back(face);
            container.screen.faceColors.push_back(faceColor);
        }
    }
}

void colorPolygon(struct Screen& screen, int polygon) {
    std::array<std::array<float,4>,3> vertices = {screen.vertices[screen.faces[polygon][0]],screen.vertices[screen.faces[polygon][1]],screen.vertices[screen.faces[polygon][2]]};
    int minX = floor(vertices[0][0]), maxX = ceil(vertices[0][0]), minY = floor(vertices[0][1]), maxY = ceil(vertices[0][1]);
    for (int i = 0; i < 3; i++) {
        if (vertices[i][0] < minX) {
            minX = vertices[i][0];
        } else if (vertices[i][0] > maxX) {
            maxX = vertices[i][0];
        }
        if (vertices[i][1] < minY) {
            minX = vertices[i][1];
        } else if (vertices[i][1] > maxY) {
            maxX = vertices[i][1];
        }
    }
    // figure out floor and ceiling for the vertices themselves
}
// add snow when rendering, add snow color to pixel colors
// add overall render function that calls all of this