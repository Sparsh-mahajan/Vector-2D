#pragma once

#include "Body.h"
#include <GLFW/glfw3.h>

#define MaxPolyVertexCount 64

struct Shape
{
	Body* body;
	enum class Type
	{
		ePoly,
		eCircle,
		eCount
	};
	real radius; // for circle shape.
	mat22 orientMatrix; // orientation matrix for polygon shape.

	virtual Shape* Clone() const = 0;
	virtual void Initialize() = 0;
	virtual void ComputeMass(real density) = 0;
	virtual void SetOrient(real radians) = 0;
	virtual void Draw() const = 0;
	virtual Type GetType() const = 0;
};

struct Circle : public Shape
{
	Circle(real r)
	{
		radius = r;
	}
	Circle* Clone() const override
	{
		return new Circle(radius); // reminder: implement use of std::shared_ptr to optimise usage of memory.
	}
	void Initialize() override
	{
		ComputeMass(1.0f);
	}
	void ComputeMass(real density) override
	{
		body->M = PI * radius * radius * density;
		body->iM = (body->M) ? (1.0f / body->M) : 0.0f; // if Mass of body != 0, assign inverse mass 1/Mass otherwise assign 0
		body->I = body->M * radius * radius;
		body->iI = (body->I) ? (1.0f / body->I) : 0.0f; // if MOI of body != 0, assign inverse mass 1/MOI otherwise assign 0
	}
	void SetOrient(real radians) override
	{
		// Circle does not need any anngular orientation.
	}
	void Draw() const override
	{
		//rendering the circle.
		const uint32 segments = 20;
		glColor3f(body->r, body->g, body->b);
		glBegin(GL_LINE_LOOP);
		real theta = body->orientation;
		real incAngle = PI * 2.0f / (real)segments;
		for (uint32 i = 0; i < segments; i++)
		{
			theta += incAngle;
 			vec2 vec(std::cos(theta), std::sin(theta));
			vec *= radius;
			vec += body->position;
			glVertex2f(vec.x, vec.y);
		}
		glEnd();

		//rendering the line. ( very very smart use of vectors :D )
		glBegin(GL_LINE_STRIP);
		vec2 lineVec(0, 1.0f); // vector that will point to the end of the line that will be rendered.
		real c = std::cos(body->orientation);
		real s = std::sin(body->orientation);
		lineVec.Set(lineVec.x * c - lineVec.y * s, lineVec.x * s + lineVec.y * c);
		lineVec *= radius;
		lineVec += body->position;
		glVertex2f(body->position.x, body->position.y);
		glVertex2f(lineVec.x, lineVec.y);
	}
	Type GetType() const override
	{
		return Type::eCircle;
	}
};

struct Polygon : public Shape
{
	uint32 m_vertexCount;
	vec2 m_vertices[MaxPolyVertexCount];
	vec2 m_normals[MaxPolyVertexCount];

	Polygon* Clone() const override
	{
		Polygon* poly = new Polygon();
		for (uint32 i = 0; i < m_vertexCount; i++)
		{
			poly->m_vertices[i] = m_vertices[i];
			poly->m_normals[i] = m_normals[i];
		}
		poly->m_vertexCount = m_vertexCount;
		return poly;
	}
	void Initialize() override
	{
		ComputeMass(1.0f);
	}
	void ComputeMass(real density) override //needs thorough documentation.
	{
		real area = 0.0f;
		real I = 0.0f;
		real inv3 = 1.0f / 3.0f;

		for (uint32 i1 = 0; i1 < m_vertexCount; i1++)
		{
			vec2 p1 = m_vertices[i1];
			uint32 i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
			vec2 p2 = m_vertices[i2];
			real cross = Cross(p1, p2);
			real triangleArea = 0.5f * cross;
			area += triangleArea;
			real xCoord2 = p1.x * p1.x + p1.x * p2.x + p2.x * p2.x;
			real yCoord2 = p1.y * p1.y + p1.y * p2.y + p2.y * p2.y;
			I += (0.25f * inv3 * cross) * (xCoord2 + yCoord2);
		}
		body->M = density * area;
		body->iM = (body->M) ? 1.0f / body->M : 0.0f;
		body->I = I;
		body->iI = (body->I) ? 1.0f / body->I : 0.0f;
	}
	void SetOrient(real radians) override
	{
		orientMatrix.set(radians);
	}
	void Draw() const override
	{
		glColor3f(body->r, body->g, body->b);
		glBegin(GL_LINE_LOOP);
		for (uint32 i = 0; i < m_vertexCount; i++)
		{
			vec2 vec = body->position + orientMatrix * m_vertices[i];
			glVertex2f(vec.x, vec.y);
		}
		glEnd();
	}
	Type GetType() const override
	{
		return Type::ePoly;
	}
	void SetBox(real hw, real hh) //hw- half width of the base box, hh - half height of the base box.
	{
		m_vertexCount = 4;
		m_vertices[0].Set( -hw, -hh );
		m_vertices[1].Set( hw, -hh );
		m_vertices[2].Set( hw, hh) ;
		m_vertices[3].Set( -hw, hh );
		m_normals[0].Set( 0.0f, -1.0f );
		m_normals[1].Set( 1.0f, 0.0f );
		m_normals[2].Set( 0.0f, 1.0f );
		m_normals[3].Set( -1.0f, 0.0f );
	}
	void set(vec2* vertices, uint32 count)
	{
		assert(count > 2 && count < MaxPolyVertexCount);
		count = std::min((int32)count, MaxPolyVertexCount);
		real highestXCoordinate = 0.0f;
		int32 rightMost = 0;
		// finding the rightmost coordinate (with the lowest y coordinate)
		for (uint32 i = 1; i < count; i++)
		{
			if (vertices[i].x > highestXCoordinate)
			{
				highestXCoordinate = vertices[i].x;
				rightMost = i;
			}
			else if (vertices[i].x == highestXCoordinate) // then find the vertex with the lowest y coordinate.
			{
				if (vertices[i].y < vertices[rightMost].y)
					rightMost = i;
			}
		}
		int32 hull[MaxPolyVertexCount];
		int32 outCount = 0;
		int32 indexHull = rightMost;
		for ( ; ; ) // loop runs until all the vertices have been sorted into an array in an order that will form a convex polygon.
		{			
			//this loop starts with the rightmost vertex and finds the most counter clockwise point for that vertex.
			// we find the most counter clockwise point by using the cross product.
			// the most counter clockwise point becomes the nextHullIndex and again the most counter clockwise point is calculated for this vertex.
			// this is done until all the vertices are sorted in a clockwise manner in such a way that they form the hull of the polygon. 

			hull[outCount] = indexHull;
			int32 nextHullIndex = 0;
			for (uint32 i = 1; i < count; i++)
			{
				if (i == indexHull)
				{
					nextHullIndex = i;
					continue;
				}
				vec2 v1 = vertices[nextHullIndex] - vertices[hull[outCount]];
				vec2 v2 = vertices[i] - vertices[hull[outCount]];
				real cross = Cross(v1, v2);
				
				if (cross < 0.0f) 
					nextHullIndex = i;
				if (cross == 0.0f && v2.LenSqr() > v1.LenSqr())
					nextHullIndex = i;
			}
			indexHull = nextHullIndex;
			outCount++;
			if (nextHullIndex == rightMost)
			{
				m_vertexCount = outCount;
				break;
			}
		}
		for (uint32 i = 0; i < m_vertexCount; i++)
			m_vertices[i] = vertices[hull[i]];
		for (uint32 i1 = 0; i1 < m_vertexCount; i1++)
		{
			uint32 i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
			vec2 face = m_vertices[i2] - m_vertices[i1];
			assert(face.LenSqr() > EPSILON * EPSILON);
			m_normals[i1] = vec2(face.y, -face.x);
			m_normals[i1].Normalize();
		}
	}
	vec2 GetSupport(const vec2& dir)
	{
		real bestProjection = -FLT_MAX;
		vec2 bestVertex;
		for (uint32 i = 0; i < m_vertexCount; i++)
		{
			real dot = Dot(m_vertices[i], dir);
			if (dot > bestProjection)
			{
				bestProjection = dot;
				bestVertex = m_vertices[i];
			}
		}
		return bestVertex;
	}
};
