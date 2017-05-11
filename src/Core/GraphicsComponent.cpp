#include "Core/GraphicsComponent.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"
#include "GraphicsComponent.h"

GraphicsComponent::GraphicsComponent(QOpenGLWidget& context, bool transparent, int layer)
	: _context(context), _transparent(transparent), _layer(layer)
{
	_context.makeCurrent();
	this->initializeOpenGLFunctions();
	_context.doneCurrent();
}

GraphicsComponent::GraphicsComponent(QOpenGLWidget& context,
	GeomType geomType, unsigned geomID, bool transparent, int layer)
	: _context(context), _geomType(geomType), _geomID(geomID),
	_transparent(transparent), _layer(layer)
{
	if (geomType == GEOM_TYPE_MESH) {
		ResourceManager::instance().mesh(geomID)->attachTo(this);
	}
	if (geomType == GEOM_TYPE_POINTCLOUD) {
		
	}
	_context.makeCurrent();
	this->initializeOpenGLFunctions();
	_context.doneCurrent();
}

GraphicsComponent::GraphicsComponent(SceneNode* node, QOpenGLWidget& context, bool transparent, int layer)
	: Component(node), _context(context), _transparent(transparent), _layer(layer)
{
	_context.makeCurrent();
	this->initializeOpenGLFunctions();
	_context.doneCurrent();
}

GraphicsComponent::~GraphicsComponent() = default;

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

ShadingMethod GraphicsComponent::shadingMethod() const
{
	return _shading;
}

void GraphicsComponent::setShadingMethod(ShadingMethod shading)
{
	_shading = shading;
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

void GraphicsComponent::render(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights)
{
	if (!_unlit) {
		_renderLit(camera, lights);
	}
	else {
		_renderUnlit(camera);
	}
}

void GraphicsComponent::renderPick(PickingPrimitive primitive, Camera& camera)
{
	if (primitive == PICKING_PRIMITIVE_VERTEX) {
		_renderPickVertex(camera);
	}
	if (primitive == PICKING_PRIMITIVE_LINE) {
		_renderPickLine(camera);
	}
	if (primitive == PICKING_PRIMITIVE_FACE) {
		_renderPickFace(camera);
	}
}

void GraphicsComponent::_renderPickVertex(const Camera& camera)
{
}

void GraphicsComponent::_renderPickLine(const Camera& camera)
{
}

void GraphicsComponent::_renderPickFace(const Camera& camera)
{
}
