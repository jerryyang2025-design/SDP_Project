#include <math.h>
#include "utils.h"

float clamp(float value, float min, float max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    }
    return value;
}

float dotProduct(float a[],float b[]) {
    // Calculates and returns the dot product
    return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

void crossProduct(float a[],float b[],float c[]) {
    // Calculates and sets vector c to the cross product
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

float pythag(float x, float y) {
    // Return the Euclidean distance (hypotenuse) for a right triangle with sides x and y.
    return sqrt(x*x + y*y);
}

float distance(float x, float y, float z) {
    // Literally pythag in 3d
    return sqrt(x*x + y*y + z*z);
}

float direction(float a[],float b[]) {
    float magnitudeOne = pythag(pythag(a[0],a[1]),a[2]);
    float magnitudeTwo = pythag(pythag(b[0],b[1]),b[2]);
    float dot = dotProduct(a,b);
    return dot / (magnitudeOne * magnitudeTwo);
}

float angle(float a[],float b[]) {
    return acos(direction(a, b));
}

float fieldOfViewBoundX(float z) {
    if (z >= data.wireframe.side + data.display.side / data.states.perspective) {
        return none;
    }
    return (z + 100);
}

float fieldOfViewBoundY(float z) {
    if (z >= data.wireframe.side + data.display.side / data.states.perspective) {
        return none;
    }
    return (z + 100);
}