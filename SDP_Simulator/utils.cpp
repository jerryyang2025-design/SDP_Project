#include <math.h>
#include "utils.h"
#include "data.h"

#define EPSILON 1e-6

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
        toCameraVector[i] = objects.cameraPosition[i] - vertex[i];
    }
    float objectDepth = magnitudeInDirection(objects.cameraVector,toCameraVector);
    float hidden = 0;
    if (objectDepth < 0) {
        hidden = 1; // 0 for not hidden, 1 for hidden
    }
    return {objectDepth,hidden};
}

float depthUp(const struct Objects& objects, std::array<float,3> vertex) {
    std::array<float,3> toCameraVector;

    for (int i = 0; i < 3; i++) {
        toCameraVector[i] = objects.cameraPosition[i] - vertex[i];
    }
    float objectDepth = magnitudeInDirection(objects.cameraUpVector,toCameraVector);
    return objectDepth;
}

float depthSide(const struct Objects& objects, std::array<float,3> vertex) {
    std::array<float,3> toCameraVector;

    for (int i = 0; i < 3; i++) {
        toCameraVector[i] = objects.cameraPosition[i] - vertex[i];
    }
    float objectDepth = magnitudeInDirection(objects.cameraRightVector,toCameraVector);
    return objectDepth;
}

std::array<float,2> fieldOfViewBoundSide(const struct Objects& objects, std::array<float,3> vertex) {
    float x = depthSide(objects,vertex);
    std::array<float,2> results = depth(objects,vertex);
    float z = results[0];
    float screenAtZ = SCREEN_X / SCREEN_Y * z;
    float hidden = results[1];
    if (hidden < 0.5) { // checks if it's already hidden
        if (fabs(x) > screenAtZ) {
            hidden = 1;
        }
    }
    if (fabs(screenAtZ) < EPSILON) {
        return {x * (SCREEN_Y / 2),hidden};
    }
    float projectedX = x * (SCREEN_X / 2) / screenAtZ;
    return {projectedX,hidden};
}

std::array<float,2> fieldOfViewBoundUp(const struct Objects& objects, std::array<float,3> vertex) {
    float y = depthUp(objects,vertex);
    std::array<float,2> results = depth(objects,vertex);
    float z = results[0];
    float screenAtZ = z;
    float hidden = results[1];
    if (hidden < 0.5) { // checks if it's already hidden
        if (fabs(y) > screenAtZ) {
            hidden = 1;
        }
    }
    if (fabs(screenAtZ) < EPSILON) {
        return {y * (SCREEN_Y / 2),hidden};
    }
    float projectedY = y * (SCREEN_Y / 2) / screenAtZ;
    return {projectedY,hidden};
}