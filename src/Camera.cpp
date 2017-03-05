#include "Camera.h"

Camera::Camera(const QVector3D& position, const QVector3D& focus, const QVector3D& up, float fov, float aspect)
	: _fov(fov), _aspect(aspect)
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

QVector3D Camera::focus() const
{
	return _focus;
}

QVector3D Camera::direction() const
{
	return _direction;
}

QVector3D Camera::up() const
{
	return _up;
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

float Camera::aspect() const
{
	return _aspect;
}

void Camera::setAspect(float aspect)
{
	_aspect = aspect;
}
