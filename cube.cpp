#pragma once
#include "cube.h"


Cube::Cube()
{

}

Cube::Cube(float i_sizeW, float i_sizeH, float i_sizeD, float i_posX, float i_posY, float i_posZ, bool i_awake)
{

	Vector3 V1(-i_sizeW/2,-i_sizeH/2,i_sizeD/2);
 	Vector3 V2(i_sizeW/2,-i_sizeH/2,i_sizeD/2);
 	Vector3 V3(-i_sizeW/2,-i_sizeH/2,-i_sizeD/2);
	Vector3 V4(i_sizeW/2,-i_sizeH/2,-i_sizeD/2);
 	Vector3 V5(-i_sizeW/2,i_sizeH/2,i_sizeD/2);
	Vector3 V6(i_sizeW/2,i_sizeH/2,i_sizeD/2);
 	Vector3 V7(-i_sizeW/2,i_sizeH/2,-i_sizeD/2);
 	Vector3 V8(i_sizeW/2,i_sizeH/2,-i_sizeD/2);
 
 	m_vertex[0] = V1;
 	m_vertex[1] = V2;
 	m_vertex[2] = V3;
 	m_vertex[3] = V4;
 	m_vertex[4] = V5;
 	m_vertex[5] = V6;
 	m_vertex[6] = V7;
 	m_vertex[7] = V8;

	m_rotateSpeedX = 0; m_rotateSpeedY = 0; m_rotateSpeedZ = 0;
	m_color = Vector4(0,1,0.3,1);

	setDamping(0.99f,0.965f);
	setPosition(i_posX, i_posY, i_posZ);
	m_rotation = Vector3(0,0,0);
	m_velocity = Vector3(0,0,0);
	m_mass = (i_sizeW*i_sizeH*i_sizeD)*1000.0f; //mass
	setMass(m_mass);
	setAwake(i_awake);

	m_forceAccum = Vector3(0,0,0);
	m_torqueAccum = Vector3(0,0,0);
	m_acceleration = Vector3(0,0,0);
	m_lastFrameAcceleration = Vector3(0,0,0);

	m_transformMatrix = Matrix4(4,4);

	m_inverseInertiaTensor = Matrix3(3,3);
	m_inverseInertiaTensorWorld = Matrix3(3,3);
	Matrix3 t_inertiaTensor = Matrix3(3,3);
	t_inertiaTensor[0][0] = (1.0f/12.0f) * m_mass * (pow((i_sizeW/2),2) + pow((i_sizeD/2),2));
	t_inertiaTensor[1][1] = (1.0f/12.0f) * m_mass * (pow((i_sizeH/2),2) + pow((i_sizeD/2),2));
	t_inertiaTensor[2][2] = (1.0f/12.0f) * m_mass * (pow((i_sizeH/2),2) + pow((i_sizeW/2),2));
	setInertiaTensor(t_inertiaTensor);

	m_orientation = QQuaternion(1,0,0,0);

	m_sizeW = i_sizeW;
	m_sizeH = i_sizeH;
	m_sizeD = i_sizeD;

	m_maxVert = Vector3(0,0,0);
	m_minVert = Vector3(0,0,0);

	m_isColliding = false;
	m_isPushed = false;

	calculateDerivedData();
}

/**
* Inline function that creates a transform matrix from a position
* and orientation.
*/
void Cube::calculateTransformMatrix(Matrix4 &i_transformMatrix, Vector3 &i_position, const QQuaternion &i_orientation)
{
	i_transformMatrix[0][0] = 1-2*i_orientation.y()*i_orientation.y() - 2*i_orientation.z()*i_orientation.z();
	i_transformMatrix[1][0] = 2*i_orientation.x()*i_orientation.y() - 2*i_orientation.scalar()*i_orientation.z();
	i_transformMatrix[2][0] = 2*i_orientation.x()*i_orientation.z() + 2*i_orientation.scalar()*i_orientation.y();
	i_transformMatrix[3][0] = i_position[0];

	i_transformMatrix[0][1]	= 2*i_orientation.x()*i_orientation.y() + 2*i_orientation.scalar()*i_orientation.z();
	i_transformMatrix[1][1] = 1-2*i_orientation.x()*i_orientation.x() - 2*i_orientation.z()*i_orientation.z();
	i_transformMatrix[2][1] = 2*i_orientation.y()*i_orientation.z() - 2*i_orientation.scalar()*i_orientation.x();
	i_transformMatrix[3][1] = i_position[1];

	i_transformMatrix[0][2] = 2*i_orientation.x()*i_orientation.z() - 2*i_orientation.scalar()*i_orientation.y();
	i_transformMatrix[1][2] = 2*i_orientation.y()*i_orientation.z() + 2*i_orientation.scalar()*i_orientation.x();
	i_transformMatrix[2][2] = 1-2*i_orientation.x()*i_orientation.x()-2*i_orientation.y()*i_orientation.y();
	i_transformMatrix[3][2] = i_position[2];
}

void Cube::calculateDerivedData()
{
	m_orientation.normalize();

	calculateTransformMatrix(m_transformMatrix, m_position, m_orientation);

	transformInertiaTensor(m_inverseInertiaTensorWorld,m_orientation,m_inverseInertiaTensor,m_transformMatrix);
}

void Cube::setInertiaTensor(Matrix3 &i_inertiaTensor)
{
	m_inverseInertiaTensor = i_inertiaTensor.Invert();
}

void Cube::getInertiaTensor(Matrix3 *i_inertiaTensor)
{
	i_inertiaTensor = &m_inverseInertiaTensor.Invert();
}

void Cube::transformInertiaTensor(Matrix3 &i_iitWorld, const QQuaternion &i_q, Matrix3 &i_iitBody, Matrix4 &i_rotmat)
{
	float *t_rotmat = new float[16];
	float *t_iitBody = new float[9];
	float *t_iitWorld = new float[9];

	t_rotmat = i_rotmat.getMatrix16();
	t_iitBody = i_iitBody.getMatrix9();
	t_iitWorld = i_iitWorld.getMatrix9();

	float t4 = t_rotmat[0]*t_iitBody[0]+t_rotmat[1]*t_iitBody[3]+t_rotmat[2]*t_iitBody[6];
    float t9 = t_rotmat[0]*t_iitBody[1]+t_rotmat[1]*t_iitBody[4]+t_rotmat[2]*t_iitBody[7];
    float t14 = t_rotmat[0]*t_iitBody[2]+t_rotmat[1]*t_iitBody[5]+t_rotmat[2]*t_iitBody[8];
    float t28 = t_rotmat[4]*t_iitBody[0]+t_rotmat[5]*t_iitBody[3]+t_rotmat[6]*t_iitBody[6];
    float t33 = t_rotmat[4]*t_iitBody[1]+t_rotmat[5]*t_iitBody[4]+t_rotmat[6]*t_iitBody[7];
    float t38 = t_rotmat[4]*t_iitBody[2]+t_rotmat[5]*t_iitBody[5]+t_rotmat[6]*t_iitBody[8];
    float t52 = t_rotmat[8]*t_iitBody[0]+t_rotmat[9]*t_iitBody[3]+t_rotmat[10]*t_iitBody[6];
    float t57 = t_rotmat[8]*t_iitBody[1]+t_rotmat[9]*t_iitBody[4]+t_rotmat[10]*t_iitBody[7];
    float t62 = t_rotmat[8]*t_iitBody[2]+t_rotmat[9]*t_iitBody[5]+t_rotmat[10]*t_iitBody[8];

    t_iitWorld[0] = t4*t_rotmat[0]+t9*t_rotmat[1]+t14*t_rotmat[2];
    t_iitWorld[1] = t4*t_rotmat[4]+t9*t_rotmat[5]+t14*t_rotmat[6];
    t_iitWorld[2] = t4*t_rotmat[8]+t9*t_rotmat[9]+t14*t_rotmat[10];
    t_iitWorld[3] = t28*t_rotmat[0]+t33*t_rotmat[1]+t38*t_rotmat[2];
    t_iitWorld[4] = t28*t_rotmat[4]+t33*t_rotmat[5]+t38*t_rotmat[6];
    t_iitWorld[5] = t28*t_rotmat[8]+t33*t_rotmat[9]+t38*t_rotmat[10];
    t_iitWorld[6] = t52*t_rotmat[0]+t57*t_rotmat[1]+t62*t_rotmat[2];
    t_iitWorld[7] = t52*t_rotmat[4]+t57*t_rotmat[5]+t62*t_rotmat[6];
    t_iitWorld[8] = t52*t_rotmat[8]+t57*t_rotmat[9]+t62*t_rotmat[10];

	int k = 0;
	for(int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			i_iitWorld[i][j] = t_iitWorld[k];
			k++;
		}
	}

	delete[] t_rotmat;
	delete[] t_iitBody;
	delete[] t_iitWorld;
}


void Cube::addForce(Vector3 &i_force)
{
	m_forceAccum = m_forceAccum + i_force;
}


void Cube::clearAccumulators()
{
	m_forceAccum = Vector3(0,0,0);
	m_torqueAccum = Vector3(0,0,0);
}

void Cube::addForceAtBodyPoint(float i_force, Vector3 &i_point, Vector3 i_min)
{
	Vector3 m_rayforce = Vector3(i_min[0]*i_force, i_min[1]*i_force, i_min[2]*i_force);

	Vector3 t_cross = m_rayforce.cross(i_point-m_position);
	m_torqueAccum = t_cross*-0.1;
	m_forceAccum = m_rayforce;
}

void Cube::addForceAtPoint(Vector3 &i_force, Vector3 &i_point)
{
	Vector3 t_pt = i_point;
    t_pt = t_pt - m_position;

    m_forceAccum = m_forceAccum +  i_force;
	m_torqueAccum = m_torqueAccum + (t_pt.cross(i_force));
}

void Cube::setMass(float i_mass)
{
	m_inverseMass = ((float)1.0f)/i_mass;
}

float Cube::getMass()
{
	return ((float)1.0f/m_inverseMass);
}

bool Cube::hasFiniteMass()
{
	return m_inverseMass >= 0.0f;
}

void Cube::integrate(float i_duration)
{
	if(!isAwake) return;

	// Calculate linear acceleration from force inputs.
	m_lastFrameAcceleration = m_acceleration;
	m_lastFrameAcceleration.addScaledVector(m_forceAccum, m_inverseMass);


	// Calculate angular acceleration from torque inputs.
	Vector3 t_angularAcceleration = m_inverseInertiaTensorWorld*m_torqueAccum;

	// Adjust velocities
	// Update linear velocity from both acceleration and impulse.
	m_velocity.addScaledVector(m_lastFrameAcceleration, i_duration);

	// Update angular velocity from both acceleration and impulse.
	m_rotation.addScaledVector(t_angularAcceleration, i_duration);

	// Impose drag.
	m_velocity = m_velocity * pow(m_linearDamping,i_duration);
	m_rotation = m_rotation * pow(m_angularDamping,i_duration);

	// Adjust positions
	// Update linear position.
	m_position.addScaledVector(m_velocity, i_duration);

	// Update angular position.
	addScaledOrientation(m_rotation, i_duration);

	// Normalize the orientation, and update the matrices with the new
	// position and orientation.
	calculateDerivedData();

	// Clear accumulators.
	clearAccumulators();

}

void Cube::setAwake(bool i_awake)
{
	isAwake = i_awake;
}

bool Cube::getAwake()
{
	return isAwake;
}

void Cube::setInverseMass(float i_inverseMass)
{
	m_inverseMass = i_inverseMass;
}

float Cube::getInverseMass()
{
	return m_inverseMass;
}

void Cube::getInertiaTensorWorld(Matrix3 *i_inertiaTensor)
{
	i_inertiaTensor = &m_inverseInertiaTensorWorld.Invert();
}

void Cube::setInverseInertiaTensor(Matrix3 &i_inverseInertiaTensor)
{
	m_inverseInertiaTensor = i_inverseInertiaTensor;
}

void Cube::getInverseInertiaTensor(Matrix3 *i_inverseInertiaTensor)
{
	*i_inverseInertiaTensor = m_inverseInertiaTensor;
}

void Cube::getInverseInertiaTensorWorld(Matrix3 *i_inverseInertiaTensor)
{
	 *i_inverseInertiaTensor = m_inverseInertiaTensorWorld;
}


void Cube::setDamping(float i_linearDamping, float i_angularDamping)
{
	m_linearDamping = i_linearDamping;
	m_angularDamping = i_angularDamping;
}

void Cube::setPosition(Vector3 &i_position)
{
	m_position = i_position;
}

void Cube::setPosition(float i_x, float i_y, float i_z)
{
	m_position[0] = i_x;
	m_position[1] = i_y;
	m_position[2] = i_z;

	m_transformMatrix[3][0] = i_x;
	m_transformMatrix[3][1] = i_y;
	m_transformMatrix[3][2] = i_z;
}

void Cube::setPosition(float i_value, int i_axis)
{
	if(i_axis < 3 && i_axis > -1)
	{
		m_position[i_axis] = i_value;
		m_transformMatrix[3][i_axis] = i_value;
	}
}

void Cube::changePosition(float i_value, int i_axis)
{
	if (i_axis == 0)
	{
		addForce(Vector3(i_value,0,0));
	}
	if (i_axis == 1)
	{
		addForce(Vector3(0,i_value,0));
	}
	if (i_axis == 2)
	{
		addForce(Vector3(0,0,i_value));
	}
}

void Cube::changeRotation(float i_value, int i_axis)
{
	if (i_axis == 0)
	{
		m_torqueAccum = Vector3(i_value,0,0);
	}
	if (i_axis == 1)
	{
		m_torqueAccum = Vector3(0,i_value,0);
	}
	if (i_axis == 2)
	{
		m_torqueAccum = Vector3(0,0,i_value);
	}
}

Vector3 Cube::getPosition()
{
	return m_position;
}

void Cube::setVelocity(Vector3 &i_velocity)
{
	m_velocity = i_velocity;
}

void Cube::addScaledOrientation(Vector3 &i_velocity, float scale)
{
	QQuaternion t_q(1,
					i_velocity[0] * scale,
					i_velocity[1] * scale,
					i_velocity[2] * scale);

	t_q *= m_orientation;

	m_orientation.setScalar( m_orientation.scalar() + t_q.scalar() * 0.7f);
	m_orientation.setX( m_orientation.x() + t_q.x() * 0.7f);
	m_orientation.setY( m_orientation.y() + t_q.y() * 0.7f);
	m_orientation.setZ( m_orientation.z() + t_q.z() * 0.7f);
}

void Cube::addVelocity(Vector3 &i_deltaVel)
{
	m_velocity = m_velocity + i_deltaVel;
}

void Cube::addRotation(Vector3 &i_rot)
{
	m_rotation = m_rotation + i_rot;
}

void Cube::loadState()
{
	m_orientation = m_prevOrientation;
	m_position = m_prevPosition;
}

void Cube::saveState()
{
	m_prevOrientation = m_orientation;
	m_prevPosition = m_position;
}

float Cube::getVertex(int vec, int xyz)
{
	return m_vertex[vec][xyz];
}

Vector3 Cube::getVertex(int vec)
{
	return m_vertex[vec];
}

float Cube::getPosX()
{
	return m_position[0];
}
float Cube::getPosY()
{
	return m_position[1];
}
float Cube::getPosZ()
{
	return m_position[2];
}
int Cube::getRSpeedX()
{
	return m_rotateSpeedX;
}
int Cube::getRSpeedY()
{
	return m_rotateSpeedY;
}
int Cube::getRSpeedZ()
{
	return m_rotateSpeedZ;
}
Vector4 Cube::getColor()
{
	return m_color;
}

void Cube::setColor(Vector4 i_color)
{
	m_color = i_color;
}