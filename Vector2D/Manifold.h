#pragma once

#include "Body.h"
#include<vector>

struct Body;

struct Manifold
{
	Body* A;
	Body* B;
	real penetration;
	vec2 normal;	// vector normal to 
	vec2 contacts[2];
	uint32 contactCount;
	real e;			//restituition for the collision between the bodies A and B.
	real sf;		//static friction between the bodies A and B.
	real df;		//dynamic friction between the bodies A and B.
};