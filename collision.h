#pragma once
#include "cube.h"
#include "contact.h"


struct interval
{
	float m_beginning;
	float m_end;
	int cubeIndex;
};

struct figure
{
	float m_figure;
	int cubeIndex;
	bool isBeginning;
};

struct CollisionData
{
	//new
	Cube* m_bodys[2];
	Vector3 m_contactPoint;
	Vector3 m_contactNormal;
	float m_penetration;
	Vector3 m_contactVelocity;

	void SetBodyData(Cube* i_b1, Cube* i_b2)
	{
		m_bodys[0] = i_b1;
		m_bodys[1] = i_b2;
	}
};

class Collision
{

public:
	Collision();

	void addToArray(Cube *i_cube);
	void insertionSort(figure *i_figures);
	int sweep(figure *i_figures, int *i_colliding);
	void getCollidOnAxis(int i_axis, int* i_return); //x = 0, y = 1, z = 2.
	void updateArrays(Cube *i_cube, int i_index);
	void collidingAABB(int i_axis);
	void updateAABBCollision();
	
	bool boxOnbox(Cube* i_b1, Cube* i_b2, CollisionData *i_data);
	float penOnAxis(Cube* i_1, Cube* i_2, Vector3 i_axis, Vector3 &i_toCenter);
	float transformToAxis(Cube* i_B, Vector3 i_vec);
	bool testAxis(Cube* i_1, Cube* i_2, Vector3 i_axis, Vector3 &i_toCenter, unsigned i_index, float& i_SmalestPenetration, unsigned& i_SmallestCase);
	void fillPointFaceBoxBox(Cube* i_b1, Cube* i_b2, Vector3 &i_toCenter, CollisionData *i_data, unsigned int i_best, float i_penetaration);
	Vector3 contactPoint(Vector3 &i_p1, Vector3 &i_d1, float i_oneSize, Vector3 &i_p2, Vector3 &i_d2, float i_twoSize, bool useOne);

	void resolveCollsisions(float i_duration);
	void clarCollisionData();



	QQuaternion addScaledOrientation(QQuaternion &i_quaterion, Vector3 &i_velocity, float scale);
	float abs(float i_value);


	figure m_xFigures[200];
	figure m_yFigures[200];
	figure m_zFigures[200];

	interval m_xArray[100];
	interval m_yArray[100];
	interval m_zArray[100];

	int m_sweepCollidingX[100];
	int m_sweepCollidingCountX;

	int m_sweepCollidingY[100];
	int m_sweepCollidingCountY;

	int m_sweepCollidingZ[100];
	int m_sweepCollidingCountZ;

	int m_collidingAABB[100];
	int m_collidingAABBCount;
	CollisionData m_collisionData[50];
	int m_collisionDataInex;
	int m_collisionDataCount;

	int m_positionIterationsUsed;
	int m_positionIterations;
	int m_velocityIterationsUsed;
	int m_velocityIterations;

	private:
	int m_cubeCount;
	int m_figureCount;
	int t_activeList[100];
};