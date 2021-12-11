#pragma once

#include "V2D_Math.h"
struct Shape;

struct Body
{
	real r, g, b;
	Shape* shape;

	real M;  // mass of the body.
	real iM; //inverse of mass of the body ( 1/mass ).
	real I;  // moment of inertia ( MOI ) of the body. 
	real iI; // inverse of moment of inertia ( MOI ) of the body ( 1/Moment of Inertia )

	vec2 position;
	vec2 velocity;
	vec2 force;

	real orientation; // orientation in radians
	real angularVelocity;
	real torque;
	
	real staticFriction;
	real dynamicFriction;
	real restituition;

	Body() //default constructor
	{}
	Body(Shape* shape_, uint32 x_, uint32 y_);

	void ApplyForce(const vec2& f)
	{
		force += f;
	}
	void ApplyImpulse(const vec2& impulse, const vec2& contactVector)
	{
		velocity += impulse * iM;
		angularVelocity += iI * Cross(impulse, contactVector);
	}
	void SetStatic()
	{
		M = 0.0f;
		iM = 0.0f;
		I = 0.0f;
		iI = 0.0f;
	}
	void SetOrient(real radians);
	
};