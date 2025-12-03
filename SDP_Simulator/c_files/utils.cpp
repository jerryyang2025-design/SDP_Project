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
        return 0;
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
    if (fabs(z) < NEAR_PLANE) {
        projectedX = x * (SCREEN_X / 2);
    } else {
        projectedX = x * (SCREEN_X / 2) / z;
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
    if (fabs(z) < NEAR_PLANE) {
        projectedY = y * (SCREEN_Y / 2);
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

int manageFPS(int time) {
    float desiredTime = 1000.0 / FPS;
    int sleepTime = clamp(round(desiredTime - time),0,1000);
    return sleepTime;
} // probably unnecessary, needs testing to be sure

std::array<float,3> sphericalToCartesian(float rho, float theta, float phi) {
    // theta is xz plane, phi is yz plane, use rho = 1
    std::array<float,3> xyz; // {x, y, z}
    xyz[0] = rho * sin(theta*M_PI/180) * cos(phi*M_PI/180);
    xyz[1] = rho * sin(phi*M_PI/180);
    xyz[2] = rho * cos(theta*M_PI/180) * cos(phi*M_PI/180);
    return xyz;
    // Needs testing
}