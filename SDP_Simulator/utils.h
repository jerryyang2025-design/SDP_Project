#pragma once

float clamp(float value, float min, float max);

float dotProduct(std::array<float,3> a,std::array<float,3> b);

std::array<float,3> crossProduct(std::array<float,3> a,std::array<float,3> b);

float pythag(float x, float y);

float distance(float x, float y, float z);

float direction(std::array<float,3> a,std::array<float,3> b);

float angle(std::array<float,3> a,std::array<float,3> b);

float magnitudeInDirection(std::array<float,3> a,std::array<float,3> b);

std::array<float,2> depth(struct Objects objects, std::array<float,3> vertex);

float depthUp(struct Objects objects, std::array<float,3> vertex);

float depthSide(struct Objects objects, std::array<float,3> vertex);

std::array<float,2> fieldOfViewBoundSide(struct Objects objects, std::array<float,3> vertex);

std::array<float,2> fieldOfViewBoundUp(struct Objects objects, std::array<float,3> vertex);