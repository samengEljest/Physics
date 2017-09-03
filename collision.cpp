#include "collision.h"

Collision::Collision()
{
	m_cubeCount = 0;
	m_figureCount = 0;
	m_sweepCollidingCountX = 0;
	m_sweepCollidingCountY = 0;
	m_sweepCollidingCountZ = 0;
	m_collidingAABBCount = 0;
	m_collisionDataCount = 0;
	
}

//ABB Collision ------------------------------------------------------------------------------------------------------------
void Collision::addToArray(Cube *i_cube)
{
	///figure arrays
	m_xFigures[m_figureCount].m_figure = i_cube->m_minVert[0];
	m_yFigures[m_figureCount].m_figure = i_cube->m_minVert[1];
	m_zFigures[m_figureCount].m_figure = i_cube->m_minVert[2];

	m_xFigures[m_figureCount].cubeIndex = m_cubeCount;
	m_yFigures[m_figureCount].cubeIndex = m_cubeCount;
	m_zFigures[m_figureCount].cubeIndex = m_cubeCount;

	m_xFigures[m_figureCount].isBeginning = true;
	m_yFigures[m_figureCount].isBeginning = true;
	m_zFigures[m_figureCount].isBeginning = true;

	m_figureCount++;

	m_xFigures[m_figureCount].m_figure = i_cube->m_maxVert[0];
	m_yFigures[m_figureCount].m_figure = i_cube->m_maxVert[1];
	m_zFigures[m_figureCount].m_figure = i_cube->m_maxVert[2];

	m_xFigures[m_figureCount].cubeIndex = m_cubeCount;
	m_yFigures[m_figureCount].cubeIndex = m_cubeCount;
	m_zFigures[m_figureCount].cubeIndex = m_cubeCount;

	m_xFigures[m_figureCount].isBeginning = false;
	m_yFigures[m_figureCount].isBeginning = false;
	m_zFigures[m_figureCount].isBeginning = false;

	m_figureCount++;
	m_cubeCount++;
}
void Collision::updateArrays(Cube *i_cube, int i_index)
{
	//X
	for (int i = 0; i < m_figureCount; i++)
	{
		if (m_xFigures[i].cubeIndex == i_index)
		{
			if (m_xFigures[i].isBeginning)
			{
				m_xFigures[i].m_figure = i_cube->m_minVert[0];
			}
			else
			{
				m_xFigures[i].m_figure = i_cube->m_maxVert[0];
			}
		}
	}
	//Y
	for (int i = 0; i < m_figureCount; i++)
	{
		if (m_yFigures[i].cubeIndex == i_index)
		{
			if (m_yFigures[i].isBeginning)
			{
				m_yFigures[i].m_figure = i_cube->m_minVert[1];
			}
			else
			{
				m_yFigures[i].m_figure = i_cube->m_maxVert[1];
			}
		}
	}
	//Z
	for (int i = 0; i < m_figureCount; i++)
	{
		if (m_zFigures[i].cubeIndex == i_index)
		{
			if (m_zFigures[i].isBeginning)
			{
				m_zFigures[i].m_figure = i_cube->m_minVert[2];
			}
			else
			{
				m_zFigures[i].m_figure = i_cube->m_maxVert[2];
			}
		}
	}
}

void Collision::insertionSort(figure *i_figures)
{
	figure t_tmp; 
	int j;
	
	for (int i = 1; i < m_figureCount; i++)
	{
		j = i;
		while(j > 0 && i_figures[j - 1].m_figure > i_figures[j].m_figure)
		{
			t_tmp = i_figures[j];
			i_figures[j] = i_figures[j - 1];
			i_figures[j - 1] = t_tmp;
			j--;
		}
	}	
}

int Collision::sweep(figure *i_figures, int *i_colliding)
{
	int t_activeAmount = 0;
	int i_collidingIndex = 0;
	
	for(int i = 0; i < m_figureCount; i++) //go throe all figures
	{
		if (i_figures[i].isBeginning) //if it is the beginning of an intervall continue
		{
			for (int j = i; i < m_figureCount; j++) //
			{
				if (i_figures[j].cubeIndex == i_figures[i].cubeIndex && !i_figures[j].isBeginning) //continue untill you find the end of i intervall
				{
					for (int k = 0; k < t_activeAmount; k++)
					{
						//put the two colliding intervalls indexes in an array, reset activeAmount
						i_colliding[i_collidingIndex++] = i_figures[i].cubeIndex;
						i_colliding[i_collidingIndex++] = t_activeList[k];
					}
					t_activeAmount = 0;
					break;
				}
				else if (i_figures[j].cubeIndex != i_figures[i].cubeIndex && i_figures[j].isBeginning) //if its the beginning of another intervall throw it in the active list 
				{
					//put intervall that is colliding with i in activelist
					t_activeList[t_activeAmount++] = i_figures[j].cubeIndex;
				}
			}
		}
	}
	return i_collidingIndex;
}

void Collision::getCollidOnAxis(int i_axis, int* i_return)
{
	if (i_axis == 0)
	{
		i_return = m_sweepCollidingX;
	}
	else if (i_axis == 1)
	{
		i_return = m_sweepCollidingY;
	}
	else if (i_axis == 2)
	{
		i_return = m_sweepCollidingZ;
	}
}

void Collision::collidingAABB(int i_axis)
{
	m_collidingAABBCount = 0;
	Vector3 t_collisionArrayX[100];
	int j = 0;
	for (int i = 0; i < m_sweepCollidingCountX;)
	{
		t_collisionArrayX[j] = Vector3(m_sweepCollidingX[i],m_sweepCollidingX[i+1], 1);
		i+=2;
		j++;
	}

	for (int j = 0; j < m_sweepCollidingCountX/2; j++)
	{
		for (int i = 0; i < m_sweepCollidingCountY;)
		{
			if (t_collisionArrayX[j][0] == m_sweepCollidingY[i] && t_collisionArrayX[j][1] == m_sweepCollidingY[i+1] ||
				t_collisionArrayX[j][1] == m_sweepCollidingY[i] && t_collisionArrayX[j][0] == m_sweepCollidingY[i+1])
			{
				t_collisionArrayX[j][2]++;
			}
			i+=2;
		}
		for (int i = 0; i < m_sweepCollidingCountZ;)
		{
			if (t_collisionArrayX[j][0] == m_sweepCollidingZ[i] && t_collisionArrayX[j][1] == m_sweepCollidingZ[i+1] ||
				t_collisionArrayX[j][1] == m_sweepCollidingZ[i] && t_collisionArrayX[j][0] == m_sweepCollidingZ[i+1])
			{
				t_collisionArrayX[j][2]++;
			}
			i+=2;
		}
	}
	for (int i = 0; i < m_sweepCollidingCountX/2; i++)
	{
		if (t_collisionArrayX[i][2] >= 3)
		{
			m_collidingAABB[m_collidingAABBCount] = t_collisionArrayX[i][0];
			m_collidingAABB[m_collidingAABBCount+1] = t_collisionArrayX[i][1];
			m_collidingAABBCount+=2;
		}
	}
}

void Collision::updateAABBCollision()
{
	insertionSort(m_xFigures);
	insertionSort(m_yFigures);
	insertionSort(m_zFigures);
	m_sweepCollidingCountX = sweep(m_xFigures, m_sweepCollidingX);
	m_sweepCollidingCountY = sweep(m_yFigures, m_sweepCollidingY);
	m_sweepCollidingCountZ = sweep(m_zFigures, m_sweepCollidingZ);
	collidingAABB(0);
	
}


///OBB Collision -------------------------------------------------------------------------------------------------------------------------------
#define CHECK_OVERLAP(axis, index) \
	if (!testAxis(i_b1, i_b2, (axis), t_center, (index), t_penetration, t_best)) return 0;
#define GET_COL(object,i) \
	Vector3((object)->m_transformMatrix[(i)][0],(object)->m_transformMatrix[(i)][1],(object)->m_transformMatrix[(i)][2])
#define GET_HALFESIZE(object) \
	Vector3((object)->m_sizeW/2, (object)->m_sizeH/2, (object)->m_sizeD/2)
#define GET_HALFESIZE4(object) \
	Vector4((object)->m_sizeW/2, (object)->m_sizeH/2, (object)->m_sizeD/2, 1)

float Collision::transformToAxis(Cube* i_B, Vector3 i_vec)
{
	return
		i_B->m_sizeW/2 * abs(i_vec.dot(GET_COL(i_B, 0))) +
		i_B->m_sizeH/2 * abs(i_vec.dot(GET_COL(i_B, 1))) +
		i_B->m_sizeD/2 * abs(i_vec.dot(GET_COL(i_B, 2)));
}

float Collision::penOnAxis(Cube* i_1, Cube* i_2, Vector3 i_axis, Vector3 &i_toCenter)
{
	float t_oneProject = transformToAxis(i_1, i_axis);
	float t_twoProject = transformToAxis(i_2, i_axis);

	float t_distance = abs(i_toCenter.dot(i_axis));

	return t_oneProject + t_twoProject - t_distance;
}


bool Collision::boxOnbox(Cube* i_b1, Cube* i_b2, CollisionData *i_data)
{
	Vector3 t_center = GET_COL(i_b2,3) - GET_COL(i_b1,3);
	float t_penetration = 9999999999;
	unsigned t_best = -1;

	CHECK_OVERLAP(GET_COL(i_b1,0),0);
	CHECK_OVERLAP(GET_COL(i_b1,1),1);
	CHECK_OVERLAP(GET_COL(i_b1,2),2);

	CHECK_OVERLAP(GET_COL(i_b2,0),3);
	CHECK_OVERLAP(GET_COL(i_b2,1),4);
	CHECK_OVERLAP(GET_COL(i_b2,2),5);

	unsigned t_bestSingleAxis = t_best;

	CHECK_OVERLAP(GET_COL(i_b1,0).cross(GET_COL(i_b2,0)), 6);
	CHECK_OVERLAP(GET_COL(i_b1,0).cross(GET_COL(i_b2,1)), 7);
	CHECK_OVERLAP(GET_COL(i_b1,0).cross(GET_COL(i_b2,2)), 8);
	CHECK_OVERLAP(GET_COL(i_b1,1).cross(GET_COL(i_b2,0)), 9);
	CHECK_OVERLAP(GET_COL(i_b1,1).cross(GET_COL(i_b2,1)), 10);
	CHECK_OVERLAP(GET_COL(i_b1,1).cross(GET_COL(i_b2,2)), 11);
	CHECK_OVERLAP(GET_COL(i_b1,2).cross(GET_COL(i_b2,0)), 12);
	CHECK_OVERLAP(GET_COL(i_b1,2).cross(GET_COL(i_b2,1)), 13);
	CHECK_OVERLAP(GET_COL(i_b1,2).cross(GET_COL(i_b2,2)), 14);

	if (t_best < 3)
	{
		fillPointFaceBoxBox(i_b1, i_b2, t_center, i_data, t_best, t_penetration);
		return true;
	}
	else if (t_best < 6)
	{
		fillPointFaceBoxBox(i_b2, i_b1, t_center*-1.0f, i_data, t_best-3, t_penetration);
		return true;
	}
	else
	{
		t_best -= 6;
		unsigned int t_oneAxisIndex = t_best / 3;
		unsigned int t_twoAxisIndex = t_best % 3;
		Vector3 t_oneAxis = GET_COL(i_b1,t_oneAxisIndex);
		Vector3 t_twoAxis = GET_COL(i_b2,t_twoAxisIndex);
		Vector3 t_axis = t_oneAxis.cross(t_twoAxis);
		t_axis.normal();

		if (t_axis.dot(t_center) > 0)
		{
			t_axis = t_axis * -1.0f;
		}

		Vector3 t_pointOnOneEdge = GET_HALFESIZE(i_b1);
		Vector3 t_pointOnTwoEdge = GET_HALFESIZE(i_b1);

		for (int i = 0; i < 3; i++)
		{
			if (i == t_oneAxisIndex)
			{
				t_pointOnOneEdge[i] = 0;
			}
			else if (GET_COL(i_b1,i).dot(t_axis) > 0)
			{
				t_pointOnOneEdge[i] = -t_pointOnOneEdge[i];
			}

			if (i == t_twoAxisIndex)
			{
				t_pointOnTwoEdge[i] = 0;
			}
			else if (GET_COL(i_b2,i).dot(t_axis) < 0)
			{
				t_pointOnTwoEdge[i] = -t_pointOnTwoEdge[i];
			}
		}

		Vector4 t_pointOnOneEdge4 = Vector4(t_pointOnOneEdge[0], t_pointOnOneEdge[1], t_pointOnOneEdge[2], 1);
		Vector4 t_pointOnTwoEdge4 = Vector4(t_pointOnTwoEdge[0], t_pointOnTwoEdge[1], t_pointOnTwoEdge[2], 1);

		t_pointOnOneEdge4 = ~i_b1->m_transformMatrix * t_pointOnOneEdge4; 
		t_pointOnTwoEdge4 = ~i_b2->m_transformMatrix * t_pointOnTwoEdge4;

		t_pointOnOneEdge = Vector3(t_pointOnOneEdge4[0], t_pointOnOneEdge4[1], t_pointOnOneEdge4[2]);
		t_pointOnTwoEdge = Vector3(t_pointOnTwoEdge4[0], t_pointOnTwoEdge4[1], t_pointOnTwoEdge4[2]);

		Vector3 t_halfeSize1 = GET_HALFESIZE(i_b1);
		Vector3 t_halfeSize2 = GET_HALFESIZE(i_b2);
		Vector3 t_vertex = contactPoint(t_pointOnOneEdge, t_oneAxis, t_halfeSize1[t_oneAxisIndex],
										t_pointOnTwoEdge, t_twoAxis, t_halfeSize2[t_twoAxisIndex],
										t_bestSingleAxis > 2);


		i_data->m_penetration = t_penetration;
		i_data->m_contactNormal = t_axis;
		i_data->m_contactPoint = t_vertex;
		i_data->SetBodyData(i_b1,i_b2);
		return true;

	}

	return false;

}

bool Collision::testAxis(Cube* i_1, Cube* i_2, Vector3 i_axis, Vector3 &i_toCenter, unsigned i_index, float &i_SmalestPenetration, unsigned &i_SmallestCase)
{
	if((i_axis.length() * i_axis.length()) < 0.0001)
	{
		return true;
	}

	i_axis = i_axis.normal();

	float t_pen = penOnAxis(i_1, i_2, i_axis, i_toCenter);

	if (t_pen < 0)
	{
		return false;
	}
	if (t_pen < i_SmalestPenetration) 
	{
		i_SmalestPenetration = t_pen;
		i_SmallestCase = i_index;
	}
	return true;
}

void Collision::fillPointFaceBoxBox(Cube* i_b1, Cube* i_b2, Vector3 &i_toCenter, CollisionData *i_data, unsigned int i_best, float i_penetaration)
{
	Vector3 t_normal = GET_COL(i_b1, i_best);
	if ((t_normal.dot(i_toCenter)) > 0)
	{
		t_normal = t_normal * -1.0f;
	}

	Vector4 t_vertex = GET_HALFESIZE4(i_b2);
	if ((GET_COL(i_b2,0).dot(t_normal)) < 0)
	{
		t_vertex[0] = -t_vertex[0];
	}
	if ((GET_COL(i_b2,1).dot(t_normal)) < 0)
	{
		t_vertex[1] = -t_vertex[1];
	}
	if ((GET_COL(i_b2,2).dot(t_normal)) < 0)
	{
		t_vertex[2] = -t_vertex[2];
	}

	i_data->m_penetration = i_penetaration;
	i_data->m_contactNormal = t_normal;
	//vector3 convert
	Vector4 t_vec4 = ~i_b2->m_transformMatrix * t_vertex;
	Vector3 t_vec3 = Vector3(t_vec4[0], t_vec4[1], t_vec4[2]);
	//vector3 convert
	i_data->m_contactPoint = t_vec3;
	i_data->SetBodyData(i_b1, i_b2);

}

Vector3 Collision::contactPoint(Vector3 &i_p1, Vector3 &i_d1, float i_oneSize, Vector3 &i_p2, Vector3 &i_d2, float i_twoSize, bool useOne)
{
	Vector3 toSt, cOne, cTwo;
	float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
	float denom, mua, mub;

	smOne = i_d1.length() * i_d1.length();
	smTwo = i_d2.length() * i_d2.length();
	dpOneTwo = i_d2.dot(i_d1);

	toSt = i_p1 - i_p2;
	dpStaOne = i_d1.dot(toSt);
	dpStaTwo = i_d2.dot(toSt);

	 denom = smOne * smTwo - dpOneTwo * dpOneTwo;

	 if (abs(denom) < 0.0001f)
	 {
		 return useOne?i_p1:i_p2;
	 }

	 mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
	 mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

	 if (mua > i_oneSize || mua < -i_oneSize || mub > i_twoSize || mub < -i_twoSize)
	 {
		 return useOne?i_p1:i_p2;
	 }
	 else
	 {
		 cOne = i_p1+(i_d1*mua);
		 cTwo = i_p2+(i_d2*mub);

		 return (cOne * 0.5) + (cTwo * 0.5);
	 }
}

//Contact ----------------------------------------------------------------------------------------------------------------

void Collision::resolveCollsisions(float i_duration)
{
	//assumes 2 objects are in contact.
	for (int i = 0; i < m_collisionDataCount; i++)
	{
		Cube* t_obj1 = m_collisionData[i].m_bodys[0];
		Cube* t_obj2 = m_collisionData[i].m_bodys[1];

		Vector3 t_contactPoint = m_collisionData[i].m_contactPoint;
		Vector3 t_contactNormal = m_collisionData[i].m_contactNormal;;

		Vector3 t_relativeContact[2];
		t_relativeContact[0] = t_contactPoint - t_obj1->getPosition();
		t_relativeContact[1] = t_contactPoint - t_obj2->getPosition();

		Vector3 t_closingVelocity[2];
		t_closingVelocity[0] = t_obj1->m_velocity + t_obj1->m_rotation.cross(t_relativeContact[0]);  
		t_closingVelocity[1] = t_obj2->m_velocity + t_obj2->m_rotation.cross(t_relativeContact[1]);

		Vector3 t_deltaVelocity = t_closingVelocity[0] - t_closingVelocity[1];

		if (-t_deltaVelocity.dot(t_contactNormal) < -0.01f)
		{
			continue; // if they are moving away from each other, no need to apply impulse
		}

		//normal impulse
		float t_restitu = 1.1f;

		Vector3 tmpVector = Vector3();

		float t_normDiv = tmpVector.dot(t_contactNormal, t_contactNormal) * ( (t_obj1->getInverseMass() + t_obj2->getInverseMass())
			+tmpVector.dot( t_contactNormal, tmpVector.cross(  t_obj1->m_inverseInertiaTensorWorld * 
			tmpVector.cross( t_relativeContact[0], t_contactNormal) , t_relativeContact[0] )
			+ tmpVector.cross( t_obj2->m_inverseInertiaTensorWorld * 
			tmpVector.cross( t_relativeContact[1], t_contactNormal) , t_relativeContact[1])));

		//float normDiv =  contactNormal.dot(contactNormal) * ( (obj1->getInverseMass() + obj2->getInverseMass())
		//	+ contactNormal.dot( (obj1->m_inverseInertiaTensor * relativeContact[0].cross(contactNormal)).cross(relativeContact[0])
		//	+ obj1->m_inverseInertiaTensor * relativeContact[1].cross(t_contactPoint).cross(relativeContact[1])));


		float t_jn = -1*(1+t_restitu)* t_deltaVelocity.dot(t_contactNormal) / t_normDiv;

		t_jn += m_collisionData[i].m_penetration*1.5;

		t_obj1->m_velocity = t_obj1->m_velocity + (t_contactNormal * t_obj1->getInverseMass() * t_jn);
		t_obj1->m_rotation = t_obj1->m_rotation + (t_obj1->m_inverseInertiaTensorWorld * (t_relativeContact[0].cross(t_contactNormal * t_jn)));

		t_obj2->m_velocity = t_obj2->m_velocity - (t_contactNormal * t_obj2->getInverseMass() * t_jn);
		t_obj2->m_rotation = t_obj2->m_rotation - (t_obj2->m_inverseInertiaTensorWorld * t_relativeContact[1].cross(t_contactNormal * t_jn));
		//tangent impulse

		Vector3 t_tangent = t_deltaVelocity - (t_contactNormal * t_deltaVelocity.dot(t_contactNormal));
		t_tangent = t_tangent.normal();

		/*float tangDiv = obj1->getInverseMass() + obj2->getInverseMass()
			+ tangent.dot((obj1->m_inverseInertiaTensor * relativeContact[0].cross(tangent)).cross(relativeContact[0])
			+ obj2->m_inverseInertiaTensor * relativeContact[1].cross(tangent).cross(relativeContact[1]));*/

		float t_tangDiv = t_obj1->getInverseMass() + t_obj2->getInverseMass()
			+ tmpVector.dot(t_tangent, tmpVector.cross((t_obj1->m_inverseInertiaTensor * 
			tmpVector.cross( t_relativeContact[0], t_tangent)), t_relativeContact[0])
			+ tmpVector.cross((t_obj2->m_inverseInertiaTensor * tmpVector.cross(t_relativeContact[1], t_tangent)), t_relativeContact[1]) );

		//set force depending on the penetration
		float t_jt = -1 * t_deltaVelocity.dot(t_tangent) / t_tangDiv;
		//clamp min/max 

		t_obj1->m_velocity = t_obj1->m_velocity + (t_tangent * t_obj1->getInverseMass() * t_jt);
		t_obj1->m_rotation = t_obj1->m_rotation + (t_obj1->m_inverseInertiaTensorWorld * t_relativeContact[0].cross(t_tangent * t_jt));

		t_obj2->m_velocity = t_obj2->m_velocity - (t_tangent * t_obj2->getInverseMass() * t_jt);
		t_obj2->m_rotation = t_obj2->m_rotation - (t_obj2->m_inverseInertiaTensorWorld * t_relativeContact[1].cross(t_tangent * t_jt));

		//push them back so they no longer in contact
		t_obj1->loadState();
		t_obj2->loadState();
	}
}

//Other -----------------------------------------------------------------------------------------------------------------
QQuaternion Collision::addScaledOrientation(QQuaternion &i_quaterion, Vector3 &i_velocity, float scale)
{
	QQuaternion t_q(1,
		i_velocity[0] * scale,
		i_velocity[1] * scale,
		i_velocity[2] * scale);

	t_q *= i_quaterion;

	t_q.setScalar( i_quaterion.scalar() + t_q.scalar() * 0.5f);
	t_q.setX( i_quaterion.x() + t_q.x() * 0.5f);
	t_q.setY( i_quaterion.y() + t_q.y() * 0.5f);
	t_q.setZ( i_quaterion.z() + t_q.z() * 0.5f);

	return t_q;
}

float Collision::abs(float i_value)
{
	if (i_value < 0)
	{
		i_value = i_value * -1;
	}
	return i_value;
}

void Collision::clarCollisionData()
{
	/*for (int i = 0; i < m_collisionDataCount; i++)
	{
		delete m_collisionData[i];
	}*/
	m_collisionDataCount = 0;
}