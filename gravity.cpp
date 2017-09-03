#pragma once
#include "gravity.h"

Gravity::Gravity(Vector3 &i_gravity)
{
	m_gravity = i_gravity;
}

Gravity::Gravity()
{}

void Gravity::updateForce(Cube *i_body, float i_duration)
{
	if (!i_body->hasFiniteMass()) return;
	// Apply the mass-scaled force to the body
	i_body->addForce(m_gravity * i_body->getMass());
}