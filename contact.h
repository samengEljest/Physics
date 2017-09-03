#pragma  once
#include "cube.h"

class Contact
{
public:
	Contact();

	Cube* m_bodys[2];
	Vector3 m_contactPoint;
	Vector3 m_contactNormal;
	float m_penetration;

	void SetBodyData(Cube* i_b1, Cube* i_b2);

};