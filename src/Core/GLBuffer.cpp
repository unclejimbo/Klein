#include "Core/GLBuffer.h"

GLBuffer::GLBuffer(unsigned bufferSpec, QOpenGLBuffer::Type type)
	: QOpenGLBuffer(type), _bufferSpec(bufferSpec)
{
}

GLBuffer::GLBuffer(const GLBuffer& other) = default;

GLBuffer::~GLBuffer() = default;

unsigned GLBuffer::bufferSpec() const
{
	return _bufferSpec;
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
