#pragma once

#include "Core/Camera.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

class CameraController
{
public:
	CameraController(Camera& camera, int width, int height);
	~CameraController();

	void bindCamera(Camera& camera);
	void onResize(int width, int height);

	virtual void keyPressEvent(QKeyEvent* event) = 0;
	virtual void keyReleaseEvent(QKeyEvent* event) = 0;
	virtual void mouseMoveEvent(QMouseEvent* event) = 0;
	virtual void mousePressEvent(QMouseEvent* event) = 0;
	virtual void mouseReleaseEvent(QMouseEvent* event) = 0;
	virtual void wheelEvent(QWheelEvent* event) = 0;

protected:
	Camera* _camera;
	int _width;
	int _height;
};
