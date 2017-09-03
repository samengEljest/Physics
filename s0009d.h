#ifndef S0009D_H
#define S0009D_H

#include <QtGui/QMainWindow>
#include "ui_s0009d.h"
#include "Vector3.h"
#include <QMouseEvent>
#include <QWidget>

class S0009D : public QMainWindow
{
	Q_OBJECT

public:
	S0009D(QWidget *parent = 0, Qt::WFlags flags = 0);
	~S0009D();

private:
	Ui::Form ui;
	QSpinBox* m_xBox;
	QSpinBox* m_yBox;
	QSpinBox* m_zBox;

	QSpinBox* m_sxBox;
	QSpinBox* m_syBox;
	QSpinBox* m_szBox;

protected:
	void keyPressEvent(QKeyEvent *);

signals:
	void sendKey(int);


};

#endif // S0009D_H
