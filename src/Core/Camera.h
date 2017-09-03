#pragma once

#include <QMatrix4x4>
#include <QVector3D>

class Camera
{
public:
	Camera(const QVector3D& position, const QVector3D& focus, const QVector3D& up,
		float fov, float nearPlane = 0.001f, float farPlane = 1000.0f);
	virtual ~Camera();

	void lookAt(const QVector3D& position, const QVector3D& focus, const QVector3D& up);
	QMatrix4x4 view() const;
	QMatrix4x4 invView() const;
	QVector3D position() const;
	void setPosition(const QVector3D& pos);
	QVector3D focus() const;
	void setFocus(const QVector3D& focus);
	QVector3D direction() const;
	QVector3D up() const;
	void setUp(const QVector3D& up);
	QVector3D worldUp() const;
	QVector3D right() const;
	float fov() const;
	void setFov(float fov);
	float farPlane() const;
	void setFarPlane(float far);
	float nearPlane() const;
	void setNearPlane(float near);

protected:
	QMatrix4x4 _view;
	mutable QMatrix4x4 _invView;
	QVector3D _position;
	QVector3D _focus;
	QVector3D _direction;
	QVector3D _up;
	QVector3D _worldUp;
	QVector3D _right;
	float _fov;
	float _nearPlane;
	float _farPlane;
	mutable bool _invViewDirty = true;
};
