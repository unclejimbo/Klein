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
    enum ColorMode : int
    {
        BASECOLOR_MODE = 0,
        TEXTURE_MODE,
        VCOLOR_MODE
    };

public:
    explicit BaseUnlitMaterial(Qt3DCore::QNode* parent = nullptr);

    BaseUnlitMaterial(ColorMode mode, Qt3DCore::QNode* parent = nullptr);

    virtual ~BaseUnlitMaterial() = default;

    QColor baseColor() const { return m_baseColor->value().value<int>(); }

    const Qt3DRender::QAbstractTexture* baseColorMap() const
    {
        return m_baseColorMap->value().value<Qt3DRender::QAbstractTexture*>();
    }

    ColorMode colorMode() const
    {
        return m_colorMode->value().value<ColorMode>();
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
        if (!m_baseColorMapInitialized) {
            m_baseColorMap = new Qt3DRender::QParameter(
                QStringLiteral("baseColorMap"), value, this);
            this->addParameter(m_baseColorMap);
            m_baseColorMapInitialized = true;
        }
        else {
            m_baseColorMap->setValue(QVariant::fromValue(value));
        }
    }

    void setColorMode(ColorMode value) { m_colorMode->setValue(value); }

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
    bool m_baseColorMapInitialized = false;
    Qt3DRender::QParameter* m_baseColor;
    Qt3DRender::QParameter* m_baseColorMap;
    Qt3DRender::QParameter* m_colorMode;
    Qt3DRender::QParameter* m_shift;
    Qt3DRender::QParameter* m_texCoordOffset;
    Qt3DRender::QParameter* m_texCoordScale;
};

} // namespace Klein

Q_DECLARE_METATYPE(Klein::BaseUnlitMaterial::ColorMode)

#endif