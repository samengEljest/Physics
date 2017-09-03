#pragma once

#include <QGLFunctions>
#include <QtOpenGL\QGLWidget>
#include <QGLWidget>
#include <QtOpenGL>
#include <QTimer>
#include <GL/glu.h>
#include <QMouseEvent>
#include <math.h>

//#include <QKeyEvent>
#include <QMouseEvent>

#include "cube.h"
#include "collision.h"
#include "gravity.h"
#include "Matrix4.h"
#include "Vector4.h"

/*!
	this is the class that handels the OpenGl window
*/
class GlWindow : public QGLWidget
{
	Q_OBJECT
public:
	GlWindow(QWidget* parent = 0); ///< opens the openGL window with the chosen file, sets the size to the picture
	~GlWindow();

	/*float* camraRotate(float theta,std::string axis);*/
	Vector3 m_spawnCube;
	Vector3 m_cubeSize;
	
private:
 	QTimer m_timer;
	QTimer m_UpdatePhy;
	QTimer m_UpdateCol;
	Cube cubeContainer[200];
	Gravity m_gravity;
	Collision m_collision;
	int m_cubeCount;
 	float m_camYaw;
 	float m_camPitch;
 	float m_camX, m_camY, m_camZ;
	bool m_mouseIn;
	float m_pi;
	bool m_raycastDone;
	float m_force;
	float m_cubeWidth;
	float m_cubeHeight;
	float m_cubeDepth;

	float m_moveVel;
	POINT m_mousePos;

	Vector4 m_groundVertex[8];
	Vector3 m_raycastStart;
	Vector3 m_raycastEnd;
	Vector3 m_rayHitPos;
	Matrix4 m_camMat;
	bool m_rayHit;
	bool m_cubeAwake;
	bool m_gravityBool;
	bool m_springify;

	int CollidingObbPairs;

	bool m_wireframeON;
	int m_choosenCube1;
	int m_choosenCube2;
	


protected:
	void initializeGL();
	void paintGL(); ///< paints the picture on the openGL window
	void resizeGL(int width, int height);

	void doBeginning();

	void newCube();
	void paintCube(int i_index);
	void paintOther();
	
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void lockCamera();
	void moveCamera(float i_dist, float i_dir);
	void moveCameraUp(float i_dist, float i_dir);
	void Control(float i_moveVel, float i_mouseVel, bool i_mi);
	void UpdateCamera();

	void Raycast(); //select body at end
	void rayCast2(); //add force at end
	bool Line_AABB(Vector3 &i_start, Vector3 &i_end, Vector3 &i_min, Vector3 &i_max, Vector3 &i_hitPoint);
	bool Line_AABB_1d(float i_start, float i_dir, float i_min, float i_max, float& i_enter, float& i_exit);
	void getMaxMinCube(Cube *i_body);

public slots:
	void getKey(int i_key);

private slots:
		void updatePhysics();
		void updateCollision();
};
