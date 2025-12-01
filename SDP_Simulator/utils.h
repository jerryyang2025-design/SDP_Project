#pragma once

#include <array>

float clamp(float value, float min, float max);

float dotProduct(std::array<float,3> a,std::array<float,3> b);

std::array<float,3> crossProduct(std::array<float,3> a,std::array<float,3> b);

float pythag(float x, float y);

float distance(float x, float y, float z);

float direction(std::array<float,3> a,std::array<float,3> b);

float angle(std::array<float,3> a,std::array<float,3> b);

float magnitudeInDirection(std::array<float,3> a,std::array<float,3> b);

std::array<float,2> depth(const struct Objects& objects, std::array<float,3> vertex);

float depthUp(const struct Objects& objects, std::array<float,3> vertex);

float depthSide(const struct Objects& objects, std::array<float,3> vertex);

std::array<float,2> fieldOfViewBoundSide(const struct Objects& objects, std::array<float,3> vertex);

std::array<float,2> fieldOfViewBoundUp(const struct Objects& objects, std::array<float,3> vertex);

float depth(const std::array<std::array<float,4>,3>& polygon, const std::array<int,2>& point);

int cross2d(const std::array<int,2> &v, const std::array<int,2> &w);

bool sameSide(const std::array<int,2> &p1, const std::array<int,2> &p2, const std::array<int,2> &A, const std::array<int,2> &B);

bool pointInTriangle(const std::array<std::array<float,4>,3> &polygon, const std::array<int,2> &point);

unsigned int rgbToHex(int r, int g, int b);