#include "contact.h"

Contact::Contact()
{
	m_penetration = 0;
	Vector3 m_contactPoint = Vector3(0,0,0);
	Vector3 m_contactNormal = Vector3(0,0,0);
}

void Contact::SetBodyData(Cube* i_b1, Cube* i_b2)
{
	m_bodys[0] = i_b1;
	m_bodys[1] = i_b2;
}