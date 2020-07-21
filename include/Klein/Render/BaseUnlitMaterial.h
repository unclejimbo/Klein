#ifndef KLEIN_BASEUNLIT_H
#define KLEIN_BASEUNLIT_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QMatrix4x4>
#include <QVariant>
#include <QVector3D>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTexture>

namespace Qt3DRender
{
class QEfffect;
class QFrameGraphNode;
class QShaderProgram;
} // namespace Qt3DRender

namespace Klein
{

class KLEIN_API BaseUnlitMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT

public:
    enum RenderModeBits : char
    {
        RENDER_MODE_BASE_COLOR = 0x0,
        RENDER_MODE_BASE_COLOR_MAP = 0x1,
        RENDER_MODE_VCOLOR = 0x3
    };
    using RenderMode = char;

public:
    explicit BaseUnlitMaterial(Qt3DCore::QNode* parent = nullptr);

    QColor baseColor() const { return m_baseColor->value().value<int>(); }

    const Qt3DRender::QAbstractTexture* baseColorMap() const
    {
        return m_baseColorMap->value().value<Qt3DRender::QAbstractTexture*>();
    }

    RenderMode renderMode() const
    {
        return m_renderMode->value().value<RenderMode>();
    }

    float shift() const { return m_shift->value().value<float>(); }

    float texCoordOffset() const
    {
        return m_texCoordOffset->value().value<float>();
    }

    float texCoordScale() const
    {
        return m_texCoordScale->value().value<float>();
    }

public slots:
    void setBaseColor(const QColor& value) { m_baseColor->setValue(value); }

    void setBaseColorMap(Qt3DRender::QAbstractTexture* value)
    {
        m_baseColorMap->setValue(QVariant::fromValue(value));
    }

    void setRenderMode(BaseUnlitMaterial::RenderMode value)
    {
        m_renderMode->setValue(value);
    }

    void setShift(float value) { m_shift->setValue(value); }

    void setTexCoordOffset(float value) { m_texCoordOffset->setValue(value); }

    void setTexCoordScale(float value) { m_texCoordScale->setValue(value); }

public:
    static Qt3DRender::QFrameGraphNode* attachRenderPassTo(
        Qt3DRender::QFrameGraphNode* parent);

    static Qt3DRender::QFrameGraphNode* attachRenderPassTo(
        Qt3DRender::QFrameGraphNode* parent,
        Qt3DRender::QAbstractTexture* shadowMap,
        const QMatrix4x4& lightSpaceMatrix,
        const QVector3D& lightDir,
        float shadowFilterScale);

    static Qt3DRender::QFrameGraphNode* attachShadowPassTo(
        Qt3DRender::QFrameGraphNode* parent);

protected:
    static Qt3DRender::QEffect* createEffectImpl(
        Qt3DRender::QShaderProgram* shader);

protected:
    Qt3DRender::QParameter* m_baseColor = nullptr;
    Qt3DRender::QParameter* m_baseColorMap = nullptr;
    Qt3DRender::QParameter* m_renderMode = nullptr;
    Qt3DRender::QParameter* m_shift = nullptr;
    Qt3DRender::QParameter* m_texCoordOffset = nullptr;
    Qt3DRender::QParameter* m_texCoordScale = nullptr;
};

} // namespace Klein

Q_DECLARE_METATYPE(Klein::BaseUnlitMaterial::RenderModeBits)

#endif
