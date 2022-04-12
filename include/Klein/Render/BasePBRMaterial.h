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
    enum RenderModeBits : char
    {
        RENDER_MODE_BASE_COLOR = 0x0,
        RENDER_MODE_BASE_COLOR_MAP = 0x1,
        RENDER_MODE_VCOLOR = 0x3,
        RENDER_MODE_NORMAL_MAP = 0x4,
        RENDER_MODE_MATERIAL_MAP = 0x8
    };
    using RenderMode = char;

public:
    explicit BasePBRMaterial(Qt3DCore::QNode* parent = nullptr);

    QColor baseColor() const { return m_baseColor->value().value<int>(); }

    const Qt3DRender::QAbstractTexture* baseColorMap() const
    {
        return m_baseColorMap->value().value<Qt3DRender::QAbstractTexture*>();
    }

    float clearCoat() const { return m_clearCoat->value().toFloat(); }

    float clearCoatRoughness() const
    {
        return m_clearCoatRoughness->value().toFloat();
    }

    const Qt3DRender::QAbstractTexture* envLightBrdf() const
    {
        return m_envLightBrdf->value().value<Qt3DRender::QAbstractTexture*>();
    }

    float envLightIntensity() const
    {
        return m_envLightIntensity->value().toFloat();
    }

    int envLightMipLevels() const
    {
        return m_envLightMipLevels->value().toInt();
    }

    int envLightMipOffset() const
    {
        return m_envLightMipOffset->value().toInt();
    }

    bool isShadowCastingEnabled() const { return m_shadowCastingEnabled; }

    bool isShadowReceivingEnabled() const
    {
        return m_receiveShadow->value().value<bool>();
    }

    float metalness() const { return m_metalness->value().value<float>(); }

    const Qt3DRender::QAbstractTexture* metalnessMap() const
    {
        return m_metalnessMap->value().value<Qt3DRender::QAbstractTexture*>();
    }

    const Qt3DRender::QAbstractTexture* normalMap() const
    {
        return m_normalMap->value().value<Qt3DRender::QAbstractTexture*>();
    }

    RenderMode renderMode() const
    {
        return m_renderMode->value().value<RenderMode>();
    }

    float roughness() const { return m_roughness->value().value<float>(); }

    const Qt3DRender::QAbstractTexture* roughnessMap() const
    {
        return m_roughnessMap->value().value<Qt3DRender::QAbstractTexture*>();
    }

    float shift() const { return m_shift->value().value<float>(); }

    float specular() const { return m_specular->value().value<float>(); }

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

    void setClearCoat(float value) { m_clearCoat->setValue(value); }

    void setClearCoatRoughness(float value)
    {
        m_clearCoatRoughness->setValue(value);
    }

    void setEnvLightBrdf(Qt3DRender::QAbstractTexture* value)
    {
        m_envLightBrdf->setValue(QVariant::fromValue(value));
    }

    void setEnvLightIntensity(float value)
    {
        m_envLightIntensity->setValue(value);
    }

    void setEnvLightMipLevels(int value)
    {
        m_envLightMipLevels->setValue(value);
    }

    void setEnvLightMipOffset(int value)
    {
        m_envLightMipOffset->setValue(value);
    }

    void setMetalness(float value) { m_metalness->setValue(value); }

    void setMetalnessMap(Qt3DRender::QAbstractTexture* value)
    {
        m_metalnessMap->setValue(QVariant::fromValue(value));
    }

    void setNormalMap(Qt3DRender::QAbstractTexture* value)
    {
        m_normalMap->setValue(QVariant::fromValue(value));
    }

    void setRenderMode(BasePBRMaterial::RenderMode value)
    {
        m_renderMode->setValue(value);
    }

    void setRoughness(float value) { m_roughness->setValue(value); }

    void setRoughnessMap(Qt3DRender::QAbstractTexture* value)
    {
        m_roughnessMap->setValue(QVariant::fromValue(value));
    }

    void setShadowCastingEnabled(bool value)
    {
        m_shadowCastingEnabled = value;
        this->setEffect(getEffectVariant(value));
    }

    void setShadowReceivingEnabled(bool value)
    {
        m_receiveShadow->setValue(value);
    }

    void setShift(float value) { m_shift->setValue(value); }

    void setSpecular(float value) { m_specular->setValue(value); }

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
    virtual Qt3DRender::QEffect* getEffectVariant(bool castShadow) = 0;

protected:
    static Qt3DRender::QEffect* createEffectImpl(
        Qt3DRender::QShaderProgram* shader,
        bool castShadow);

protected:
    Qt3DRender::QParameter* m_baseColor = nullptr;
    Qt3DRender::QParameter* m_baseColorMap = nullptr;
    Qt3DRender::QParameter* m_clearCoat = nullptr;
    Qt3DRender::QParameter* m_clearCoatRoughness = nullptr;
    Qt3DRender::QParameter* m_envLightBrdf = nullptr;
    Qt3DRender::QParameter* m_envLightIntensity = nullptr;
    Qt3DRender::QParameter* m_envLightMipLevels = nullptr;
    Qt3DRender::QParameter* m_envLightMipOffset = nullptr;
    Qt3DRender::QParameter* m_metalness = nullptr;
    Qt3DRender::QParameter* m_metalnessMap = nullptr;
    Qt3DRender::QParameter* m_normalMap = nullptr;
    Qt3DRender::QParameter* m_receiveShadow = nullptr;
    Qt3DRender::QParameter* m_renderMode = nullptr;
    Qt3DRender::QParameter* m_roughness = nullptr;
    Qt3DRender::QParameter* m_roughnessMap = nullptr;
    Qt3DRender::QParameter* m_shift = nullptr;
    Qt3DRender::QParameter* m_specular = nullptr;
    Qt3DRender::QParameter* m_texCoordOffset = nullptr;
    Qt3DRender::QParameter* m_texCoordScale = nullptr;
    bool m_shadowCastingEnabled = false;
};

} // namespace Klein

Q_DECLARE_METATYPE(Klein::BasePBRMaterial::RenderModeBits)

#endif
