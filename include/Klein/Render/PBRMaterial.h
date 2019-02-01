#ifndef KLEIN_PBRMATERIAL_H
#define KLEIN_PBRMATERIAL_H

#include <Klein/Core/Export.h>

#include <QColor>
#include <QVariant>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTexture>

namespace Klein
{

class KLEIN_API PBRMaterial : public Qt3DRender::QMaterial
{
public:
    explicit PBRMaterial(Qt3DCore::QNode* parent = nullptr);

    float ambientness() const { return m_ambientness->value().value<float>(); }

    QColor baseColor() const { return m_baseColor->value().value<QColor>(); }

    Qt3DRender::QTexture2D* baseColorMap() const
    {
        return m_baseColorMap->value().value<Qt3DRender::QTexture2D*>();
    }

    float metalness() const { return m_metalness->value().value<float>(); }

    float roughness() const { return m_roughness->value().value<float>(); }

    float texCoordOffset() const
    {
        return m_texCoordOffset->value().value<float>();
    }

    float texCoordScale() const
    {
        return m_texCoordScale->value().value<float>();
    }

    bool useTexture() const { return m_useTexture->value().value<float>(); }

public slots:
    void setAmbientness(float value) { m_ambientness->setValue(value); }

    void setBaseColor(const QColor& value) { m_baseColor->setValue(value); }

    void setBaseColorMap(Qt3DRender::QTexture2D* value)
    {
        m_baseColorMap->setValue(QVariant::fromValue(value));
    }

    void setMetalness(float value) { m_metalness->setValue(value); }

    void setRoughness(float value) { m_roughness->setValue(value); }

    void setTexCoordOffset(float value) { m_texCoordOffset->setValue(value); }

    void setTexCoordScale(float value) { m_texCoordScale->setValue(value); }

    void setUseTexture(bool value) { m_useTexture->setValue(value); }

private:
    Qt3DRender::QParameter* m_ambientness;
    Qt3DRender::QParameter* m_baseColor;
    Qt3DRender::QParameter* m_baseColorMap;
    Qt3DRender::QParameter* m_metalness;
    Qt3DRender::QParameter* m_roughness;
    Qt3DRender::QParameter* m_texCoordOffset;
    Qt3DRender::QParameter* m_texCoordScale;
    Qt3DRender::QParameter* m_useTexture;
};

} // namespace Klein

#endif
