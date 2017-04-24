#pragma once

#include "Editor/CameraController.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QVector2D>

#ifdef _WIN32
#pragma warning(pop)
#endif

enum class ControllerMode
{
	Free,
	Rotate,
	Pan
};

class ArcballController : public CameraController
{
public:
	ArcballController(Camera& camera, int width, int height);
	~ArcballController();

	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
	float _yawSpeed = -0.3f;
	float _pitchSpeed = -0.3f;
	float _scaleSpeed = 0.1f;
	float _panSpeed = 0.003f;
	QVector2D _lastMousePos;
	ControllerMode _mode = ControllerMode::Free;
};
