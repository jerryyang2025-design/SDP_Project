/*
    File contains all utility functions. Utility functions perform a calculation that can used within any other function.
*/

#include <math.h>
#include <sstream>
#include "header_files/utils.h"
#include "header_files/data.h"

#define EPSILON 1e-6
#define NEAR_PLANE 1.0f

float clamp(float value, float min, float max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    }
    return value;
}

float dotProduct(std::array<float,3> a,std::array<float,3> b) {
    // Calculates and returns the dot product
    return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

std::array<float,3> crossProduct(std::array<float,3> a,std::array<float,3> b) {
    // Calculates and sets vector c to the cross product
    std::array<float,3> c;
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
    return c;
}

float pythag(float x, float y) {
    // Return the Euclidean distance (hypotenuse) for a right triangle with sides x and y.
    return sqrt(x*x + y*y);
}

float distance(float x, float y, float z) {
    // Literally pythag in 3d
    return sqrt(x*x + y*y + z*z);
}

float direction(std::array<float,3> a,std::array<float,3> b) {
    float magnitudeOne = distance(a[0],a[1],a[2]);
    float magnitudeTwo = distance(b[0],b[1],b[2]);
    float dot = dotProduct(a,b);
    if (fabs(magnitudeOne) < EPSILON || fabs(magnitudeTwo) < EPSILON) {
        return 0;
    }
    return dot / (magnitudeOne * magnitudeTwo);
}

float angle(std::array<float,3> a,std::array<float,3> b) {
    return acos(clamp(direction(a, b),-1,1));
}

float magnitudeInDirection(std::array<float,3> a,std::array<float,3> b) {
    float magnitude = distance(a[0],a[1],a[2]);
    float dot = dotProduct(a,b);
    if (fabs(magnitude) < EPSILON) {
        return dot;
    }
    return dot / (magnitude);
}

std::array<float,2> depth(const struct Objects& objects, std::array<float,3> vertex) {
    std::array<float,3> toCameraVector;

    for (int i = 0; i < 3; i++) {
        toCameraVector[i] = -objects.cameraPosition[i] + vertex[i];
    }
    float objectDepth = magnitudeInDirection(objects.cameraVector,toCameraVector);
    float hidden = 0;
    if (objectDepth <= NEAR_PLANE) {
        hidden = 1; // 0 for not hidden, 1 for hidden
    }
    return {objectDepth,hidden};
}

float depthUp(const struct Objects& objects, std::array<float,3> vertex) {
    std::array<float,3> toCameraVector;

    for (int i = 0; i < 3; i++) {
        toCameraVector[i] = -objects.cameraPosition[i] + vertex[i];
    }
    float objectDepth = magnitudeInDirection(objects.cameraUpVector,toCameraVector);
    return objectDepth;
}

float depthSide(const struct Objects& objects, std::array<float,3> vertex) {
    std::array<float,3> toCameraVector;

    for (int i = 0; i < 3; i++) {
        toCameraVector[i] = -objects.cameraPosition[i] + vertex[i];
    }
    float objectDepth = magnitudeInDirection(objects.cameraRightVector,toCameraVector);
    return objectDepth;
}

std::array<float,2> fieldOfViewBoundSide(const struct Objects& objects, std::array<float,3> vertex) {
    float x = depthSide(objects,vertex);
    std::array<float,2> results = depth(objects,vertex);
    float z = results[0];
    float hidden = 0;
    float projectedX;
    if (z < NEAR_PLANE) {
        projectedX = x * (SCREEN_Y / 2);
        hidden = 1;
    } else {
        projectedX = x * (SCREEN_Y / 2) / z;
    }
    if (fabs(projectedX) > (SCREEN_X / 2)) {
        hidden = 1;
    }
    return {projectedX,hidden};
}

std::array<float,2> fieldOfViewBoundUp(const struct Objects& objects, std::array<float,3> vertex) {
    float y = depthUp(objects,vertex);
    std::array<float,2> results = depth(objects,vertex);
    float z = results[0];
    float hidden = 0;
    float projectedY;
    if (z < NEAR_PLANE) {
        projectedY = y * (SCREEN_Y / 2);
        hidden = 1;
    } else {
        projectedY = y * (SCREEN_Y / 2) / z;
    }
    if (fabs(projectedY) > (SCREEN_Y / 2)) {
        hidden = 1;
    }
    return {projectedY,hidden};
}

float fragDepth(const std::array<std::array<float,4>,3>& polygon, const std::array<int,2>& point) {
    float coordinateOne = (polygon[1][1] - polygon[2][1]) * (point[0] - polygon[2][0]) + (polygon[2][0] - polygon[1][0]) * (point[1] - polygon[2][1]);
    coordinateOne /= ((polygon[1][1] - polygon[2][1]) * (polygon[0][0] - polygon[2][0]) + (polygon[2][0] - polygon[1][0]) * (polygon[0][1] - polygon[2][1]));
    
    float coordinateTwo = (polygon[2][1] - polygon[0][1]) * (point[0] - polygon[2][0]) + (polygon[0][0] - polygon[2][0]) * (point[1] - polygon[2][1]);
    coordinateTwo /= ((polygon[1][1] - polygon[2][1]) * (polygon[0][0] - polygon[2][0]) + (polygon[2][0] - polygon[1][0]) * (polygon[0][1] - polygon[2][1]));
    
    float coordinateThree = 1 - coordinateOne - coordinateTwo;
    return coordinateOne * polygon[0][2] + coordinateTwo * polygon[1][2] + coordinateThree * polygon[2][2];
}

int cross2d(const std::array<int,2>& v, const std::array<int,2>& w) {
    return v[0] * w[1] - v[1] * w[0];
}

bool sameSide(const std::array<int,2>& p1, const std::array<int,2>& p2, const std::array<int,2>& A, const std::array<int,2>& B) {
    std::array<int, 2> AB {B[0] - A[0], B[1] - A[1]};
    std::array<int, 2> AP1 {p1[0] - A[0], p1[1] - A[1]};
    std::array<int, 2> AP2 {p2[0] - A[0], p2[1] - A[1]};
    return cross2d(AB, AP1) * cross2d(AB, AP2) >= 0;
}

bool pointInTriangle(const std::array<std::array<float,4>,3>& polygon, const std::array<int,2>& point) {
    std::array<int,2> A = {polygon[0][0], polygon[0][1]};
    std::array<int,2> B = {polygon[1][0], polygon[1][1]};
    std::array<int,2> C = {polygon[2][0], polygon[2][1]};
    return sameSide(point, C, A, B) && sameSide(point, A, B, C) && sameSide(point, B, C, A);
}

unsigned int rgbToHex(int r, int g, int b) {
    return (r << 16) | (g << 8) | b;
}

bool equals(const std::array<int,3>& color1, const std::array<int,3>& color2) {
    return color1[0] == color2[0] && color1[1] == color2[1] && color1[2] == color2[2];
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> parts;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim)) {
        parts.push_back(item);
    }

    return parts;
}

std::array<std::array<float,3>,9> generateHitbox(std::array<float,3> center, float scale) { // test heavily
    std::array<std::array<float,3>,9> temp;
    int index = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            std::array<float,3> vertex = {center[0] + j * scale,center[1] - PLAYER_HEIGHT,center[2] + i * scale};
            temp[index] = vertex;
            index++;
        }
    }
    return temp;
}

int manageFPS(Container& container, int time) {
    float desiredTime = 1000.0 / FPS;
    int sleepTime = clamp(round(desiredTime - time),0,1000);
    container.states.gameStates.fps = 1000 / (sleepTime + time);
    container.states.gameStates.totalTime += sleepTime + time;
    return sleepTime;
}

/*Takes an array of spherical coords an returns an array in cartesian. Author: Nigel*/
std::array<float,3> sphericalToCartesian(const std::array<float,3>& rThetaPhi) {
    std::array<float,3> xyz; // {x, y, z}
    xyz[0] = rThetaPhi[0] * cos(rThetaPhi[2]) * sin(rThetaPhi[1]);
    xyz[1] = rThetaPhi[0] * sin(rThetaPhi[2]);
    xyz[2] = rThetaPhi[0] * cos(rThetaPhi[2]) * cos(rThetaPhi[1]);
    return xyz;
}
/*Takes an array of cartesian coordinates and returns an array in spherical. Author: Nigel*/
std::array<float,3> cartesianToSpherical(const std::array<float,3>& xyz) {
    // converts an array of cartesian coords to spherical coords in radians.
    std::array<float,3> rThetaPhi; // {r, theta, phi}
    rThetaPhi[0] = distance(xyz[0],xyz[1],xyz[2]);
    rThetaPhi[1] = atan2(xyz[0],xyz[2]);
    rThetaPhi[2] = atan2(xyz[1],pythag(xyz[0],xyz[2]));
    return rThetaPhi;
}

void normalize(std::array<float,3>& vector) {
    float magnitude = distance(vector[0],vector[1],vector[2]);
    for (int i = 0; i < 3; i++) {
        vector[i] /= magnitude;
    } 
}

void toCameraSpace(const Objects& objects, const std::array<float,3>& v, float& x, float& y, float& z) {
    x = depthSide(objects, v);
    y = depthUp(objects, v);
    z = depth(objects, v)[0];
}

/*
currently unused since clipping was cut from the project
*/
std::array<float,3> interpolateVertex(const std::array<float,3>& vInside, const std::array<float,3>& vOutside, const Objects& objects) {
    float xIn, yIn, zIn;
    float xOut, yOut, zOut;

    toCameraSpace(objects, vInside,  xIn,  yIn,  zIn);
    toCameraSpace(objects, vOutside, xOut, yOut, zOut);

    float t = 1.0f;

    if (zIn >= NEAR_PLANE && zOut < NEAR_PLANE) {
        float tZ = (NEAR_PLANE - zIn) / (zOut - zIn);
        if (tZ < t) {
            t = tZ;
        }
    }

    float aspect = (float)SCREEN_X / (float)SCREEN_Y;

    if (xIn >= -zIn * aspect && xOut < -zOut * aspect) {
        float tX = (-aspect * zIn - xIn) / ((xOut - xIn) - aspect * (zOut - zIn));
        if (tX < t) {
            t = tX;
        }
    }

    if (xIn <= zIn * aspect && xOut > zOut * aspect) {
        float tX = (aspect * zIn - xIn) / ((xOut - xIn) - aspect * (zOut - zIn));
        if (tX < t) {
            t = tX;
        }
    }

    if (yIn >= -zIn && yOut < -zOut) {
        float tY = (-zIn - yIn) / ((yOut - yIn) - (zOut - zIn));
        if (tY < t) {
            t = tY;
        }
    }

    if (yIn <= zIn && yOut > zOut) {
        float tY = (zIn - yIn) / ((yOut - yIn) - (zOut - zIn));
        if (tY < t) {
            t = tY;
        }
    }

    t = clamp(t, 0.0f, 1.0f);

    std::array<float,3> result;
    for (int i = 0; i < 3; i++) {
        result[i] = vInside[i] + t * (vOutside[i] - vInside[i]);
    }
    return result;
}