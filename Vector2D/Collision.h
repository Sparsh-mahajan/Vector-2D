#pragma once

#include "Shape.h"
struct Manifold;
struct Body;

using CollisionCallback = void(*)(Manifold* m, Body* a, Body* b);

void CircleToCircle(Manifold* m, Body* a, Body* b);
void CircleToPolygon(Manifold* m, Body* a, Body* b);
void PolygonToCircle(Manifold* m, Body* a, Body* b);
void PolygonToPolygon(Manifold* m, Body* a, Body* b);
