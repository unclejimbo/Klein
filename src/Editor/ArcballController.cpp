#include "Editor/ArcballController.h"

#ifdef _WIN32
#include <CodeAnalysis/Warnings.h>
#pragma warning(push)
#pragma warning(disable : ALL_CODE_ANALYSIS_WARNINGS)
#endif

#include <QMatrix4x4>

#ifdef _WIN32
#pragma warning(pop)
#endif

ArcballController::ArcballController(Camera& camera, int width, int height)
	: CameraController(camera, width, height)
{
}

ArcballController::~ArcballController() = default;

void ArcballController::keyPressEvent(QKeyEvent* event)
{
	Q_UNUSED(event);
}

void ArcballController::keyReleaseEvent(QKeyEvent* event)
{
	Q_UNUSED(event);
}

void ArcballController::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MiddleButton && event->modifiers() == Qt::NoModifier) {
		_lastMousePos = QVector2D(event->x() + 0.0f, event->y() + 0.0f);
		_mode = ControllerMode::Rotate;
	}
	if (event->button() == Qt::MiddleButton && event->modifiers() == Qt::ShiftModifier) {
		_lastMousePos = QVector2D(event->x() + 0.0f, event->y() + 0.0f);
		_mode = ControllerMode::Pan;
	}
}

void ArcballController::mouseMoveEvent(QMouseEvent* event)
{
	if (_mode == ControllerMode::Rotate) {
		auto mousePos = QVector2D(event->x() + 0.0f, event->y() + 0.0f);

		QMatrix4x4 rotation;
		rotation.setToIdentity();
		rotation.rotate((mousePos.x() - _lastMousePos.x()) * _yawSpeed, _camera->up()); // Tilt enabled
		//rotation.rotate((mousePos.x() - _lastMousePos.x()) * _yawSpeed, _camera->worldUp()); // Tilt disabled
		rotation.rotate((mousePos.y() - _lastMousePos.y()) * _pitchSpeed, _camera->right());

		auto camFocus = _camera->focus();
		auto camPosition = _camera->position();
		auto camDirection = camPosition - camFocus;
		auto camDirectionRotated = rotation * camDirection;
		auto camPositionRotated = camFocus + camDirectionRotated;
		auto camRightRotated = rotation * _camera->right();
		auto camUpRotated = QVector3D::crossProduct(camDirectionRotated.normalized(), camRightRotated);
		_camera->lookAt(camPositionRotated, camFocus, camUpRotated);

		_lastMousePos = mousePos;
	}

	if (_mode == ControllerMode::Pan) {
		auto mousePos = QVector2D(event->x() + 0.0f, event->y() + 0.0f);
		
		auto direction = (_lastMousePos.x() - mousePos.x()) * _camera->right() +
			(mousePos.y() - _lastMousePos.y()) * _camera->up();
		auto camPosition = _camera->position() + direction * _panSpeed;
		auto camFocus = _camera->focus() + direction * _panSpeed;
		_camera->lookAt(camPosition, camFocus, _camera->up());

		_lastMousePos = mousePos;
	}
}

void ArcballController::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::MiddleButton) {
		_mode = ControllerMode::Free;
	}
}

void ArcballController::wheelEvent(QWheelEvent* event)
{
	if (_mode == ControllerMode::Free && event->modifiers() == Qt::NoModifier) {
		auto distance = (_camera->position() - _camera->focus()).length();
		if (event->angleDelta().y() > 0.0f) {
			auto camPositionScaled = _camera->position() - distance * _scaleSpeed * _camera->direction();
			_camera->lookAt(camPositionScaled, _camera->focus(), _camera->up());
		}
		if (event->angleDelta().y() < 0.0f) {
			auto camPositionScaled = _camera->position() + distance * _scaleSpeed * _camera->direction();
			_camera->lookAt(camPositionScaled, _camera->focus(), _camera->up());
		}
	}
	if (_mode == ControllerMode::Free && event->modifiers() == Qt::ShiftModifier) {
		if (event->angleDelta().y() > 0.0f) {
			auto camPositionTranslated = _camera->position() + _scaleSpeed * _camera->up();
			auto camFocusTranslated = _camera->focus() + _scaleSpeed * _camera->up();
			_camera->lookAt(camPositionTranslated, camFocusTranslated, _camera->up());
		}
		if (event->angleDelta().y() < 0.0f) {
			auto camPositionTranslated = _camera->position() - _scaleSpeed * _camera->up();
			auto camFocusTranslated = _camera->focus() - _scaleSpeed * _camera->up();
			_camera->lookAt(camPositionTranslated, camFocusTranslated, _camera->up());
		}
	}
	if (_mode == ControllerMode::Free && event->modifiers() == Qt::ControlModifier) {
		if (event->angleDelta().y() > 0.0f) {
			auto camPositionTranslated = _camera->position() + _scaleSpeed * _camera->right();
			auto camFocusTranslated = _camera->focus() + _scaleSpeed * _camera->right();
			_camera->lookAt(camPositionTranslated, camFocusTranslated, _camera->up());
		}
		if (event->angleDelta().y() < 0.0f) {
			auto camPositionTranslated = _camera->position() - _scaleSpeed * _camera->right();
			auto camFocusTranslated = _camera->focus() - _scaleSpeed * _camera->right();
			_camera->lookAt(camPositionTranslated, camFocusTranslated, _camera->up());
		}
	}
}
