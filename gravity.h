#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "Matrix3.h"
#include "cube.h"

class Gravity
{
private:
	Vector3 m_gravity;

public:
	Gravity(Vector3 &i_gravity);
	Gravity();
	virtual void updateForce(Cube *i_body, float i_duration);

};