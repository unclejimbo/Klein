#include "Core/GraphicsComponent.h"
#include "Core/Logger.h"
#include "GraphicsComponent.h"

GraphicsComponent::GraphicsComponent(QOpenGLWidget& context, bool transparent, int layer)
	: _context(context), _transparent(transparent), _layer(layer)
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

bool GraphicsComponent::visible() const
{
	return _visible;
}

void GraphicsComponent::setVisible(bool visible)
{
	_visible = visible;
}

bool GraphicsComponent::lit() const
{
	return _lit;
}

void GraphicsComponent::setLit(bool lit)
{
	_lit = lit;
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

int GraphicsComponent::layer() const
{
	return _layer;
}

void GraphicsComponent::setLayer(bool layer)
{
	_layer = layer;
}
