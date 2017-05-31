#pragma once

#include <QOpenGLBuffer>

class GLBuffer : public QOpenGLBuffer
{
public:
	GLBuffer();
	explicit GLBuffer(QOpenGLBuffer::Type type);
	GLBuffer(const GLBuffer& other);
	~GLBuffer();

	void attachGeom(unsigned type, unsigned id);
	unsigned attachedGeomType() const;
	unsigned attachedGeomID() const;

private:
	unsigned _geomType = 0;
	unsigned _geomID = 0;
};
