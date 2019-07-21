#ifndef KLEIN_BASEPBRMATERIAL_H
#define KLEIN_BASEPBRMATERIAL_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QMatrix4x4>
#include <QVariant>
#include <QVector3D>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QTexture>

namespace Qt3DRender
{
class QEfffect;
class QFrameGraphNode;
class QShaderProgram;
} // namespace Qt3DRender

namespace Klein
{

class KLEIN_API BasePBRMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT

public:
    enum RenderModeBits : int
    {
        RENDER_MODE_BASE_COLOR = 0x0,
        RENDER_MODE_TEXTURE = 0x1,
        RENDER_MODE_VCOLOR = 0x3,
        RENDER_MODE_NORMAL_MAP = 0x4,
        RENDER_MODE_MATERIAL_MAP = 0x8
    };
    using RenderMode = int;

public:
    explicit BasePBRMaterial(Qt3DCore::QNode* parent = nullptr);

    virtual ~BasePBRMaterial() = default;

    QColor baseColor() const { return m_baseColor->value().value<int>(); }

    const Qt3DRender::QAbstractTexture* baseColorMap() const
    {
        return m_baseColorMap->value().value<Qt3DRender::QAbstractTexture*>();
    }

    const Qt3DRender::QAbstractTexture* envLightBrdf() const
    {
        return m_envLightBrdf->value().value<Qt3DRender::QAbstractTexture*>();
    }

    float envLightIntensity() const
    {
        return m_envLightIntensity->value().toFloat();
    }

    bool isShadowCastingEnabled() const { return m_shadowPass->isEnabled(); }

    bool isShadowReceivingEnabled() const
    {
        return m_receiveShadow->value().value<bool>();
    }

    float metalness() const { return m_metalness->value().value<float>(); }

    RenderMode renderMode() const
    {
        return m_renderMode->value().value<RenderMode>();
    }

    float roughness() const { return m_roughness->value().value<float>(); }

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
        if (!m_baseColorMap) {
            m_baseColorMap = new Qt3DRender::QParameter(
                QStringLiteral("baseColorMap"), value, this);
            this->addParameter(m_baseColorMap);
        }
        else {
            m_baseColorMap->setValue(QVariant::fromValue(value));
        }
    }

    void setEnvLightBrdf(Qt3DRender::QAbstractTexture* value)
    {
        if (!m_envLightBrdf) {
            m_envLightBrdf = new Qt3DRender::QParameter(
                QStringLiteral("envLight.brdf"), value, this);
            this->addParameter(m_envLightBrdf);
        }
        else {
            m_envLightBrdf->setValue(QVariant::fromValue(value));
        }
    }

    void setEnvLightIntensity(float value)
    {
        m_envLightIntensity->setValue(value);
    }

    void setMetalness(float value) { m_metalness->setValue(value); }

    void setRenderMode(RenderMode value) { m_renderMode->setValue(value); }

    void setRoughness(float value) { m_roughness->setValue(value); }

    void setShadowCastingEnabled(bool value)
    {
        m_shadowPass->setEnabled(value);
    }

    void setShadowReceivingEnabled(bool value)
    {
        m_receiveShadow->setValue(value);
    }

    void setShift(float value) { m_shift->setValue(value); }

    void setTexCoordOffset(float value) { m_texCoordOffset->setValue(value); }

    void setTexCoordScale(float value) { m_texCoordScale->setValue(value); }

public:
    static Qt3DRender::QFrameGraphNode* attachRenderPassTo(
        Qt3DRender::QFrameGraphNode* parent,
        Qt3DRender::QAbstractTexture* shadowMap,
        const QMatrix4x4& lightSpaceMatrix,
        const QVector3D& lightDir,
        float shadowFilterScale);

    static Qt3DRender::QFrameGraphNode* attachShadowPassTo(
        Qt3DRender::QFrameGraphNode* parent);

protected:
    Qt3DRender::QEffect* createEffect(Qt3DRender::QShaderProgram* shader);

protected:
    Qt3DRender::QParameter* m_baseColor = nullptr;
    Qt3DRender::QParameter* m_baseColorMap = nullptr;
    Qt3DRender::QParameter* m_envLightBrdf = nullptr;
    Qt3DRender::QParameter* m_envLightIntensity = nullptr;
    Qt3DRender::QParameter* m_metalness = nullptr;
    Qt3DRender::QParameter* m_receiveShadow = nullptr;
    Qt3DRender::QParameter* m_renderMode = nullptr;
    Qt3DRender::QParameter* m_roughness = nullptr;
    Qt3DRender::QParameter* m_shift = nullptr;
    Qt3DRender::QParameter* m_texCoordOffset = nullptr;
    Qt3DRender::QParameter* m_texCoordScale = nullptr;
    Qt3DRender::QRenderPass* m_shadowPass = nullptr;
};

} // namespace Klein

Q_DECLARE_METATYPE(Klein::BasePBRMaterial::RenderModeBits)

#endif
