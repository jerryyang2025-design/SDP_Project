#pragma once

#include "data.h"

void applyGravity(Container& container, int time);

void applyDrag(Container& container);

void handlePhysics(Container& container);

void movePlayer(Container& container, std::array<float,3> movement);

void handleCollision(Container& container, int step);