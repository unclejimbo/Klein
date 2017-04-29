#pragma once

#include "Core/Component.h"
#include "Core/Camera.h"
#include "Core/Light.h"
#include "Core/Common.h"

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <array>

enum class ShadingMethod
{
	shaded,
	wireframe,
	hiddenLine
};

enum RenderPass : int
{
	none = 0x0,
	onscreen = 0x1,
	offscreen = 0x10
};

class GraphicsComponent : public Component, public QOpenGLFunctions_4_3_Core
{
public:
	GraphicsComponent(QOpenGLWidget& context, bool transparent = false, int layer = 0);
	virtual ~GraphicsComponent();

	bool transparent() const;
	float transparency() const;
	bool setTransparency(float transparency);
	int layer() const;
	void setLayer(bool layer);
	bool visible() const;
	void setVisible(bool visible);
	bool lit() const;
	void setLit(bool lit);
	ShadingMethod shadingMethod() const;
	void setShadingMethod(ShadingMethod shading);
	int renderPass() const;
	void addRenderPass(int renderPass);
	void removeRenderPass(int renderPass);
	void setRenderPass(int renderPass);
	
	virtual void render(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights) = 0;

private:
	QOpenGLWidget& _context;
	bool _transparent;
	float _transparency = 1.0f;
	int _layer;
	bool _visible = true;
	bool _lit = true;
	ShadingMethod _shading = ShadingMethod::shaded;
	int _renderPass = onscreen;
};
