#pragma once

#include <QOpenGLBuffer>

class GLBuffer : public QOpenGLBuffer
{
public:
	GLBuffer();
	GLBuffer(unsigned bufferSpec, QOpenGLBuffer::Type type = QOpenGLBuffer::VertexBuffer);

	unsigned bufferSpec() const;
	void attachGeom(unsigned type, unsigned id);
	unsigned attachedGeomType() const;
	unsigned attachedGeomID() const;

private:
	unsigned _bufferSpec = 0;
	unsigned _geomType = 0;
	unsigned _geomID = 0;
};
