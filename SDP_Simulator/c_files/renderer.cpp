#include <math.h>
#include "windows.h"
#include "FEHLCD.h"
#include "header_files/data.h"
#include "header_files/utils.h"
#include "header_files/renderer.h"

#define BRIGHTNESS 120000000 // may adjust depending on light source distance
#define SNOWCOLOR 30
#define SNOWSIZE 2000
#define NEAR_PLANE 1.0f

void polygonLightning(struct Objects& objects, struct Object& object, int polygon, std::array<float,3> lightSource, std::array<float,3> cameraPosition);
void polygonRefraction(struct Object& object, int polygon, std::array<float,3> lightSource, std::array<float,3> cameraPosition);
void clipPolygon(Objects& objects, Object& object, int polygon);

/*
structs used to separate heavy math into threads to speed up processing
includes the start and end indexes for a thread to iterate through
reference used for all threading functions and structs: https://www.cs.uah.edu/~mdb0013/Common/CodeVault/Code/WindowsThreads.html
Author: Jerry
*/
struct LightingThreadTask {
    Objects* objects;
    int startObject;
    int endObject;
    int mode;
};

struct ClipThreadTask {
    Objects* objects;
    int startObject;
    int endObject;
    int mode;
};

struct ProjectThreadTask {
    const Objects* objects;
    const std::vector<Object>* objectList;
    int startIndex;
    int endIndex;
    std::vector<std::array<float,4>> projectedVertices;
    std::vector<std::array<int,3>> projectedFaces;
    std::vector<std::array<int,3>> projectedFaceColors;
};

/*
helper funtions used for threading
iterates through the start and end indexes for a thread, performing the appropriate function calls for each
Author: Jerry
*/
DWORD WINAPI lightingWorker(LPVOID lpParameter) {
    LightingThreadTask* task = (LightingThreadTask*)lpParameter;
    Objects* objects = (*task).objects;

    if ((*task).mode == 0) {
        for (int i = (*task).startObject; i < (*task).endObject; i++) {
            for (int j = 0; j < (*objects).platforms[i].faces.size(); j++) {
                polygonLightning(*objects, (*objects).platforms[i], j, (*objects).lightSource, (*objects).cameraPosition);
                polygonRefraction((*objects).platforms[i], j, (*objects).lightSource, (*objects).cameraPosition);
            }
        }
    }
    else {
        for (int i = (*task).startObject; i < (*task).endObject; i++) {
            for (int j = 0; j < (*objects).water[i].faces.size(); j++) {
                polygonLightning(*objects, (*objects).water[i], j, (*objects).lightSource, (*objects).cameraPosition);
                polygonRefraction((*objects).water[i], j, (*objects).lightSource, (*objects).cameraPosition);
            }
        }
    }

    return 0;
}

DWORD WINAPI clipWorker(LPVOID lpParameter) {
    ClipThreadTask* task = (ClipThreadTask*)lpParameter;
    Objects* obj = (*task).objects;

    if ((*task).mode == 0) {
        for (int i = (*task).startObject; i < (*task).endObject; i++) {
            (*obj).platforms[i].tempVertices.clear();
            (*obj).platforms[i].tempFaces.clear();
            (*obj).platforms[i].tempFaceColors.clear();

            for (int j = 0; j < (*obj).platforms[i].faces.size(); j++) {
                clipPolygon(*obj, (*obj).platforms[i], j);
            }
        }
    }

    if ((*task).mode == 1) {
        (*obj).end.tempVertices.clear();
        (*obj).end.tempFaces.clear();
        (*obj).end.tempFaceColors.clear();

        for (int j = 0; j < (*obj).end.faces.size(); j++) {
            clipPolygon(*obj, (*obj).end, j);
        }
    }

    if ((*task).mode == 2) {
        for (int i = (*task).startObject; i < (*task).endObject; i++) {
            (*obj).water[i].tempVertices.clear();
            (*obj).water[i].tempFaces.clear();
            (*obj).water[i].tempFaceColors.clear();

            for (int j = 0; j < (*obj).water[i].faces.size(); j++) {
                clipPolygon(*obj, (*obj).water[i], j);
            }
        }
    }

    return 0;
}

DWORD WINAPI projectWorker(LPVOID lpParameter) {
    ProjectThreadTask* task = (ProjectThreadTask*)lpParameter;

    const Objects* objects = (*task).objects;

    for (int i = (*task).startIndex; i < (*task).endIndex; i++) {
        const Object& obj = (*(*task).objectList)[i];
        int vertexBase = (*task).projectedVertices.size();

        for (int j = 0; j < obj.tempVertices.size(); j++) {
            std::array<float,2> depthResults = depth(*objects,obj.tempVertices[j]);
            std::array<float,2> xResults = fieldOfViewBoundSide(*objects,obj.tempVertices[j]);
            std::array<float,2> yResults = fieldOfViewBoundUp(*objects,obj.tempVertices[j]);

            float hidden = 0;
            if ((depthResults[1] > 0.5) || (xResults[1] > 0.5) || (yResults[1] > 0.5)) {
                hidden = 1;
            }

            int x = round(xResults[0] + SCREEN_X / 2);
            int y = round(-yResults[0] + SCREEN_Y / 2);
            float depth = depthResults[0];

            std::array<float,4> screenVertex = {x, y, depth, hidden};
            (*task).projectedVertices.push_back(screenVertex);
        }

        for (int j = 0; j < obj.tempFaces.size(); j++) {
            int f0 = obj.tempFaces[j][0] + vertexBase;
            int f1 = obj.tempFaces[j][1] + vertexBase;
            int f2 = obj.tempFaces[j][2] + vertexBase;

            if ((*task).projectedVertices[f0][3] < 0.5 || (*task).projectedVertices[f1][3] < 0.5 || (*task).projectedVertices[f2][3] < 0.5) {
                std::array<int,3> face = {f0,f1,f2};
                (*task).projectedFaces.push_back(face);
                (*task).projectedFaceColors.push_back(obj.tempFaceColors[j]);
            }
            // if (!((*task).projectedVertices[f0][3] > 0.5 || (*task).projectedVertices[f1][3] > 0.5 || (*task).projectedVertices[f2][3] > 0.5)) {
            //     std::array<int,3> face = {f0,f1,f2};
            //     (*task).projectedFaces.push_back(face);
            //     (*task).projectedFaceColors.push_back(obj.tempFaceColors[j]);
            // }
        }
    }

    return 0;
}

/*
calculates and applies a lighting multiplier to a polygon based on the angle and distance to the light source, as well as the half vector
purpose is to add lighting to a polygon and adjust the color accordingly so that the polygons don't all blend together
Author: Jerry
*/

void polygonLightning(struct Objects& objects, struct Object& object, int polygon, std::array<float,3> lightSource, std::array<float,3> cameraPosition) {
    std::array<float,3> center,vectorOne,vectorTwo,normalVector,toLightVector,toCameraVector,halfVector;
    std::vector<std::array<float,3>> vertices = {object.vertices[object.faces[polygon][0]],
        object.vertices[object.faces[polygon][1]], 
        object.vertices[object.faces[polygon][2]]};

    int polyHidden = 0;
    for (int i = 0; i < 3; i++) {
        float x, y, z;
        toCameraSpace(objects, vertices[i], x, y, z);

        float aspect = (float)SCREEN_X / (float)SCREEN_Y;

        bool hidden = (z < NEAR_PLANE || x < -z * aspect || x > z * aspect || y < -z || y > z);
        if (hidden) {
            polyHidden += 1;
        }
    }
    if (polyHidden == 3) {
        return;
    }

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
calculates a color shift for a polygon based on the half vector of the light source and camera position
purpose is the give the ice a shiny light scattering effect
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
separate objects and creates threads for each, to apply the lighting to each in parallel
purpose is the apply the lighting system to all objects in a single function
Author: Jerry
*/

void handleLighting(struct Objects& objects) {
    int numThreads = 16;

    HANDLE threads[32];
    LightingThreadTask tasks[32];
    int threadCount = 0;

    int platformCount = objects.platforms.size();
    int perThread = (platformCount + numThreads - 1) / numThreads;

    for (int t = 0; t < numThreads; t++) {
        int start = t * perThread;
        int end = (t + 1) * perThread;

        if (start >= platformCount) {
            break;
        }
        if (end > platformCount) {
            end = platformCount;
        }

        tasks[threadCount].objects = &objects;
        tasks[threadCount].startObject = start;
        tasks[threadCount].endObject = end;
        tasks[threadCount].mode = 0;

        threads[threadCount] = CreateThread(NULL, 0, lightingWorker, &tasks[threadCount], 0, NULL);

        threadCount++;
    }

    int waterPolyCount = objects.water.size();
    perThread = (waterPolyCount + numThreads - 1) / numThreads;

    for (int t = 0; t < numThreads; t++) {
        int start = t * perThread;
        int end = (t + 1) * perThread;

        if (start >= waterPolyCount) {
            break;
        }
        if (end > waterPolyCount) {
            end = waterPolyCount;
        }

        tasks[threadCount].objects = &objects;
        tasks[threadCount].startObject = start;
        tasks[threadCount].endObject = end;
        tasks[threadCount].mode = 1;

        threads[threadCount] = CreateThread(NULL, 0, lightingWorker, &tasks[threadCount], 0, NULL);

        threadCount++;
    }

    WaitForMultipleObjects(threadCount, threads, TRUE, INFINITE);

    for (int i = 0; i < threadCount; i++) {
        CloseHandle(threads[i]);
    }
    
    // for (int i = 0; i < objects.movingPlatforms.size(); i++) {
    //     for (int j = 0; j < objects.movingPlatforms[i].faces.size(); j++) {
    //         polygonLightning(objects,objects.movingPlatforms[i],j,objects.lightSource,objects.cameraPosition);
    //         polygonRefraction(objects.movingPlatforms[i],j,objects.lightSource,objects.cameraPosition);
    //     }
    // }

    for (int j = 0; j < objects.end.faces.size(); j++) {
        polygonLightning(objects,objects.end,j,objects.lightSource,objects.cameraPosition);
        polygonRefraction(objects.end,j,objects.lightSource,objects.cameraPosition);
    }
}

/*
function was broken and ultimately scraped for the time being, currently used to filter out polygons that are completely hidden
Author: Jerry
*/
void clipPolygon(Objects& objects, Object& object, int polygon) {
    std::vector<std::array<float,3>> verts = {object.vertices[object.faces[polygon][0]], object.vertices[object.faces[polygon][1]], object.vertices[object.faces[polygon][2]]};

    std::array<int,3> insideIndices;
    std::array<int,3> outsideIndices;
    int insideCount = 0, outsideCount = 0;

    for (int i = 0; i < 3; i++) {
        float x, y, z;
        toCameraSpace(objects, verts[i], x, y, z);

        float aspect = (float)SCREEN_X / (float)SCREEN_Y;

        bool hidden = (z < NEAR_PLANE);

        if (!hidden) {
            float projectedX;
            projectedX = x * (SCREEN_Y / 2) / z;
            if (fabs(projectedX) > (SCREEN_X / 2)) {
                hidden = true;
            }
        }
        if (!hidden) {
            float projectedY;
            projectedY = y * (SCREEN_Y / 2) / z;
            if (fabs(projectedY) > (SCREEN_Y / 2)) {
                hidden = true;
            }
        }

        if (!hidden) {
            insideIndices[insideCount++] = i;
        } else {
            outsideIndices[outsideCount++] = i;
        }
    }

    if (insideCount == 0) {
        return;
    } else { // if (insideCount == 3) {
        int baseIndex = object.tempVertices.size();
        for (int i = 0; i < 3; i++) {
            object.tempVertices.push_back(verts[i]);
        }
        object.tempFaces.push_back({baseIndex, baseIndex+1, baseIndex+2});
        object.tempFaceColors.push_back(object.faceColors[polygon]);
        return;
    }

    std::array<float,3> newV[2];

    if (insideCount == 1) {
        int i0 = insideIndices[0];
        int o0 = outsideIndices[0];
        int o1 = outsideIndices[1];

        newV[0] = interpolateVertex(verts[i0], verts[o0], objects);
        newV[1] = interpolateVertex(verts[i0], verts[o1], objects);

        int baseIndex = object.tempVertices.size();
        object.tempVertices.push_back(verts[i0]);
        object.tempVertices.push_back(newV[0]);
        object.tempVertices.push_back(newV[1]);
        object.tempFaces.push_back({baseIndex, baseIndex+1, baseIndex+2});
        object.tempFaceColors.push_back(object.faceColors[polygon]);

    } else if (insideCount == 2) {
        int i0 = insideIndices[0];
        int i1 = insideIndices[1];
        int o0 = outsideIndices[0];

        newV[0] = interpolateVertex(verts[i0], verts[o0], objects);
        newV[1] = interpolateVertex(verts[i1], verts[o0], objects);

        int baseIndex = object.tempVertices.size();

        object.tempVertices.push_back(verts[i0]);
        object.tempVertices.push_back(verts[i1]);
        object.tempVertices.push_back(newV[0]);
        object.tempVertices.push_back(newV[1]);
        object.tempFaces.push_back({baseIndex, baseIndex+1, baseIndex+2});
        object.tempFaceColors.push_back(object.faceColors[polygon]);

        object.tempFaces.push_back({baseIndex+1, baseIndex+3, baseIndex+2});
        object.tempFaceColors.push_back(object.faceColors[polygon]);
    }
}

/*
polygon clipping was ultimatedly cut, so this function is currently an overcomplicated way of filtering hidden polygons for all objects
Author: Jerry
*/
void clipAll(Container& container) {
    for (int i = 0; i < container.objects.platforms.size(); i++) {
        container.objects.platforms[i].tempVertices.clear();
        container.objects.platforms[i].tempFaces.clear();
        container.objects.platforms[i].tempFaceColors.clear();
    }
    for (int i = 0; i < container.objects.water.size(); i++) {
        container.objects.water[i].tempVertices.clear();
        container.objects.water[i].tempFaces.clear();
        container.objects.water[i].tempFaceColors.clear();
    }
    container.objects.end.tempVertices.clear();
    container.objects.end.tempFaces.clear();
    container.objects.end.tempFaceColors.clear();

    const int numThreads = 16;

    HANDLE threads[64];
    ClipThreadTask tasks[64];
    int tCount = 0;

    Objects& obj = container.objects;

    int count = obj.platforms.size();
    int perThread = (count + numThreads - 1) / numThreads;

    for (int t = 0; t < numThreads; t++) {
        int start = t * perThread;
        int end   = (t + 1) * perThread;

        if (start >= count) {
            break;
        }
        if (end > count) {
            end = count;
        }

        tasks[tCount].objects = &obj;
        tasks[tCount].startObject = start;
        tasks[tCount].endObject = end;
        tasks[tCount].mode = 0;

        threads[tCount] = CreateThread(NULL, 0, clipWorker, &tasks[tCount], 0, NULL);

        tCount++;
    }

    tasks[tCount].objects = &obj;
    tasks[tCount].startObject = 0;
    tasks[tCount].endObject = 1;
    tasks[tCount].mode = 1;

    threads[tCount] = CreateThread(NULL, 0, clipWorker, &tasks[tCount], 0, NULL);

    tCount++;

    int waterCount = obj.water.size();
    perThread = (waterCount + numThreads - 1) / numThreads;

    for (int t = 0; t < numThreads; t++) {
        int start = t * perThread;
        int end = (t + 1) * perThread;

        if (start >= waterCount) {
            break;
        }
        if (end > waterCount) {
            end = waterCount;
        }

        tasks[tCount].objects = &obj;
        tasks[tCount].startObject = start;
        tasks[tCount].endObject = end;
        tasks[tCount].mode = 2;

        threads[tCount] = CreateThread(NULL,0,clipWorker,&tasks[tCount],0,NULL);

        tCount++;
    }

    WaitForMultipleObjects(tCount, threads, TRUE, INFINITE);

    for (int i = 0; i < tCount; i++) {
        CloseHandle(threads[i]);
    }
}

/*
projects a vertex from 3d space to 2d coordinates
purpose is to aid in turning 3d objects into drawable 2d coordinates
Author: Jerry
*/

void project(const struct Objects& objects, const std::vector<std::array<float,3>>& vertices, struct Screen& screen, int vertex) {
    std::array<float,2> depthResults = depth(objects,vertices[vertex]);
    std::array<float,2> xResults = fieldOfViewBoundSide(objects,vertices[vertex]);
    std::array<float,2> yResults = fieldOfViewBoundUp(objects,vertices[vertex]);

    float hidden = 0;
    if (depthResults[1] > 0.5) { // || xResults[1] > 0.5 || yResults[1] > 0.5) {
        hidden = 1;
    }
    int x = round(xResults[0] + SCREEN_X / 2);
    int y = round(-yResults[0] + SCREEN_Y / 2);
    float depth = depthResults[0];

    std::array<float,4> screenVertex = {x,y,depth,hidden};
    screen.vertices.push_back(screenVertex);
}

/*
almost the exact same as the previous function, except used for projecting and organizing the particle effects
Author: Jerry
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
projects all objects from 3d to 2d in separate threads to run them in parallel so it isn't as slow
purpose is to convert all objects to 2d data that can be drawn on the screen
Author: Jerry
*/

void projectAll(Container& container) {
    container.screen.vertices.clear();
    container.screen.effects.clear();
    container.screen.faces.clear();
    container.screen.faceColors.clear();
    
    const int numThreads = 16;
    HANDLE threads[numThreads];
    ProjectThreadTask tasks[numThreads];

    int totalObjects = container.objects.platforms.size();
    int perThread = (totalObjects + numThreads - 1) / numThreads;
    int threadIndex = 0;

    std::vector<Object> allObjects = container.objects.platforms;

    for (int t = 0; t < numThreads; t++) {
        int start = t * perThread;
        int end = (start + perThread);
        if (start >= allObjects.size()) {
            break;
        }
        if (end > allObjects.size()) {
            end = allObjects.size();
        }

        tasks[threadIndex].objects = &container.objects;
        tasks[threadIndex].objectList = &allObjects;
        tasks[threadIndex].startIndex = start;
        tasks[threadIndex].endIndex = end;

        threads[threadIndex] = CreateThread(NULL,0,projectWorker,&tasks[threadIndex],0,NULL);
        threadIndex++;
    }

    totalObjects = container.objects.water.size();
    perThread = (totalObjects + numThreads - 1) / numThreads;

    std::vector<Object> allWater = container.objects.water;

    for (int t = 0; t < numThreads; t++) {
        int start = t * perThread;
        int end = (start + perThread);
        if (start >= allWater.size()) {
            break;
        }
        if (end > allWater.size()) {
            end = allWater.size();
        }

        tasks[threadIndex].objects = &container.objects;
        tasks[threadIndex].objectList = &allWater;
        tasks[threadIndex].startIndex = start;
        tasks[threadIndex].endIndex = end;

        threads[threadIndex] = CreateThread(NULL,0,projectWorker,&tasks[threadIndex],0,NULL);
        threadIndex++;
    }

    WaitForMultipleObjects(threadIndex, threads, TRUE, INFINITE);

    for (int t = 0; t < threadIndex; t++) {
        int baseIndex = container.screen.vertices.size();
        for (int v = 0; v < tasks[t].projectedVertices.size(); v++) {
            container.screen.vertices.push_back(tasks[t].projectedVertices[v]);
        }
        for (int f = 0; f < tasks[t].projectedFaces.size(); f++) {
            std::array<int,3> face = tasks[t].projectedFaces[f];
            for (int k = 0; k < 3; k++) {
                face[k] += baseIndex;
            }
            container.screen.faces.push_back(face);
            container.screen.faceColors.push_back(tasks[t].projectedFaceColors[f]);
        }
    }

    int endBase = container.screen.vertices.size();
    for (int j = 0; j < container.objects.end.tempVertices.size(); j++) {
        project(container.objects, container.objects.end.tempVertices, container.screen, j);
    }
    for (int j = 0; j < container.objects.end.tempFaces.size(); j++) {
        if (container.screen.vertices[container.objects.end.tempFaces[j][0] + endBase][3] < 0.5 || container.screen.vertices[container.objects.end.tempFaces[j][1] + endBase][3] < 0.5 || container.screen.vertices[container.objects.end.tempFaces[j][2] + endBase][3] < 0.5) {
            std::array<int,3> face = container.objects.end.tempFaces[j];
            for (int k = 0; k < 3; k++) {
                face[k] += endBase;
            }
            container.screen.faces.push_back(face);
            container.screen.faceColors.push_back(container.objects.end.tempFaceColors[j]);
        }
        // if (!(container.screen.vertices[container.objects.end.tempFaces[j][0] + endBase][3] > 0.5 || container.screen.vertices[container.objects.end.tempFaces[j][1] + endBase][3] > 0.5 || container.screen.vertices[container.objects.end.tempFaces[j][2] + endBase][3] > 0.5)) {
        //     std::array<int,3> face = container.objects.end.tempFaces[j];
        //     for (int k = 0; k < 3; k++) {
        //         face[k] += endBase;
        //     }
        //     container.screen.faces.push_back(face);
        //     container.screen.faceColors.push_back(container.objects.end.tempFaceColors[j]);
        // }
    }

    for (int j = 0; j < container.objects.snow.size(); j++) {
        projectEffects(container.objects, container.objects.snow, container.screen, j);
    }

    for (int t = 0; t < threadIndex; t++) {
        CloseHandle(threads[t]);
    }

    // for (int i = 0; i < container.objects.movingPlatforms.size(); i++) {
    //     int size = container.screen.vertices.size();
    //     for (int j = 0; j < container.objects.movingPlatforms[i].tempVertices.size(); j++) {
    //         project(container.objects,container.objects.movingPlatforms[i].tempVertices,container.screen,j);
    //     }
    //     for (int j = 0; j < container.objects.movingPlatforms[i].tempFaces.size(); j++) {
    //         if (!(container.screen.vertices[container.objects.movingPlatforms[i].tempFaces[j][0] + size][3] > 0.5 || container.screen.vertices[container.objects.movingPlatforms[i].tempFaces[j][1] + size][3] > 0.5 || container.screen.vertices[container.objects.movingPlatforms[i].tempFaces[j][2] + size][3] > 0.5)) {
    //             std::array<int,3> face = container.objects.movingPlatforms[i].tempFaces[j], faceColor = container.objects.movingPlatforms[i].tempFaceColors[j];
    //             for (int k = 0; k < 3; k++) {
    //                 face[k] += size;
    //             }
    //             container.screen.faces.push_back(face);
    //             container.screen.faceColors.push_back(faceColor);
    //         }
    //     }
    // }
}

/*
iterates through a box of pixels containing a polygon and checks if the pixel is inside the polygon, drawing it if it is and if the calculated depth is less than the previously stored depth at the pixel
purpose is to use the data provided by the projection step to determine the color of a group of pixels
Author: Jerry
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
the same function as above, but for coloring the snow particles. does not have z buffering as the snow particles are translucent
Author: Jerry
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
iterates through every polygon and creates a 2d array of every pixel color on the screen
purpose is to convert the data from the projection step into usable information that can be directly drawn on the screen
Author: Jerry
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
iterates through the 2d pixel array to identify contiguous horizontal lines of the same color, provided that the pixel color has changed from the previous frame
purpose is to turn the pixel array into data that can be put directly into a draw line function, as draw line is much faster than draw pixel
Author: Jerry
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
iterates through the lines of pixels and draws each line
purpose is the draw the current frame on the screen
Author: Jerry
*/

void drawScreen(Container& container) {
    for (int i = 0; i < container.screen.lines.size(); i++) {
        LCD.SetFontColor(container.screen.lines[i].color);
        LCD.DrawHorizontalLine(container.screen.lines[i].y,container.screen.lines[i].x1,container.screen.lines[i].x2 + 1);
    }
}

/*
shows the fps in the corner of the screen
purpose is to aid in optimization and provide performance info to the user
Author: Jerry
*/
void showOverlay(Container& container) {
    LCD.SetFontColor(GRAY);
    LCD.FillRectangle(255,5,55,24);
    LCD.SetFontColor(DARKGRAY);
    LCD.FillRectangle(257,7,51,20);
    LCD.SetFontColor(MIDNIGHTBLUE);
    LCD.SetFontScale(0.5);
    LCD.WriteAt("FPS:",260,10);
    LCD.WriteAt(container.states.gameStates.fps,290,10);

    // LCD.SetFontColor(BLACK);
    // LCD.FillRectangle(0,SCREEN_Y - 40,SCREEN_X,40);
}

/*
Master rendering function, finally
calls all previous functions and stores them in a single function that can be called in the game loop
Author: Jerry
*/

void render(Container& container) {
    handleLighting(container.objects);
    clipAll(container);
    projectAll(container);
    colorAll(container);
    findLines(container);
    drawScreen(container);
    showOverlay(container);
    LCD.Update();
}