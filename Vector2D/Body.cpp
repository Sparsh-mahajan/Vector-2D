#include "Body.h"
#include "Shape.h"
Body::Body(Shape* shape_, uint32 x_, uint32 y_) : shape(shape_)
{
	shape->body = this;
	position.Set((real)x_, (real)y_);
	velocity.Set(0.0f, 0.0f);
	angularVelocity = 0.0f;
	torque = 0.0f;
	orientation = Random(-PI, PI);
	staticFriction = 0.5f;
	dynamicFriction = 0.3f;
	restituition = 0.2f;
	shape->Initialize();
	r = Random(0.2f, 1.0f);
	g = Random(0.2f, 1.0f);
	b = Random(0.2f, 1.0f);
}
void Body::SetOrient(real radians)
{
	orientation = radians;
	shape->SetOrient(orientation);
}