#include "s0009d.h"

S0009D::S0009D(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	m_xBox = this->findChild<QSpinBox*>("spinBoxX");
	m_yBox = this->findChild<QSpinBox*>("spinBoxY");
	m_zBox = this->findChild<QSpinBox*>("spinBoxZ");

	m_sxBox = this->findChild<QSpinBox*>("spinBoxSX");
	m_syBox = this->findChild<QSpinBox*>("spinBoxSY");
	m_szBox = this->findChild<QSpinBox*>("spinBoxSZ");
}

S0009D::~S0009D()
{

}

void S0009D::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_W)
	{
		//forward
		ui.widget->getKey(1);
	}
	else if(event->key() == Qt::Key_S)
	{
		//backwords
		ui.widget->getKey(2);
	}
	else if(event->key() == Qt::Key_A)
	{
		//left
		ui.widget->getKey(3);
	}
	else if(event->key() == Qt::Key_D)
	{
		//right
		ui.widget->getKey(4);
	}
	else if(event->key() == Qt::Key_E)
	{
		ui.widget->getKey(5);
	}
	else if(event->key() == Qt::Key_Q)
	{
		ui.widget->getKey(6);
	}
	else if(event->key() == Qt::Key_C)
	{
		if (m_sxBox->value() > 0 && m_syBox->value() > 0 && m_szBox->value() > 0)
		{
			ui.widget->m_spawnCube = Vector3(m_xBox->value(), m_yBox->value(), m_zBox->value());
			ui.widget->m_cubeSize = Vector3(m_sxBox->value(), m_syBox->value(), m_szBox->value());
			ui.widget->getKey(7);
		}
	}
	else if (event->key() == Qt::Key_Space)
	{
		ui.widget->getKey(8);
	}
	else if (event->key() == Qt::Key_G)
	{
		ui.widget->getKey(9);
	}
	else if (event->key() == Qt::Key_R)
	{
		ui.widget->getKey(10);
	}
	else if (event->key() == Qt::Key_Plus)
	{
		ui.widget->getKey(11);
	}
	else if (event->key() == Qt::Key_Minus)
	{
		ui.widget->getKey(12);
	}
	else if (event->key() == Qt::Key_Up)
	{
		ui.widget->getKey(13);
	}
	else if (event->key() == Qt::Key_Down)
	{
		ui.widget->getKey(14);
	}
	else if (event->key() == Qt::Key_Left)
	{
		ui.widget->getKey(15);
	}
	else if (event->key() == Qt::Key_Right)
	{
		ui.widget->getKey(16);
	}
	else if (event->key() == Qt::Key_F)
	{
		ui.widget->getKey(17);
	}
	else if (event->key() == Qt::Key_X)
	{
		ui.widget->getKey(18);
	}
	else if (event->key() == Qt::Key_K)
	{
		ui.widget->getKey(19);
	}
	else if (event->key() == Qt::Key_L)
	{
		ui.widget->getKey(20);
	}
	else if (event->key() == Qt::Key_1)
	{
		ui.widget->getKey(21);
	}
	else if (event->key() == Qt::Key_2)
	{
		ui.widget->getKey(22);
	}
	else if (event->key() == Qt::Key_3)
	{
		ui.widget->getKey(23);
	}
	else if (event->key() == Qt::Key_4)
	{
		ui.widget->getKey(24);
	}
	else if (event->key() == Qt::Key_5)
	{
		ui.widget->getKey(25);
	}
	else if (event->key() == Qt::Key_6)
	{
		ui.widget->getKey(26);
	}
	else if (event->key() == Qt::Key_Z)
	{
		ui.widget->getKey(27);
	}
	
}