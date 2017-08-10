#include "Core/GraphicsComponent.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"

PickingInfo::PickingInfo() = default;

PickingInfo::PickingInfo(unsigned buffer[4])
{
	bufferSpec = buffer[0] >> 16;
	primitiveType = buffer[0] & 0x0000ffff;
	nodeID = buffer[1];
	bufferID = buffer[2];
	primitiveID = buffer[3];
}

PickingInfo::PickingInfo(const PickingInfo & info) = default;

PickingInfo::~PickingInfo() = default;

GraphicsComponent::GraphicsComponent(QOpenGLWidget& context, bool transparent, int layer)
	: _context(context), _transparent(transparent), _layer(layer)
{
	_context.makeCurrent();
	this->initializeOpenGLFunctions();
	_context.doneCurrent();
}

GraphicsComponent::~GraphicsComponent()
{
}

bool GraphicsComponent::transparent() const
{
	return _transparent;
}

float GraphicsComponent::transparency() const
{
	return _transparency;
}

bool GraphicsComponent::setTransparency(float transparency)
{
	if (_transparent) {
		_transparency = transparency;
		return true;
	}
	else {
		KLEIN_LOG_WARNING("Unable to set transparency because this component is not transparent by construction");
		return false;
	}
}

int GraphicsComponent::layer() const
{
	return _layer;
}

void GraphicsComponent::setLayer(bool layer)
{
	_layer = layer;
}

bool GraphicsComponent::visible() const
{
	return _visible;
}

void GraphicsComponent::setVisible(bool visible)
{
	_visible = visible;
}

bool GraphicsComponent::unlit() const
{
	return _unlit;
}

void GraphicsComponent::setUnlit(bool unlit)
{
	_unlit = unlit;
}

bool GraphicsComponent::setShaderLit(const std::string & shaderID)
{
	auto shader = ResourceManager::instance().shaderProgram(shaderID);
	if (shader != nullptr) {
		_shaderLit = shader;
		return true;
	}
	else {
		return false;
	}
}

bool GraphicsComponent::setShaderUnlit(const std::string & shaderID)
{
	auto shader = ResourceManager::instance().shaderProgram(shaderID);
	if (shader != nullptr) {
		_shaderUnlit = shader;
		return true;
	}
	else {
		return false;
	}
}

MeshRenderMode GraphicsComponent::meshRenderMode() const
{
	return _meshRenderMode;
}

void GraphicsComponent::setMeshRenderMode(MeshRenderMode mode)
{
	_meshRenderMode = mode;
}

PrimitiveRenderMode GraphicsComponent::primitiveRenderMode() const
{
	return _primitiveRenderMode;
}

void GraphicsComponent::setPrimitiveRenderMode(PrimitiveRenderMode mode)
{
	_primitiveRenderMode = mode;
}

int GraphicsComponent::renderPass() const
{
	return _renderPass;
}

void GraphicsComponent::addRenderPass(int renderPass)
{
	_renderPass |= renderPass;
}

void GraphicsComponent::removeRenderPass(int renderPass)
{
	_renderPass = _renderPass & (~renderPass);
}

void GraphicsComponent::setRenderPass(int renderPass)
{
	_renderPass = renderPass;
}

void GraphicsComponent::render(const Camera& camera,
	const std::array<Light, KLEIN_MAX_LIGHTS>& lights,
	float aspectRatio,
	MeshRenderMode meshRenderMode,
	PrimitiveRenderMode primitiveRenderMode)
{
	auto meshMode = _meshRenderMode == MeshRenderMode::global ?
		meshRenderMode : _meshRenderMode;
	auto primitiveMode = _primitiveRenderMode == PrimitiveRenderMode::global ?
		primitiveRenderMode : _primitiveRenderMode;
	if (!_unlit) {
		_renderLit(camera, lights, aspectRatio, meshMode, primitiveMode);
	}
	else {
		_renderUnlit(camera, aspectRatio, meshMode, primitiveMode);
	}
}

void GraphicsComponent::renderPick(PickingPrimitive primitive,
	Camera& camera, float aspectRatio)
{
	if (primitive == PICKING_PRIMITIVE_VERTEX) {
		_renderPickVertex(camera, aspectRatio);
	}
	if (primitive == PICKING_PRIMITIVE_LINE) {
		_renderPickLine(camera, aspectRatio);
	}
	if (primitive == PICKING_PRIMITIVE_FACE) {
		_renderPickFace(camera, aspectRatio);
	}
}

void GraphicsComponent::_renderPickVertex(const Camera& camera, float aspectRatio)
{
}

void GraphicsComponent::_renderPickLine(const Camera& camera, float aspectRatio)
{
}

void GraphicsComponent::_renderPickFace(const Camera& camera, float aspectRatio)
{
}
