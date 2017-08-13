#pragma once

#include <QOpenGLBuffer>

class GLBuffer : public QOpenGLBuffer
{
public:
	explicit GLBuffer(unsigned bufferSpec);
	GLBuffer(unsigned bufferSpec, QOpenGLBuffer::Type type = QOpenGLBuffer::VertexBuffer);

	// Used in picking to discriminate geometry type
	unsigned bufferSpec() const;
	void attachGeom(unsigned type, unsigned id);
	unsigned attachedGeomType() const;
	unsigned attachedGeomID() const;

private:
	unsigned _bufferSpec = 0; // GL_POINTS | GL_LINES | GL_TRIANGLES
	unsigned _geomType = 0;
	unsigned _geomID = 0;
};
