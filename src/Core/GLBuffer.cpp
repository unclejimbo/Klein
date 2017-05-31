#include "Core/GLBuffer.h"

GLBuffer::GLBuffer() = default;

GLBuffer::GLBuffer(QOpenGLBuffer::Type type)
	: QOpenGLBuffer(type)
{
}

GLBuffer::GLBuffer(const GLBuffer& other) = default;

GLBuffer::~GLBuffer()
{
	this->destroy();
}

void GLBuffer::attachGeom(unsigned type, unsigned id)
{
	_geomType = type;
	_geomID = id;
}

unsigned GLBuffer::attachedGeomType() const
{
	return _geomType;
}

unsigned GLBuffer::attachedGeomID() const
{
	return _geomID;
}
