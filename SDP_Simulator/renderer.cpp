#include <math.h>
#include "FEHLCD.h"
#include "data.h"
#include "utils.h"
#include "renderer.h"

#define BRIGHTNESS 120000000 // may adjust depending on light source distance
#define SNOWCOLOR 30
#define SNOWSIZE 5
#define NEAR_PLANE 1.0f

/*
Function to calculate the brightness value of a single polygon
*/

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

    float dir = clamp(direction(normalVector,toLightVector) / 2 + 1,0.5,1.5); // 1.5 to prevent it from going negative, so that direction still matters and to avoid using max(). MAYBE ADJUST
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

/*
Function to calculate the color shift of a single polygon
*/

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

/*
Function to calculate the lighting/color of every polygon
*/

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

/*
Function to project a single 3d vertex into 2d
*/

void project(const struct Objects& objects, const std::vector<std::array<float,3>>& vertices, struct Screen& screen, int vertex) {
    std::array<float,2> depthResults = depth(objects,vertices[vertex]);
    std::array<float,2> xResults = fieldOfViewBoundSide(objects,vertices[vertex]);
    std::array<float,2> yResults = fieldOfViewBoundUp(objects,vertices[vertex]);

    float hidden = 0;
    if (depthResults[1] > 0.5 || xResults[1] > 0.5 || yResults[1] > 0.5) {
        hidden = 1;
    }
    int x = round(xResults[0] + SCREEN_X / 2);
    int y = round(-yResults[0] + SCREEN_Y / 2);
    float depth = depthResults[0];

    std::array<float,4> screenVertex = {x,y,depth,hidden};
    screen.vertices.push_back(screenVertex);
}

/*
Function to project a single effect vertex from 3d to 2d
*/

void projectEffects(const struct Objects& objects, const std::vector<std::array<float,3>>& vertices, struct Screen& screen, int vertex) {
    std::array<float,2> depthResults = depth(objects,vertices[vertex]);
    std::array<float,2> xResults = fieldOfViewBoundSide(objects,vertices[vertex]);
    std::array<float,2> yResults = fieldOfViewBoundUp(objects,vertices[vertex]);

    float hidden = 0;
    if (depthResults[1] > 0.5) {
        hidden = 1;
    }

    if (hidden < 0.5) {
        int x = round(xResults[0] + SCREEN_X / 2);
        int y = round(-yResults[0] + SCREEN_Y / 2);
        float depth = depthResults[0];

        std::array<float,3> screenVertex = {x,y,depth};
        screen.effects.push_back(screenVertex);
    }
}

/*
Function to project every vertex from 3d to 2d
*/

void projectAll(Container& container) {
    container.screen.vertices.clear();
    container.screen.effects.clear();
    container.screen.faces.clear();
    container.screen.faceColors.clear();
    for (int i = 0; i < container.objects.platforms.size(); i++) {
        int size = container.screen.vertices.size();
        for (int j = 0; j < container.objects.platforms[i].vertices.size(); j++) {
            project(container.objects,container.objects.platforms[i].vertices,container.screen,j);
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
            project(container.objects,container.objects.movingPlatforms[i].vertices,container.screen,j);
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
        project(container.objects,container.objects.end.vertices,container.screen,j);
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
        project(container.objects,container.objects.water.vertices,container.screen,j);
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
    for (int j = 0; j < container.objects.snow.size(); j++) {
        projectEffects(container.objects,container.objects.snow,container.screen,j);
    }
}

/*
Function to color a single polygon
*/

void colorPolygon(struct Screen& screen, int polygon) {
    std::array<std::array<float,4>,3> vertices = {screen.vertices[screen.faces[polygon][0]],screen.vertices[screen.faces[polygon][1]],screen.vertices[screen.faces[polygon][2]]};
    int minX = vertices[0][0], maxX = vertices[0][0], minY = vertices[0][1], maxY = vertices[0][1];
    for (int i = 0; i < 3; i++) {
        if (vertices[i][0] < minX) {
            minX = vertices[i][0];
        } else if (vertices[i][0] > maxX) {
            maxX = vertices[i][0];
        }
        if (vertices[i][1] < minY) {
            minY = vertices[i][1];
        } else if (vertices[i][1] > maxY) {
            maxY = vertices[i][1];
        }
    }
    minX = clamp(minX,0,SCREEN_X - 1);
    maxX = clamp(maxX,0,SCREEN_X - 1);
    minY = clamp(minY,0,SCREEN_Y - 1);
    maxY = clamp(maxY,0,SCREEN_Y - 1);
    
    for (int i = minY;i <= maxY;i++) {
        for (int j = minX;j <= maxX;j++) {
            std::array<int,2> pixel = {j,i};
            if (pointInTriangle(vertices,pixel)) {
                float testDepth = fragDepth(vertices,pixel);
                if (testDepth > 0) {
                    if (screen.depths[i][j] == std::numeric_limits<float>::infinity() || testDepth < screen.depths[i][j]) {
                        screen.depths[i][j] = testDepth;
                        screen.currentPixels[i][j] = screen.faceColors[polygon];
                    }
                }
            }
        }
    }
}

/*
Function to color a snow flake
*/

void colorSnow(struct Screen& screen, int snow) {
    std::array<float,3> point = screen.effects[snow];
    int radius;
    if (point[2] > 0.2) {
        radius = round(SNOWSIZE / point[2]);
    } else {
        radius = round(SNOWSIZE / 0.2);
    }
    int minX = point[0] - radius, maxX = point[0] + radius, minY = point[1] - radius, maxY = point[1] + radius;
    minX = clamp(minX,0,SCREEN_X - 1);
    maxX = clamp(maxX,0,SCREEN_X - 1);
    minY = clamp(minY,0,SCREEN_Y - 1);
    maxY = clamp(maxY,0,SCREEN_Y - 1);

    for (int i = minY;i <= maxY;i++) {
        for (int j = minX;j <= maxX;j++) {
            if (pythag(j - point[0],i - point[1]) <= radius) {
                float testDepth = point[2];
                if (screen.depths[i][j] == std::numeric_limits<float>::infinity() || testDepth < screen.depths[i][j]) {
                    screen.currentPixels[i][j][0] = clamp(screen.currentPixels[i][j][0] + SNOWCOLOR,0,255);
                    screen.currentPixels[i][j][1] = clamp(screen.currentPixels[i][j][1] + SNOWCOLOR,0,255);
                    screen.currentPixels[i][j][2] = clamp(screen.currentPixels[i][j][2] + SNOWCOLOR,0,255);
                }
            }
        }
    }
}

/*
Function to color the screen
*/

void colorAll(Container& container) {
    for (int i = 0; i < SCREEN_Y; i++) {
        for (int j = 0; j < SCREEN_X; j++) {
            container.screen.currentPixels[i][j] = container.objects.backgroundColor;
            container.screen.depths[i][j] = std::numeric_limits<float>::infinity();
        }
    }

    for (int i = 0; i < container.screen.faces.size(); i++) {
        colorPolygon(container.screen,i);
    }
    for (int i = 0; i < container.screen.effects.size(); i++) {
        colorSnow(container.screen,i);
    }
}

/*
Function to find horizontal line segments
*/

void findLines(Container& container) {
    container.screen.lines.clear();
    bool lineInProgress = false;
    for (int i = 0; i < SCREEN_Y; i++) {
        struct line newline;
        for (int j = 0; j < SCREEN_X; j++) {
            std::array<int,3> testColor = container.screen.currentPixels[i][j];
            if (!equals(container.screen.currentPixels[i][j],container.screen.previousPixels[i][j])) {
                if (lineInProgress && newline.color != rgbToHex(testColor[0],testColor[1],testColor[2])) {
                    newline.x2 = j - 1;
                    container.screen.lines.push_back(newline);
                    lineInProgress = false;
                }
                if (!lineInProgress) {
                    lineInProgress = true;
                    newline.color = rgbToHex(testColor[0],testColor[1],testColor[2]);
                    newline.y = i;
                    newline.x1 = j;
                }
            } else if (lineInProgress) {
                newline.x2 = j - 1;
                container.screen.lines.push_back(newline);
                lineInProgress = false;
            }
            if (j == SCREEN_X - 1 && lineInProgress) {
                newline.x2 = j;
                container.screen.lines.push_back(newline);
                lineInProgress = false;
            }
        }
    }
    container.screen.previousPixels = container.screen.currentPixels;
}

/*
Function to draw all lines on the screen
*/

void drawScreen(Container& container) {
    for (int i = 0; i < container.screen.lines.size(); i++) {
        LCD.SetFontColor(container.screen.lines[i].color);
        LCD.DrawHorizontalLine(container.screen.lines[i].y,container.screen.lines[i].x1,container.screen.lines[i].x2 + 1);
    }
}

/*
Master rendering function, finally
*/

void render(Container& container) {
    handleLighting(container.objects);
    projectAll(container);
    colorAll(container);
    findLines(container);
    drawScreen(container);
    LCD.Update();
}