#pragma once
#include "GlWindow.h"

GlWindow::GlWindow(QWidget* parent) : QGLWidget(parent)
{
	doBeginning();

	this->m_timer.setInterval(1);
	connect(&this->m_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	this->m_timer.start();

	this->m_UpdatePhy.setInterval(1);
	connect(&this->m_UpdatePhy, SIGNAL(timeout()), this, SLOT(updatePhysics()));
	this->m_UpdatePhy.start();
}

GlWindow::~GlWindow()
{}


void GlWindow::doBeginning()
{
	m_cubeCount = 0;

	//editabe this for a good time
	m_cubeWidth = 2;
	m_cubeHeight = 2;
	m_cubeDepth = 2;
	m_force = 1.0; //start force
	Vector3 t_gravity = Vector3(0,-0.002,0);
	Vector3 m_spawnCube = Vector3(0,0,0);

	m_choosenCube1 = -1;
	m_choosenCube2 = -1;
	m_wireframeON = false;
	m_camYaw = 0.0f;
	m_camPitch = 0.0f;
	m_camX = 0.0f;
	m_camY = 0.0f;
	m_camZ = 25.0f;
	m_mouseIn = false;
	m_pi = 3.1415926535f;
	m_moveVel = 0.2f;
	m_raycastDone = false;
	m_rayHit = false;
	m_cubeAwake = false;
	m_gravityBool = false;
	m_camMat = Matrix4(4,4);
	m_gravity = Gravity(t_gravity);
	m_collision = Collision();
	m_springify = false;

	//Ground
	Vector4 V1(-10,-0.01,10,  1);
	Vector4 V2(10,-0.01,10,  1);
	Vector4 V3(-10,-0.01,-10,  1);
	Vector4 V4(10,-0.01,-10,  1);
	Vector4 V5(-10,0.01,10,  1);
	Vector4 V6(10,0.01,10,  1);
	Vector4 V7(-10,0.01,-10,  1);
	Vector4 V8(10,0.01,-10,  1);

	m_groundVertex[0] = V1;
	m_groundVertex[1] = V2;
	m_groundVertex[2] = V3;
	m_groundVertex[3] = V4;
	m_groundVertex[4] = V5;
	m_groundVertex[5] = V6;
	m_groundVertex[6] = V7;
	m_groundVertex[7] = V8;

}


void GlWindow::initializeGL()
{
	glClearColor(0.05, 0.05, 0.05 ,1.0);
	glEnable(GL_DEPTH_TEST);
}

void GlWindow::updatePhysics()
{
	//calculate forces on the cubes
	for(int i = 0; i < m_cubeCount; i++)
	{
		getMaxMinCube( &cubeContainer[i]);
		if(m_gravityBool)
		{
			m_gravity.updateForce(&cubeContainer[i],1);
		}
		cubeContainer[i].saveState();
		cubeContainer[i].integrate(1);

		cubeContainer[i].m_isColliding = false;
		m_collision.updateArrays(&cubeContainer[i], i);
	}


	if (m_cubeCount >= 2)
	{
		m_collision.updateAABBCollision();
		CollidingObbPairs = 0;

		for (int j = 0; j < m_collision.m_collidingAABBCount;)
		{
			if (m_collision.boxOnbox(&cubeContainer[m_collision.m_collidingAABB[j]], &cubeContainer[m_collision.m_collidingAABB[j+1]], &m_collision.m_collisionData[m_collision.m_collisionDataCount]))
			{
				cubeContainer[m_collision.m_collidingAABB[j]].m_isColliding = true;
				cubeContainer[m_collision.m_collidingAABB[j+1]].m_isColliding = true;
				CollidingObbPairs++;
			}
			m_collision.m_collisionDataCount++;
			j+=2;
		}

		int t_countX = m_collision.m_sweepCollidingCountX;
		int t_countY = m_collision.m_sweepCollidingCountY;
		int t_countZ = m_collision.m_sweepCollidingCountZ;
		bool t_isColliding;

		for(int i = 0; i < m_cubeCount; i++)
		{
			t_isColliding = false;

			for (int j = 0; j < m_collision.m_collidingAABBCount; j++)
			{
				if (i == m_collision.m_collidingAABB[j])
				{
					t_isColliding = true;
					break;
				}
			}

			//what color shoud the cube have. green - nothing happend, red - pushed, blue - colliding aabb
			if (!t_isColliding)
			{
				if (i == m_choosenCube1)
				{
					cubeContainer[i].setColor(Vector4(0,1,1,0));
				}
				else if (i == m_choosenCube2)
				{
					cubeContainer[i].setColor(Vector4(0,0.4,0.8,0));
				}
				else if (cubeContainer[i].m_isPushed)
				{
					cubeContainer[i].setColor(Vector4(1,0,0,0));
				}
				else
				{
					cubeContainer[i].setColor(Vector4(0,1,0,0));
				}
			}
			else
			{
				if (cubeContainer[i].m_isColliding)
				{
					cubeContainer[i].setColor(Vector4(1,0,1,0));
				}
				else
				{
					cubeContainer[i].setColor(Vector4(0,0,1,0));
				}
			}
		}
		m_collision.resolveCollsisions(1);
		m_collision.clarCollisionData();
		if (m_springify && m_choosenCube1 > -1 && m_choosenCube2 > -1 && m_choosenCube1 != m_choosenCube2)
		{
			float k = 2.0f;
			Vector3 x = cubeContainer[m_choosenCube2].getPosition() - cubeContainer[m_choosenCube1].getPosition();
			float xmag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
			float d = 10;
			Vector3 v =  cubeContainer[m_choosenCube2].m_velocity - cubeContainer[m_choosenCube1].m_velocity;
			float b = 0.2f;

			Vector3 x2 = Vector3(x[0]/xmag, x[1]/xmag, x[2]/xmag);
			Vector3 f = (x2) * -k * (xmag - d) - v * b;

			cubeContainer[m_choosenCube2].addForce(f);
			cubeContainer[m_choosenCube1].addForce(f*-1);
		}
	}
}

void GlWindow::updateCollision()
{
	CollidingObbPairs = 0;
	if (m_cubeCount >= 2)
	{
		for (int i = 0; i < m_cubeCount; i++)
		{
			cubeContainer[i].m_isColliding = false;
			m_collision.updateArrays(&cubeContainer[i], i);
		}
		m_collision.updateAABBCollision();

		m_collision.m_collisionDataCount = 0;
		for (int j = 0; j < m_collision.m_collidingAABBCount;)
		{
			if (m_collision.boxOnbox(&cubeContainer[m_collision.m_collidingAABB[j]], &cubeContainer[m_collision.m_collidingAABB[j+1]], &m_collision.m_collisionData[m_collision.m_collisionDataCount]))
			{
				cubeContainer[m_collision.m_collidingAABB[j]].m_isColliding = true;
				cubeContainer[m_collision.m_collidingAABB[j+1]].m_isColliding = true;
				CollidingObbPairs++;
			}
			m_collision.m_collisionDataCount++;
			j+=2;
		}
		
		int t_countX = m_collision.m_sweepCollidingCountX;
		int t_countY = m_collision.m_sweepCollidingCountY;
		int t_countZ = m_collision.m_sweepCollidingCountZ;
		bool t_isColliding;

		for(int i = 0; i < m_cubeCount; i++)
		{
			t_isColliding = false;
			
			for (int j = 0; j < m_collision.m_collidingAABBCount; j++)
			{
				if (i == m_collision.m_collidingAABB[j])
				{
					t_isColliding = true;
					break;
				}
			}

			//what color shoud the cube have. green - nothing happend, red - pushed, blue - colliding aabb
			if (!t_isColliding)
			{
				if (cubeContainer[i].m_isPushed)
				{
					cubeContainer[i].setColor(Vector4(1,0,0,0));
				}
				else
				{
					cubeContainer[i].setColor(Vector4(0,1,0,0));
				}
			}
			else
			{
				if (cubeContainer[i].m_isColliding)
				{
					cubeContainer[i].setColor(Vector4(1,1,1,0));
				}
				else
				{
					cubeContainer[i].setColor(Vector4(0,0,1,0));
				}
			}
		}

		m_collision.resolveCollsisions(1);
	}
}

void GlWindow::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,640.0/480.0,1.0,500.0);

	//Camera	
	Control(1.0f ,0.1f, m_mouseIn);
	UpdateCamera();

	//Draw
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	for(int i = 0; i < m_cubeCount; i++)
	{
		paintCube(i);
	}	

	paintOther();
}

void GlWindow::resizeGL( int width, int height )
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0, width, height);
	gluPerspective(45,640.0/480.0,1.0,500.0);
	glMatrixMode(GL_MODELVIEW);
}


void GlWindow::newCube()
{
	if (m_cubeCount < 500)
	{	
		cubeContainer[m_cubeCount] = Cube(m_cubeSize[0], m_cubeSize[1], m_cubeSize[2], m_spawnCube[0], m_spawnCube[1], m_spawnCube[2], m_cubeAwake);
		getMaxMinCube(&cubeContainer[m_cubeCount]);
		m_collision.addToArray(&cubeContainer[m_cubeCount]);
		m_cubeCount++;
	}
}

void GlWindow::paintCube(int i_index)
{	
	///cube
	glLoadIdentity();
	glLoadMatrixf((cubeContainer[i_index].m_transformMatrix).getMatrix16());

	int drawArrayCube[24] = {0,1,3,2, 0,1,5,4, 1,3,7,5, 2,3,7,6, 0,2,6,4, 4,5,7,6};
	int drawArrayLine[24] = {0,1, 1,3, 2,3, 0,2, 0,4, 1,5, 7,3, 4,5, 5,7, 7,6, 6,4, 6,2};

	Vector4 t_color = cubeContainer[i_index].getColor();
	glColor3f(t_color[0], t_color[1], t_color[2]);

	glBegin(GL_QUADS);
	for (int i = 0; i < 24; i++)
	{	
		glVertex3f(cubeContainer[i_index].getVertex(drawArrayCube[i],0),cubeContainer[i_index].getVertex(drawArrayCube[i],1),cubeContainer[i_index].getVertex(drawArrayCube[i],2));
	}
	glEnd();

	///lines
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	for (int i = 0; i < 24; i++)
	{	
		glVertex3f(cubeContainer[i_index].getVertex(drawArrayLine[i],0),cubeContainer[i_index].getVertex(drawArrayLine[i],1),cubeContainer[i_index].getVertex(drawArrayLine[i],2));
	}
	glEnd();

	//physics line
	glLoadIdentity();
	glColor3f(0.0, 0.0, 1.0);

	float t_maxX = cubeContainer[i_index].m_maxVert[0];
	float t_maxY = cubeContainer[i_index].m_maxVert[1];
	float t_maxZ = cubeContainer[i_index].m_maxVert[2];

	float t_minX = cubeContainer[i_index].m_minVert[0];
	float t_minY = cubeContainer[i_index].m_minVert[1];
	float t_minZ = cubeContainer[i_index].m_minVert[2];

	glBegin(GL_LINE_LOOP);
	glVertex3f(t_maxX, t_maxY, t_minZ);
	glVertex3f(t_minX, t_maxY, t_minZ);
	glVertex3f(t_minX, t_minY, t_minZ);
	glVertex3f(t_maxX, t_minY, t_minZ);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(t_maxX, t_minY, t_maxZ);
	glVertex3f(t_maxX, t_maxY, t_maxZ);
	glVertex3f(t_minX, t_maxY, t_maxZ);
	glVertex3f(t_minX, t_minY, t_maxZ);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(t_maxX, t_maxY, t_minZ);
	glVertex3f(t_maxX, t_maxY, t_maxZ);
	glVertex3f(t_minX, t_maxY, t_maxZ);
	glVertex3f(t_minX, t_maxY, t_minZ);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(t_maxX, t_minY, t_maxZ);
	glVertex3f(t_minX, t_minY, t_maxZ);
	glVertex3f(t_minX, t_minY, t_minZ);
	glVertex3f(t_maxX, t_minY, t_minZ);
	glEnd();
}

void GlWindow::paintOther()
{

	///Draw Lines
	glLoadIdentity();
	glTranslatef(0.0, 0.0, 0.0);
	glBegin(GL_LINES);

	glLoadIdentity();
	glColor3f(1, 0, 0);
	glVertex3f(0,0.2,0);
	glVertex3f(2,0.2,0);
	glLoadIdentity();
	glColor3f(0, 1, 0);
	glVertex3f(0,0.2,0);
	glVertex3f(0,2.0,0);
	glLoadIdentity();
	glColor3f(0, 0, 1);
	glVertex3f(0,0.2,0);
	glVertex3f(0,0.2,2);

	glColor3f(0.3, 0, 0.3);
	for(int i = 0; i < 100; i++)
	{
		glVertex3f(i*2,0.0,200);
		glVertex3f(i*2,0.0,-200);

		glVertex3f(i*-2,0.0,200);
		glVertex3f(i*-2,0.0,-200);

		glVertex3f(200,0.0,i*2);
		glVertex3f(-200,0.0,i*2);

		glVertex3f(200,0.0,i*-2);
		glVertex3f(-200,0.0,i*-2);
	}

	if (m_raycastDone)
	{
		glLoadIdentity();
		glColor3f(1,1,0);
		glVertex3f(m_raycastStart[0], m_raycastStart[1], m_raycastStart[2]);
		glVertex3f(m_raycastEnd[0], m_raycastEnd[1], m_raycastEnd[2]);
	}

	glEnd();

	if (m_rayHit)
	{
		glPointSize(10);
		glBegin(GL_POINTS);

		glLoadIdentity();
		glVertex3f(m_rayHitPos[0],m_rayHitPos[1],m_rayHitPos[2]);

		glEnd();
	}

	for (int i = 0; i < CollidingObbPairs; i++)
	{
		Vector3 t_emp1 = m_collision.m_collisionData[i].m_contactPoint;
		Vector3 t_emp2 = m_collision.m_collisionData[i].m_contactNormal;
		glColor3f(1,0,1);
		glBegin(GL_POINTS);
		glLoadIdentity();
		glVertex3f(t_emp1[0],t_emp1[1],t_emp1[2]);
		glEnd();

		
		glPointSize(10);
		glBegin(GL_LINES);
		glLoadIdentity();
		glVertex3f(t_emp1[0],t_emp1[1],t_emp1[2]);
		glVertex3f(t_emp1[0] + t_emp2[0]*2, t_emp1[1] + t_emp2[1]*2, t_emp1[2] + t_emp2[2]*2);
		glEnd();
	}
}

void GlWindow::lockCamera()
{
	if (m_camPitch > 90)
	{
		m_camPitch = 90;
	}
	if (m_camPitch < -90)
	{
		m_camPitch = -90;
	}
	if (m_camYaw < 0.0)
	{
		m_camYaw += 360;
	}
	if (m_camYaw > 360)
	{
		m_camYaw -= 360;
	}
}

void GlWindow::moveCamera(float i_dist, float i_dir)
{
	float rad = (m_camYaw+i_dir) * m_pi/180.0;
	m_camX-=sin(rad)*i_dist;
	m_camZ-=cos(rad)*i_dist;
}

void GlWindow::moveCameraUp(float i_dist, float i_dir)
{
	if(i_dir == 1.0)
	{
		m_camY+=i_dist;
	}
	else if (i_dir = -1.0)
	{
		m_camY-=i_dist;
	}
}
void GlWindow::Control(float i_moveVel, float i_mouseVel, bool i_mi)
{
	m_moveVel = i_moveVel; 

	if(i_mi)
	{
		QPoint t_screenRes = mapToGlobal(QPoint(width()/2,height()/2));

		POINT t_mousePos;
		GetCursorPos(&t_mousePos);

		m_camYaw += i_mouseVel * (t_screenRes.rx()-t_mousePos.x);	
		m_camPitch += i_mouseVel * (t_screenRes.ry()-t_mousePos.y);

		lockCamera();
		SetCursorPos(t_screenRes.rx(), t_screenRes.ry());
	}

	glRotatef(-m_camPitch, 1.0, 0.0, 0.0);
	glRotatef(-m_camYaw, 0.0, 1.0, 0.0);
}

void GlWindow::UpdateCamera()
{
	glTranslatef(-m_camX, -m_camY, -m_camZ);

	Matrix4 m_traMat = Matrix4(4,4);
	m_traMat[0][3] = m_camX;
	m_traMat[1][3] = m_camY;
	m_traMat[2][3] = m_camZ;

	Matrix4 t_rotmat = (t_rotmat.rotateAxis(m_camPitch, "x") * t_rotmat.rotateAxis(m_camYaw, "y"));
	
	m_camMat = m_traMat*t_rotmat;
}

void GlWindow::Raycast()
{
	int t_rayCastDist = 1000;

	///raycast line
	Matrix4 t_rayMat = Matrix4(4,4);
	t_rayMat[2][3] = -1 * t_rayCastDist;

	Matrix4 t_transMat = Matrix4(4,4);
	t_transMat[0][3] = m_camX;
	t_transMat[1][3] = m_camY;
	t_transMat[2][3] = m_camZ;

	Matrix4 t_rotXMat;
	t_rotXMat = t_rotXMat.rotateAxis(m_camPitch, "x");
	Matrix4 t_rotYMat;
	t_rotYMat = t_rotYMat.rotateAxis(m_camYaw, "y");
	Matrix4 t_i_rotmat;
	t_i_rotmat = t_rotYMat*t_rotXMat;

	Matrix4 t_camMat;
	t_camMat = t_transMat*t_i_rotmat;

	Matrix4 t_final = Matrix4(4,4);
	t_final = t_camMat*t_rayMat;

	m_raycastEnd = Vector3(t_final[0][3], t_final[1][3], t_final[2][3]);
	m_raycastStart = Vector3(m_camX, m_camY, m_camZ);

	Vector3 t_dir = m_raycastEnd - m_raycastStart;
	t_dir = t_dir * m_force;

	int t_indexArray[20];
	int t_arrayIndex = 0; 

	for(int i = 0; i < m_cubeCount; i++)
	{
		getMaxMinCube(&cubeContainer[i]);
		m_rayHit = Line_AABB(m_raycastStart, m_raycastEnd, cubeContainer[i].m_maxVert, cubeContainer[i].m_minVert, m_rayHitPos); 
		if(m_rayHit)
		{
			t_indexArray[t_arrayIndex++] = i;
		}
	}
	float t_lowestDistance = 999999.9f;
	int t_bodyIndex = -1;
	for (int i = 0; i < t_arrayIndex; i++)
	{
		float t_temp = sqrt(pow(m_camX - cubeContainer[t_indexArray[i]].getPosition()[0],2) + pow(m_camY - cubeContainer[t_indexArray[i]].getPosition()[1],2) + pow(m_camZ - cubeContainer[t_indexArray[i]].getPosition()[2],2));
		if (t_temp < t_lowestDistance)
		{
			t_lowestDistance = t_temp;
			t_bodyIndex = t_indexArray[i];
		}
	}
	if (t_bodyIndex != -1)
	{
		m_choosenCube2 = m_choosenCube1;
		m_choosenCube1 = t_bodyIndex;
	}

}

void GlWindow::rayCast2()
{
	int t_rayCastDist = 1000;

	///raycast line
	Matrix4 t_rayMat = Matrix4(4,4);
	t_rayMat[2][3] = -1 * t_rayCastDist;
	
	Matrix4 t_transMat = Matrix4(4,4);
	t_transMat[0][3] = m_camX;
	t_transMat[1][3] = m_camY;
	t_transMat[2][3] = m_camZ;

	Matrix4 t_rotXMat;
	t_rotXMat = t_rotXMat.rotateAxis(m_camPitch, "x");
	Matrix4 t_rotYMat;
	t_rotYMat = t_rotYMat.rotateAxis(m_camYaw, "y");
	Matrix4 t_i_rotmat;
	t_i_rotmat = t_rotYMat*t_rotXMat;

	Matrix4 t_camMat;
	t_camMat = t_transMat*t_i_rotmat;

	Matrix4 t_final = Matrix4(4,4);
	t_final = t_camMat*t_rayMat;

	m_raycastEnd = Vector3(t_final[0][3], t_final[1][3], t_final[2][3]);
	m_raycastStart = Vector3(m_camX, m_camY, m_camZ);

	Vector3 t_dir = m_raycastEnd - m_raycastStart;
	t_dir = t_dir * m_force;
	
	int t_indexArray[20];
	int t_arrayIndex = 0; 

	for(int i = 0; i < m_cubeCount; i++)
	{
		getMaxMinCube(&cubeContainer[i]);
		m_rayHit = Line_AABB(m_raycastStart, m_raycastEnd, cubeContainer[i].m_maxVert, cubeContainer[i].m_minVert, m_rayHitPos); 
		if(m_rayHit)
		{
			t_indexArray[t_arrayIndex++] = i;
		}
	}
	float t_lowestDistance = 999999.9f;
	int t_bodyIndex = -1;
	for (int i = 0; i < t_arrayIndex; i++)
	{
		float t_temp = sqrt(pow(m_camX - cubeContainer[t_indexArray[i]].getPosition()[0],2) + pow(m_camY - cubeContainer[t_indexArray[i]].getPosition()[1],2) + pow(m_camZ - cubeContainer[t_indexArray[i]].getPosition()[2],2));
		if (t_temp < t_lowestDistance)
		{
			t_lowestDistance = t_temp;
			t_bodyIndex = t_indexArray[i];
		}
	}
	if (t_bodyIndex != -1)
	{
		Vector3 t_min = Vector3(m_raycastEnd[0]-m_rayHitPos[0], m_raycastEnd[1]-m_rayHitPos[1], m_raycastEnd[2]-m_rayHitPos[2]); //new
		cubeContainer[t_bodyIndex].m_isPushed = true;
		cubeContainer[t_bodyIndex].addForceAtBodyPoint(m_force, m_rayHitPos, t_min);
	}
	

	m_raycastDone = true;
}

void GlWindow::getMaxMinCube(Cube *i_body)
{
	Vector4 t_emp4;
	int count = 0;
	for(int i = 0; i < 8; i++)
	{
		t_emp4 = Vector4(i_body->getVertex(i,0), i_body->getVertex(i,1), i_body->getVertex(i,2), 1);
		t_emp4 = ~i_body->m_transformMatrix*t_emp4;
		i_body->m_vertWorld[count++] = t_emp4[0];
		i_body->m_vertWorld[count++] = t_emp4[1];
		i_body->m_vertWorld[count++] = t_emp4[2];

	}
	Vector3 t_max = Vector3(-9999999.9f,-9999999.9f,-9999999.9f);
	Vector3 t_min = Vector3(9999999.9f, 9999999.9f, 9999999.9f);
	count = 0;
	for(int i = 0; i < 8; i++)
	{
		//X
		if(i_body->m_vertWorld[count] > t_max[0])
		{
			t_max[0] = i_body->m_vertWorld[count];
		}
		if(i_body->m_vertWorld[count] < t_min[0])
		{
			t_min[0] = i_body->m_vertWorld[count];
		}
		count++;

		//Y
		if(i_body->m_vertWorld[count] > t_max[1])
		{
			t_max[1] = i_body->m_vertWorld[count];
		}
		if(i_body->m_vertWorld[count] < t_min[1])
		{
			t_min[1] = i_body->m_vertWorld[count];
		}
		count++;

		//Z
		if(i_body->m_vertWorld[count] > t_max[2])
		{
			t_max[2] = i_body->m_vertWorld[count];
		}
		if(i_body->m_vertWorld[count] < t_min[2])
		{
			t_min[2] = i_body->m_vertWorld[count];
		}
		count++;
	}

	if(t_max[0] < t_min[0])
	{
		float t_tempMax = t_max[0];
		t_max[0] = t_min[0];
		t_min[0] = t_tempMax;
	}
	if(t_max[1] < t_min[1])
	{
		float t_tempMax = t_max[1];
		t_max[1] = t_min[1];
		t_min[1] = t_tempMax;
	}
	if(t_max[2] < t_min[2])
	{
		float t_tempMax = t_max[2];
		t_max[2] = t_min[2];
		t_min[2] = t_tempMax;
	}

	i_body->m_maxVert = t_max;
	i_body->m_minVert = t_min;
 }

bool GlWindow::Line_AABB(Vector3 &i_start, Vector3 &i_end, Vector3 &i_min, Vector3 &i_max, Vector3 &i_hitPoint)
{
	float t_enter = 0.0f;
	float t_exit = 1.0f;
	Vector3 t_dir = i_end - i_start;

	if(!Line_AABB_1d(i_start[0], t_dir[0], i_min[0], i_max[0], t_enter, t_exit))
	{
		return false;
	}
	if(!Line_AABB_1d(i_start[1], t_dir[1], i_min[1], i_max[1], t_enter, t_exit))
	{
		return false;
	}
	if(!Line_AABB_1d(i_start[2], t_dir[2], i_min[2], i_max[2], t_enter, t_exit))
	{
		return false;
	}
	i_hitPoint = i_start + t_dir * t_enter;
	cout << "Hit at: " << i_hitPoint[0] << " " << i_hitPoint[1] << " " << i_hitPoint[2] << endl;
	return true;
}

bool GlWindow::Line_AABB_1d(float i_start, float i_dir, float i_min, float i_max, float &i_enter, float &i_exit)
{
	float t_ooDir = 1.0f / i_dir;
	float t_0 = (i_min - i_start) * t_ooDir;
	float t_1 = (i_max - i_start) * t_ooDir;

	if(t_0 > t_1)
	{
		float t_tmp = t_0;
		t_0 = t_1;
		t_1 = t_tmp;
	}

	if(t_0 > i_exit || t_1 < i_enter)
	{
		return false;
	}

	if(t_0 > i_enter)
	{
		i_enter = t_0;
	}
	if(t_1 < i_exit)
	{
		i_exit = t_1;
	}

	return true;
}

void GlWindow::getKey(int i_key)
{
	switch(i_key)
	{
	case 1: ///W
	{
		//forward
		moveCamera(m_moveVel, 0.0);
		break;
	}
	case 2: ///S
	{
		//backwords
		moveCamera(m_moveVel, 180.0);
		break;

	}
	case 3: ///A
	{
		//left
		moveCamera(m_moveVel, 90);
		break;

	}
	case 4: ///D
	{
		//right
		moveCamera(m_moveVel, 270);
		break;
	}
	case 5:	 ///E
	{
		//Up
		moveCameraUp(m_moveVel, 1.0);
		break;
	}
	case 6: ///Q
	{
		//Down
		moveCameraUp(m_moveVel, -1.0);
		break;
	}
	case 7: ///C
	{
		newCube();
		break;
	}
	case 8: ///Space
	{
		rayCast2();
		break;
	}
	case 9: ///G
	{
		m_gravityBool = !m_gravityBool;
		if(m_gravityBool)
		{
			cout << "Gravity: ON" << endl;
		}
		else
		{
			cout << "Gravity: OFF" << endl;
		}
		break;
	}	
	case 10: ///R
	{
		m_cubeAwake = !m_cubeAwake;
		for(int i = 0; i < m_cubeCount; i++)
		{
			cubeContainer[i].setAwake(m_cubeAwake);
		}
		if(m_cubeAwake)
		{
			cout << "Awake: ON" << endl;
		}
		else
		{
			cout << "Awake: OFF" << endl;
		}
		break;
	}
	case 11: ///Plus
	{
		m_force += 0.1f;
		cout << "Force: " << m_force << endl;
		break;
	}
	case 12: ///Minus
	{
		m_force -= 0.1f;
		cout << "Force: " << m_force << endl;
		break;
	}
	case 13: ///UP Arrow
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changePosition(-m_force*10,2);
		}
		break;
	}
	case 14: ///Down Arrow
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changePosition(m_force*10,2);
		}
		break;
	}
	case 15: ///Left Arrow
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changePosition(-m_force*10,0);
		}
		break;
	}
	case 16: ///Right Arrow
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changePosition(m_force*10,0);
		}
		break;
	}
	case 17: //F
	{
		if (!m_wireframeON)
		{
			//wireFrame ON
			glPolygonMode(GL_FRONT, GL_LINE);
			glPolygonMode(GL_BACK, GL_LINE);
			glEnable(GL_CULL_FACE);
			m_wireframeON = !m_wireframeON;
			cout << "WireFrame ON" << endl;
		}
		else
		{
			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK, GL_FILL);
			glDisable(GL_CULL_FACE);
			m_wireframeON = !m_wireframeON;
			cout << "WireFrame OFF" << endl;
		}
	}
	case 18: //X
	{
		Raycast();
	}
	case 19: //K
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changePosition(10,1);
		}
		break;
	}
	case 20: //L
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changePosition(-10,1);
		}
		break;
	}
	case 21: //1
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changeRotation(10,0);
		}
		break;
	}
	case 22: //2
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changeRotation(-10,0);
		}
		break;
	}
	case 23: //3
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changeRotation(10,1);
		}
		break;
	}
	case 24: //4
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changeRotation(-10,1);
		}
		break;
	}
	case 25: //5
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changeRotation(10,2);
		}
		break;
	}
	case 26: //6
	{
		if (m_choosenCube1 != -1)
		{
			cubeContainer[m_choosenCube1].changeRotation(-10,2);
		}
		break;
	}
	case 27: //Z
	{
		m_springify = !m_springify;
		if (m_springify)
			cout << "Spring ON" << endl;
		else
			cout << "Spring OFF" << endl;
	}
	}

}

 void GlWindow::mousePressEvent(QMouseEvent *event)
 {
	 QPoint t_screenRes = mapToGlobal(QPoint(width()/2,height()/2));
	 SetCursorPos(t_screenRes.rx(), t_screenRes.ry());

	GetCursorPos(&m_mousePos);
	m_mouseIn = true;
	ShowCursor(false);
 }
 
 void GlWindow::mouseReleaseEvent(QMouseEvent *event)
 {
	 m_mouseIn = false;
	 ShowCursor(true);
 }