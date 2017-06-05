#pragma once

#include "Core/Component.h"
#include "Core/GeomIO.h"
#include "Core/Camera.h"
#include "Core/Light.h"
#include "Core/Common.h"

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <array>

enum class ShadingMethod
{
	shaded,
	wireframe,
	hiddenLine
};

enum PickingPrimitive : unsigned
{
	PICKING_PRIMITIVE_NONE = 0,
	PICKING_PRIMITIVE_VERTEX,
	PICKING_PRIMITIVE_LINE,
	PICKING_PRIMITIVE_FACE
};

enum RenderPass : unsigned
{
	RENDER_NONE = 0x0,
	RENDER_ONSCREEN = 0x1,
	RENDER_OFFSCREEN = 0x10,
	RENDER_PICK = 0x100
};

struct PickingInfo
{
	PickingInfo();
	explicit PickingInfo(unsigned buffer[4]);
	PickingInfo(const PickingInfo& info);
	~PickingInfo();

	unsigned bufferSpec;
	unsigned primitiveType;
	unsigned nodeID;
	unsigned bufferID;
	unsigned primitiveID;
};
Q_DECLARE_METATYPE(PickingInfo);

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
	bool unlit() const;
	void setUnlit(bool unlit);
	bool setShaderLit(const std::string& shaderID);
	bool setShaderUnlit(const std::string& shaderID);
	ShadingMethod shadingMethod() const;
	void setShadingMethod(ShadingMethod shading);
	int renderPass() const;
	void addRenderPass(int renderPass);
	void removeRenderPass(int renderPass);
	void setRenderPass(int renderPass);
	
	void render(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights);
	void renderPick(PickingPrimitive primitive, Camera& camera);

protected:
	QOpenGLShaderProgram* _shaderLit;
	QOpenGLShaderProgram* _shaderUnlit;

private:
	virtual void _renderLit(const Camera& camera, const std::array<Light, KLEIN_MAX_LIGHTS>& lights) = 0;
	virtual void _renderUnlit(const Camera& camera) = 0;
	virtual void _renderPickVertex(const Camera& camera);
	virtual void _renderPickLine(const Camera& camera);
	virtual void _renderPickFace(const Camera& camera);

private:
	QOpenGLWidget& _context;
	bool _transparent;
	float _transparency = 1.0f;
	int _layer;
	bool _visible = true;
	bool _unlit = false;
	ShadingMethod _shading = ShadingMethod::shaded;
	int _renderPass = RENDER_ONSCREEN;
};
