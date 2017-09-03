#pragma once
#include <QtGui/QMainWindow>
#include <QGLWidget>
#include <QtOpenGL>
#include <qapplication.h> 
#include <qimage.h>
#include <qgl.h>
#include <QDebug>
#include "Vector4.h"
#include "Vector3.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include <QQuaternion>

#include <string>

class Cube
{
public:
	Cube();
	Cube(float i_sizeW, float i_sizeH, float i_sizeD, float i_posX, float i_posY, float i_posZ, bool i_awake);

	float m_sizeW;
	float m_sizeH;
	float m_sizeD;

	bool m_isColliding;
	bool m_isPushed;

	float getVertex(int vec, int xyz);
	Vector3 getVertex(int vec);
	float getPosX();
	float getPosY(); 
	float getPosZ();
	int getRSpeedX(); 
	int getRSpeedY(); 
	int getRSpeedZ();
	Vector4 getColor();
	Vector3 m_maxVert;
	Vector3 m_minVert;
	float m_vertWorld[24]; 

	float m_minMaxX[2];
	float m_minMaxY[2];
	float m_minMaxZ[2];

	Matrix4 m_transformMatrix; /// > transform matrix for converting body space into world space and vice versa.
	void setColor(Vector4 i_color);
	Vector3 m_rotation; /// The angular velocity, or rotation, or the rigid body in world space.
	Vector3 m_velocity; /// The linear velocity of the rigid body in world space.
	Vector3 m_lastFrameAcceleration;
	QQuaternion m_orientation; /// Quaternion that Holds the angular orientation of the rigid body in world space.
	QQuaternion m_prevOrientation;
	Matrix3 m_inverseInertiaTensor; /// The inverse of the body’s inertia tensor
	Matrix3 m_inverseInertiaTensorWorld;

	/**
	* Calculates internal data from state data. This should be called
	* after the body’s state is altered directly (it is called
	* automatically during integration). If you change the body’s
	* state and then intend to integrate before querying any data
	* (such as the transform matrix), then you can omit this step.
	*/
	void calculateDerivedData();


	/**
	* Calculate the Transfom Matrix.
	*/
	void calculateTransformMatrix(Matrix4 &i_transformMatrix, Vector3 &i_position,const QQuaternion &i_orientation);

	/**
	* set the inverse InertiaTensor from the InertiaTensor
	*/
	void setInertiaTensor( Matrix3 &i_inertiaTensor);

	/**
    * Copies the current inertia tensor of the rigid body into
    * the given matrix.
	*/
	void getInertiaTensor(Matrix3 *i_inertiaTensor);

	/**
	* Internal function to do an inertia tensor transform by a quaternion.
	*/
	void transformInertiaTensor(Matrix3 &i_iitWorld, const QQuaternion &i_q, Matrix3 &i_i_iitBody, Matrix4 &i_i_rotmat);

	/**
	* Adds the given force to the center of mass of the rigid body.
	* The force is expressed in world coordinates.
	*/
	void addForce(Vector3 &i_force);

	/**
	* clear accumilators
	*/
	void clearAccumulators();

	/**
	* Adds the given force to the given point on the rigid body.
	* The direction of the force is given in world coordinates,
	* but the application point is given in body space.
	*/
	void addForceAtBodyPoint(float i_force, Vector3 &i_point, Vector3 i_min);

	/**
    * Adds the given force to the given point on the rigid body.
	*/
	void addForceAtPoint(Vector3 &i_force, Vector3 &i_point);

	/**
    * Sets the mass of the rigid body.
	*/
	void setMass(float i_mass);

	/**
    * Gets the mass of the rigid body.
	*/
	float getMass();

	void setAwake(bool i_awake);
	bool getAwake();

	/**
    * Returns true if the mass of the body is not-infinite.
    */
	bool hasFiniteMass();

	/**
    * Integrates the rigid body forward in time by the given amount.
	*/
	void integrate(float i_duration);

	/**
    * Sets the inverse mass of the rigid body.
	*/
	void setInverseMass(float i_inverseMass);

	/**
    * Gets the inverse mass of the rigid body.
	*/
	float getInverseMass();

	/**
    * Copies the current inertia tensor of the rigid body into
    * the given matrix.
	*/
	void getInertiaTensorWorld(Matrix3 *i_inertiaTensor);

	/**
    * Sets the inverse intertia tensor for the rigid body.
	*/
	void setInverseInertiaTensor(Matrix3 &i_inverseInertiaTensor);

	/**
    * Copies the current inverse inertia tensor of the rigid body
    * into the given matrix.
	*/
	void getInverseInertiaTensor(Matrix3 *i_inverseInertiaTensor);

	/**
    * Copies the current inverse inertia tensor of the rigid body
    * into the given matrix.
	*/
	void getInverseInertiaTensorWorld(Matrix3 *i_inverseInertiaTensor);

	/**
    * Sets both linear and angular damping in one function call.
	*/
	void setDamping(float i_linearDamping, float i_angularDamping);

	/**
    * Sets the position of the rigid body.
	*/
	void setPosition(Vector3 &i_position);
	void setPosition(float i_x, float i_y, float i_z);
	void setPosition(float i_value, int i_axis);

	void changePosition(float i_value, int i_axis);
	void changeRotation(float i_value, int i_axis);

	/**
    * Gets the position of the rigid body.
	*/
	Vector3 getPosition();

	/**
    * Sets the velocity of the rigid body.
	*/
	void setVelocity(Vector3 &i_velocity);

	void addScaledOrientation(Vector3 &i_velocity, float scale);

	/**
	* Applies the given change in velocity.
	*/
	void addVelocity(Vector3 &i_deltaVel);

	/**
	* Applies the given change in rotation.
	*/
	void addRotation(Vector3 &i_rot);

	void loadState();
	void saveState();


private:
	int m_rotateSpeedX,m_rotateSpeedY,m_rotateSpeedZ;
	Vector3 m_vertex[8];
	Vector4 m_color;

	float m_mass;
	float m_inverseMass;
	
	Vector3 m_position; /// The position of the cube
	Vector3 m_prevPosition;

	Vector3 m_forceAccum;
	Vector3 m_torqueAccum;

	Vector3 m_acceleration;

	float m_linearDamping;
	float m_angularDamping;

	bool isAwake;

};