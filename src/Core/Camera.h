#pragma once

#include <QMatrix4x4>
#include <QVector3D>

class Camera
{
public:
	Camera(const QVector3D& position, const QVector3D& focus, const QVector3D& up, float fov, float aspect);
	virtual ~Camera();

	void lookAt(const QVector3D& position, const QVector3D& focus, const QVector3D& up);
	QMatrix4x4 view() const;
	QMatrix4x4 invView() const;
	QVector3D position() const;
	QVector3D focus() const;
	QVector3D direction() const;
	QVector3D up() const;
	QVector3D worldUp() const;
	QVector3D right() const;
	float fov() const;
	void setFov(float fov);
	float aspect() const;
	void setAspect(float aspect);

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
	float _aspect;
	mutable bool _invViewDirty = true;
};
