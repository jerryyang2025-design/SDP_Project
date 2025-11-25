#include <math.h>
#include "data.h"
#include "utils.h"

#define BRIGHTNESS 3000000

float polygonLightning(struct Object object, int polygon, std::array<float,3> lightSource) {
    float center[3],vectorOne[3],vectorTwo[3],normalVector[3],toLightVector[3],toCameraVector[3],sortaHalfVector[3];
    std::vector<std::array<float,3>> vertices = {object.vertices[object.faces[polygon][0]],
        object.vertices[object.faces[polygon][1]], 
        object.vertices[object.faces[polygon][2]]};

    for (int i = 0; i < 3; i++) {
        center[i] = (vertices[0][i] + vertices[1][i] + vertices[2][i]) / 3;
        vectorOne[i] = vertices[1][i] - vertices[0][i];
        vectorTwo[i] = vertices[2][i] - vertices[0][i];
        toLightVector[i] = lightSource[i] - center[i];
        toCameraVector[i] = -center[i]; // change to camera point
    }

    crossProduct(vectorOne,vectorTwo,normalVector);

    float dir = direction(normalVector,toLightVector) + 1.5; // 1.5 to prevent it from going negative, so that direction still matters and to avoid using max(). MAYBE ADJUST
    float dist = distance(center[0] - lightSource[0],center[1] - lightSource[1],center[2] - lightSource[2]);

    float magnitudeOne = distance(toLightVector[0],toLightVector[1],toLightVector[2]);
    float magnitudeTwo = distance(toCameraVector[0],toCameraVector[1],toCameraVector[2]);
    for (int i = 0; i < 3; i++) {
        toLightVector[i] /= magnitudeOne;
        toCameraVector[i] /= magnitudeTwo;
        sortaHalfVector[i] = toLightVector[i] + toCameraVector[i];
    }

    float theta = angle(sortaHalfVector, normalVector);

    float shine;
    if (dotProduct(toLightVector,normalVector) > 0) {
        shine = object.reflectionValue / (theta * theta + 1) + 1; // maybe change the squared part, that's to make sure it goes down quickly
    } else {
        shine = 1;
    }

    float lightValueMultiplier = BRIGHTNESS * dir * shine / (dist * dist);
    return lightValueMultiplier;
}

void polygonRefraction(struct Object& object, int polygon, std::array<float,3> lightSource) {
    float center[3],vectorOne[3],vectorTwo[3],normalVector[3],halfVector[3];
    std::vector<std::array<float,3>> vertices = {object.vertices[object.faces[polygon][0]],
        object.vertices[object.faces[polygon][1]], 
        object.vertices[object.faces[polygon][2]]};
    
    for (int i = 0; i < 3; i++) {
        center[i] = (vertices[0][i] + vertices[1][i] + vertices[2][i]) / 3;
        vectorOne[i] = vertices[1][i] - vertices[0][i];
        vectorTwo[i] = vertices[2][i] - vertices[0][i];
        halfVector[i] = lightSource[i] - 2 * center[i];
    }

    crossProduct(vectorOne,vectorTwo,normalVector);

    float shift = direction(normalVector,halfVector);

    shift = pow(1 - clamp(shift,0,1), 0.5);

    float colorShift[3] = {0.5 + 0.5 * sin(shift * 6.2831 + 0.0), 
        0.5 + 0.5 * sin(shift * 6.2831 + 2.0), 
        0.5 + 0.5 * sin(shift * 6.2831 + 4.0)};

    for (int i = 0; i < 3; i++) {
        object.faceColors[polygon][i] = clamp(object.faceColors[polygon][i] + colorShift[i] * object.refractionValue,0,255); // maybe change to use default object color, depending on when lighting is called
    }
}

double waveMovement(std::array<float,3> vertex, long frames) {
    return sin(frames * 0.25 + vertex[0] + vertex[2]); // maybe adjust depending on framerate
}

def project(point,scale):
    """Project a single coordinate to 2D based on perspective and scaling, or return None if invalid."""
    projected_point = bound_TR(float(scale))
    if projected_point is None:
        return None
    elif abs(projected_point) < 0.1:
        // Don't draw the line if it will result in a divide by 0 or near 0 error
        return None
    // Calculates the screen coordinate position based on the screen size and FOV bound size its true coordinate position (the last coordinate value)
    return data.display.half_side * float(point) / float(projected_point)

def project_to_2d(point_1,point_2):
    """Project two N-dimensional points to 2D coordinates for rendering, or return None if not visible."""
    position_1, position_2 = point_1[:], point_2[:]
    // Continuously projects down a dimension until it reaches 2D
    while len(position_1) > 2:
        for i in range(len(position_1) - 1):
            position_1[i] = project(position_1[i],position_1[-1])
            position_2[i] = project(position_2[i],position_2[-1])
            if position_1[i] is None or position_2[i] is None:
                return None
        position_1.pop()
        position_2.pop()
    return (position_1,position_2)