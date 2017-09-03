#include "Core/Camera.h"

Camera::Camera(const QVector3D& position, const QVector3D& focus, const QVector3D& up,
	float fov, float nearPlane, float farPlane)
	: _fov(fov), _nearPlane(nearPlane), _farPlane(farPlane)
{
	lookAt(position, focus, up);
}

Camera::~Camera() = default;

void Camera::lookAt(const QVector3D& position, const QVector3D& focus, const QVector3D& up)
{
	_position = position;
	_focus = focus;
	_direction = (position - focus).normalized();
	_right = QVector3D::crossProduct(up.normalized(), _direction);
	_up = QVector3D::crossProduct(_direction, _right);
	if (_up.y() >= 0.0f) {
		_worldUp = QVector3D(0.0f, 1.0f, 0.0f);
	}
	else {
		_worldUp = QVector3D(0.0f, -1.0f, 0.0f);
	}
	_invViewDirty = true;
	_view.setToIdentity();
	_view.lookAt(_position, _focus, _up);
}

QMatrix4x4 Camera::view() const
{
	return _view;
}

QMatrix4x4 Camera::invView() const
{
	if (_invViewDirty) {
		_invView = _view.inverted();
		_invViewDirty = false;
	}
	return _invView;
}

QVector3D Camera::position() const
{
	return _position;
}

void Camera::setPosition(const QVector3D& pos)
{
	_position = pos;
	_direction = (_position - _focus).normalized();
	_right = QVector3D::crossProduct(_up, _direction);
	_up = QVector3D::crossProduct(_direction, _right);
	_invViewDirty = true;
	_view.setToIdentity();
	_view.lookAt(_position, _focus, _up);
}

QVector3D Camera::focus() const
{
	return _focus;
}

void Camera::setFocus(const QVector3D& focus)
{
	_focus = focus;
	_direction = (_position - _focus).normalized();
	_right = QVector3D::crossProduct(_up, _direction);
	_up = QVector3D::crossProduct(_direction, _right);
	_invViewDirty = true;
	_view.setToIdentity();
	_view.lookAt(_position, _focus, _up);
}

QVector3D Camera::direction() const
{
	return _direction;
}

QVector3D Camera::up() const
{
	return _up;
}

void Camera::setUp(const QVector3D& up)
{
	_right = QVector3D::crossProduct(up.normalized(), _direction);
	_up = QVector3D::crossProduct(_direction, _right);
	if (_up.y() >= 0.0f) {
		_worldUp = QVector3D(0.0f, 1.0f, 0.0f);
	}
	else {
		_worldUp = QVector3D(0.0f, -1.0f, 0.0f);
	}
	_invViewDirty = true;
	_view.setToIdentity();
	_view.lookAt(_position, _focus, _up);
}

QVector3D Camera::worldUp() const
{
	return _worldUp;
}

QVector3D Camera::right() const
{
	return _right;
}

float Camera::fov() const
{
	return _fov;
}

void Camera::setFov(float fov)
{
	_fov = fov;
}

float Camera::farPlane() const
{
	return _farPlane;
}

void Camera::setFarPlane(float far)
{
	_farPlane = far;
}

float Camera::nearPlane() const
{
	return _nearPlane;
}

void Camera::setNearPlane(float near)
{
	_nearPlane = near;
}
